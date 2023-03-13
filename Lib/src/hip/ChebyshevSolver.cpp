/* Copyright 2016 Kristofer Björnson
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file ChebyshevExpander.cu
 *
 *  @author Kristofer Björnson
 */

//Flag used to work around incompatibilities between nlohmann::json and CUDA.
//This disables code in header files that depends on nlohmann::json.
// #define TBTK_DISABLE_NLOHMANN_JSON //TODO compiles fine without the flag?

#include "TBTK/Solver/ChebyshevExpander.h"
#include "TBTK/GPUResourceManager.h"
#include "TBTK/HALinkedList.h"
#include "TBTK/Streams.h"
#include "TBTK/TBTKMacros.h"

#define __HIP_PLATFORM_AMD__ //TODO set this during build

#include <hip/hip_runtime.h>
#include <hip/hip_complex.h>
#include <hipsparse.h>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/complex.h>
#include <thrust/device_ptr.h>

#include <cmath>

using namespace std;

namespace TBTK{
namespace Solver{

complex<double> minus_one(-1., 0.);
complex<double> one(1., 0.);
complex<double> two(2., 0.);
complex<double> zero(0., 0.);
complex<double> i(0., 1.);




__global__
void extractCoefficients(
	complex<double>* jResult,
	complex<double>* coefficients,
	int currentCoefficient,
	int* coefficientMap,
	int numParallelCoefficients,
	int numCoefficients
){
	for (int to = blockIdx.x * blockDim.x + threadIdx.x; 
		to < numParallelCoefficients; 
		to += blockDim.x * gridDim.x)
	{
		int iterator = to*numCoefficients + currentCoefficient;
		coefficients[iterator] = jResult[coefficientMap[to]];
	}
}



vector<complex<double>> ChebyshevExpander::calculateCoefficientsGPU(
	Index to,
	Index from
){
	vector<Index> toVector;
	toVector.push_back(to);

	return calculateCoefficientsGPU(toVector, from)[0];
}

vector<
	vector<std::complex<double>>
> ChebyshevExpander::calculateCoefficientsGPU(
	vector<Index> &to,
	Index from
){
	complex<double> *damping = NULL; //TODO Compiler error of damping not defined without this, note that it disables some parts of the code?
	TBTKAssert(
		scaleFactor > 0,
		"ChebyshevExpander::calculateCoefficientsGPU()",
		"Scale factor must be larger than zero.",
		"Use ChebyshevExpander::setScaleFactor() to set scale factor."
	);
	TBTKAssert(
		numCoefficients > 0,
		"ChebyshevExpander::calculateCoefficients()",
		"numCoefficients has to be larger than zero.",
		""
	);

	int device = GPUResourceManager::getInstance().allocateDevice();

	TBTKAssert(
		hipSetDevice(device) == hipSuccess,
		"ChebyshevExpander::calculateCoefficientsGPU()",
		"CUDA set device error for device " << device << ".",
		""
	);

	vector<vector<complex<double>>> coefficients;
	for(unsigned int n = 0; n < to.size(); n++){
		coefficients.push_back(vector<complex<double>>(numCoefficients,0));
	}

	const HoppingAmplitudeSet &hoppingAmplitudeSet
		= getModel().getHoppingAmplitudeSet();

	int fromBasisIndex = hoppingAmplitudeSet.getBasisIndex(from);
	thrust::host_vector<int> coefficientMap(to.size());

	for(int n = 0; n < (int)to.size(); n++){
		coefficientMap[n] =
			hoppingAmplitudeSet.getBasisIndex(to.at(n));
	}

	if(getGlobalVerbose() && getVerbose()){
		Streams::out << "ChebyshevExpander::calculateCoefficientsGPU\n";
		Streams::out << "\tFrom Index: " << fromBasisIndex << "\n";
		Streams::out << "\tBasis size: "
			<< hoppingAmplitudeSet.getBasisSize() << "\n";
		Streams::out << "\tUsing damping: ";
		if(damping != NULL)
			Streams::out << "Yes\n";
		else
			Streams::out << "No\n";
	}

	int basisSize = hoppingAmplitudeSet.getBasisSize();
	thrust::host_vector<complex<double>> jIn1(basisSize, 0.);
	thrust::host_vector<complex<double>> jIn2(basisSize, 0.);

	//Set up initial state (|j0>)
	jIn1[fromBasisIndex] = 1.;


	for(int n = 0; n < (int)to.size(); n++)
			coefficients[0][n] = jIn1[coefficientMap[n]];

	SparseMatrix<complex<double>> sparseMatrix = hoppingAmplitudeSet.getSparseMatrix();
	sparseMatrix.setStorageFormat(SparseMatrix<complex<double>>::StorageFormat::CSR);
	const int numHoppingAmplitudes = sparseMatrix.getCSRNumMatrixElements();
	const int numRows = sparseMatrix.getNumRows();
	const unsigned int *csrRowPointers = sparseMatrix.getCSRRowPointers();
	const unsigned int *csrColumns = sparseMatrix.getCSRColumns();
	const complex<double> *csrValues = sparseMatrix.getCSRValues();

	// calculate total memory requirement for the device
	int totalMemoryRequirement
		= hoppingAmplitudeSet.getBasisSize()*sizeof(complex<double>);
	totalMemoryRequirement += hoppingAmplitudeSet.getBasisSize()*sizeof(
		complex<double>
	);
	totalMemoryRequirement += numHoppingAmplitudes*sizeof(int);
	totalMemoryRequirement += hoppingAmplitudeSet.getBasisSize()*sizeof(
		int
	);
	totalMemoryRequirement += numHoppingAmplitudes*sizeof(int);
	totalMemoryRequirement += numHoppingAmplitudes*sizeof(
		complex<double>
	);
	totalMemoryRequirement += to.size()*numCoefficients*sizeof(
		complex<double>
	);
	totalMemoryRequirement += hoppingAmplitudeSet.getBasisSize()*sizeof(
		int
	);
	if(damping != NULL){
		totalMemoryRequirement += hoppingAmplitudeSet.getBasisSize(
		)*sizeof(complex<double>);
	}
	if(getGlobalVerbose() && getVerbose()){
		Streams::out << "\tCUDA memory requirement: ";
		if(totalMemoryRequirement < 1024){
			Streams::out << totalMemoryRequirement << "B\n";
		}
		else if(totalMemoryRequirement < 1024*1024){
			Streams::out << totalMemoryRequirement/1024 << "KB\n";
		}
		else{
			Streams::out << totalMemoryRequirement/1024/1024
				<< "MB\n";
		}
	}

	//Initialize GPU
	thrust::device_vector<complex<double>> jIn1_device = jIn1; //Already allocates and copies into the device memory
	thrust::device_vector<complex<double>> jIn2_device = jIn2;
	// int *cooHARowIndices_device;
	thrust::device_vector<int> csrHARowIndices_device(csrRowPointers,
													csrRowPointers + (numRows + 1));
	thrust::device_vector<int> csrColumns_device(csrColumns, csrColumns + numHoppingAmplitudes);
	const thrust::device_vector<complex<double>> csrValues_device(csrValues, csrValues + numHoppingAmplitudes);

	//The device coefficients are stored in a flattened array as complare to the host coefficients
	thrust::device_vector<complex<double>> coefficients_device(to.size()*numCoefficients);
	thrust::device_vector<int> coefficientMap_device = coefficientMap;
	complex<double> *damping_device = NULL;

	if(damping != NULL){
		TBTKAssert(
			hipMalloc(
				(void**)&damping_device,
				hoppingAmplitudeSet.getBasisSize()*sizeof(
					complex<double>
				)
			) == hipSuccess,
			"ChebyshevExpander::calculateCoefficientsGPU()",
			"CUDA malloc error while allocating damping_device.",
			""
		);
	}
	//Only the zeroth coefficients have been calculated, so they are copied to the device
	for(int n = 0; n < (int)to.size(); n++){
		thrust::copy(coefficients[n].begin(),
		coefficients[n].begin() + 1,
		coefficients_device.begin() + n*numCoefficients
		);
	}

	if(damping != NULL){
		TBTKAssert(
			hipMemcpy(
				damping_device,
				damping,
				hoppingAmplitudeSet.getBasisSize()*sizeof(
					complex<double>
				),
				hipMemcpyHostToDevice
			) == hipSuccess,
			"ChebyshevExpander::calculateCoefficientsGPU()",
			"CUDA memcpy error while copying damping.",
			""
		);
	}

	hipsparseHandle_t handle = NULL;
	TBTKAssert(
		hipsparseCreate(&handle) == HIPSPARSE_STATUS_SUCCESS,
		"ChebyshevExpander::calculateCoefficientsGPU()",
		"hipSPARSE create error.",
		""
	);

	//Create a sparse matrix on the device
    hipsparseSpMatDescr_t descr;
    TBTKAssert( hipsparseCreateCsr(&descr, numRows, numRows,
		numHoppingAmplitudes,
		csrHARowIndices_device.data().get(), 
		csrColumns_device.data().get(), 
		(void*) csrValues_device.data().get(),
					  HIPSPARSE_INDEX_32I, HIPSPARSE_INDEX_32I,
					  HIPSPARSE_INDEX_BASE_ZERO, HIPBLAS_C_64F)
				== HIPSPARSE_STATUS_SUCCESS,
		"ChebyshevExpander::calculateCoefficientsGPU()",
		"Error in hipsparseCreateCsr.",
		""
		);

	//Create the dense vector objects on the device
	hipsparseDnVecDescr_t vecJIn1, vecJIn2;
	TBTKAssert(
		hipsparseCreateDnVec(&vecJIn1, basisSize, jIn1_device.data().get(), HIPBLAS_C_64F)
		== HIPSPARSE_STATUS_SUCCESS,
		"ChebyshevExpander::calculateCoefficientsGPU()",
		"Error in hipsparseCreateDnVec.",
		""
	);
	TBTKAssert(
		hipsparseCreateDnVec(&vecJIn2, basisSize, jIn2_device.data().get(), HIPBLAS_C_64F)
		== HIPSPARSE_STATUS_SUCCESS,
		"ChebyshevExpander::calculateCoefficientsGPU()",
		"Error in hipsparseCreateDnVec.",
		""
	);



	//Calculate |j1>
	int block_size = to.size();
	int num_blocks = 1;
	int maxNumThreads = 1024;
	if(block_size > maxNumThreads){
		block_size = maxNumThreads;
		num_blocks = 1 + ((to.size() - 1) / maxNumThreads); // ceil(to.size()/maxNumThreads)
	}
	
	if(getGlobalVerbose() && getVerbose()){
		Streams::out << "\tCUDA Block size: " << block_size << "\n";
		Streams::out << "\tCUDA Num blocks: " << num_blocks << "\n";
	}


	complex<double> multiplier = one/scaleFactor;
	//Allocate buffer memory for the hipsparseSpMV routine
    void                *buffer_device = NULL;
    size_t               bufferSize = 0;
    TBTKAssert( 
		hipsparseSpMV_bufferSize(
			handle, HIPSPARSE_OPERATION_NON_TRANSPOSE,
			&multiplier, descr, vecJIn1, &zero, vecJIn2, HIPBLAS_C_64F,
			HIPSPARSE_SPMV_CSR_ALG2, &bufferSize) 
		== HIPSPARSE_STATUS_SUCCESS,
		"ChebyshevExpander::calculateCoefficientsGPU()",
		"Error in hipsparseSpMV_bufferSize.",
		"Error occured while allocating extra buffer in device memory"
	);
    TBTKAssert(
		hipMalloc(&buffer_device, bufferSize)
		== hipSuccess,
	   "ChebyshevExpander::calculateCoefficientsGPU()",
	   "Error in hipMalloc.",
	   "Error occured while allocating extra buffer in device memory"
   	);
	TBTKAssert(
		hipsparseSpMV(
			handle,
			HIPSPARSE_OPERATION_NON_TRANSPOSE,
			&multiplier, descr, vecJIn1, 
			&zero, vecJIn2,
			HIPBLAS_C_64F,
			HIPSPARSE_SPMV_CSR_ALG2, 
			buffer_device
		) == HIPSPARSE_STATUS_SUCCESS,
		"ChebyshevExpander::calculateCoefficentsGPU()",
		"Matrix-vector multiplication error.",
		""
	);

	size_t bufferSizeSecondOperation = 0;
    TBTKAssert( 
		hipsparseSpMV_bufferSize(
			handle, HIPSPARSE_OPERATION_NON_TRANSPOSE,
			&multiplier, descr, vecJIn1, &minus_one, vecJIn2, HIPBLAS_C_64F,
			HIPSPARSE_SPMV_CSR_ALG2, &bufferSizeSecondOperation) 
		== HIPSPARSE_STATUS_SUCCESS,
		"ChebyshevExpander::calculateCoefficientsGPU()",
		"Error in hipsparseSpMV_bufferSize.",
		"Error occured while allocating extra buffer in device memory"
	);
	TBTKAssert(
		bufferSizeSecondOperation <= bufferSize,
		"ChebyshevExpander::calculateCoefficientsGPU()",
		"Error in Allocating buffer for SPMV.",
		"Buffer memory requirements changed."
	);
	extractCoefficients <<< num_blocks, block_size >>> (
		jIn2_device.data().get(),
		coefficients_device.data().get(),
		1,
		coefficientMap_device.data().get(),
		to.size(),
		numCoefficients
	);
	//Switch the order of the vectors jIn1 <-> jIn2
	hipsparseDnVecDescr_t *vecJIn1_ptr = &vecJIn1;
	hipsparseDnVecDescr_t *vecJIn2_ptr = &vecJIn2;
	hipsparseDnVecDescr_t *vecJTemp_ptr = NULL;
	thrust::device_ptr<complex<double>> jIn1_device_ptr = jIn1_device.data();
	thrust::device_ptr<complex<double>> jIn2_device_ptr = jIn2_device.data();
	thrust::device_ptr<complex<double>> jTemp_device_ptr = NULL;
	
	vecJTemp_ptr = vecJIn2_ptr;
	vecJIn2_ptr = vecJIn1_ptr;
	vecJIn1_ptr = vecJTemp_ptr;

	jTemp_device_ptr = jIn2_device_ptr;
	jIn2_device_ptr = jIn1_device_ptr;
	jIn1_device_ptr = jTemp_device_ptr;

	if(getGlobalVerbose() && getVerbose())
		Streams::out << "\tProgress (100 coefficients per dot): ";

	//Iteratively calculate |jn> and corresponding Chebyshev coefficients.
	for(int n = 2; n < numCoefficients; n++){
		multiplier = two/scaleFactor;
		TBTKAssert(
			hipsparseSpMV(
				handle,
				HIPSPARSE_OPERATION_NON_TRANSPOSE,
				&multiplier, descr, *vecJIn1_ptr, 
				&minus_one, *vecJIn2_ptr,
				HIPBLAS_C_64F,
				HIPSPARSE_SPMV_CSR_ALG2, 
				buffer_device
			) == HIPSPARSE_STATUS_SUCCESS,
			"ChebyshevExpander::calculateCoefficentsGPU()",
			"Matrix-vector multiplication error.",
			""
		);
		extractCoefficients <<< num_blocks, block_size >>> (
			jIn2_device_ptr.get(),
			coefficients_device.data().get(),
			n,
			coefficientMap_device.data().get(),
			to.size(),
			numCoefficients
		);
		vecJTemp_ptr = vecJIn2_ptr;
		vecJIn2_ptr = vecJIn1_ptr;
		vecJIn1_ptr = vecJTemp_ptr;

		jTemp_device_ptr = jIn2_device_ptr;
		jIn2_device_ptr = jIn1_device_ptr;
		jIn1_device_ptr = jTemp_device_ptr;

		if(getGlobalVerbose() && getVerbose()){
			if(n%100 == 0)
				Streams::out << "." << flush;
			if(n%1000 == 0)
				Streams::out << " " << flush;
		}
	}
	if(getGlobalVerbose() && getVerbose())
		Streams::out << "\n";
	for(unsigned int n = 0; n < to.size(); n++){
		thrust::copy(
			coefficients_device.begin() + n*numCoefficients,
			coefficients_device.begin() + numCoefficients*(n+1),
			coefficients[n].begin()
		);
	}

    TBTKAssert(
		hipsparseDestroySpMat(descr
	   ) == HIPSPARSE_STATUS_SUCCESS,
	   "ChebyshevExpander::calculateCoefficientsGPU()",
	   "Error in hipsparseDestroySpMat.",
	   "Error while destroying the handle descr stored on the device."	
   	)
	descr = NULL;

    TBTKAssert(
		hipsparseDestroyDnVec(vecJIn1
	   ) == HIPSPARSE_STATUS_SUCCESS,
	   "ChebyshevExpander::calculateCoefficientsGPU()",
	   "Error in hipsparseDestroyDnVec.",
	   "Error while destroying the dense vector vecJIn1 stored on the device."	
    )
	vecJIn1 = NULL;
    TBTKAssert(
		hipsparseDestroyDnVec(vecJIn2
	   ) == HIPSPARSE_STATUS_SUCCESS,
	   "ChebyshevExpander::calculateCoefficientsGPU()",
	   "Error in hipsparseDestroyDnVec.",
	   "Error while destroying the dense vector vecJIn2 stored on the device."	
    )
	vecJIn2 = NULL;
    TBTKAssert(
		hipFree(buffer_device
		) == hipSuccess,
		"ChebyshevExpander::calculateCoefficientsGPU()",
		"CUDA free error while deallocating buffer_device.",
		""
	)
	buffer_device = NULL;
    TBTKAssert(
		hipsparseDestroy(handle
	   ) == HIPSPARSE_STATUS_SUCCESS,
	   "ChebyshevExpander::calculateCoefficientsGPU()",
	   "Error in hipsparseDestroy.",
	   "Error while destroying the handle for the hipSparse calculation."	
    )
	handle = NULL;

	if(damping != NULL)
		hipFree(damping_device);

	GPUResourceManager::getInstance().freeDevice(device);

	//Lorentzian convolution
	if(broadening != 0){
		double lambda = broadening*numCoefficients;
		for(int n = 0; n < numCoefficients; n++){
			for(int c = 0; c < (int)to.size(); c++){
				coefficients[c][n] = coefficients[c][n]*sinh(
					lambda*(
						1 - n/(double)numCoefficients
					)
				)/sinh(lambda);
//				coefficients[n + c*numCoefficients] = coefficients[n + c*numCoefficients]*sinh(lambda*(1 - n/(double)numCoefficients))/sinh(lambda);
				//TODO it looks like the code is different here for the cpu version
			}
		}
	}

	return coefficients;
}

__global__
void calculateGreensFunction(
	hipDoubleComplex *greensFunction,
	hipDoubleComplex *coefficients,
	hipDoubleComplex *lookupTable,
	int numCoefficients,
	int energyResolution
){
	int e = blockIdx.x*blockDim.x + threadIdx.x;
	if(e < energyResolution){
		for(int n = 0; n < numCoefficients; n++){
			greensFunction[e] = hipCadd(
				greensFunction[e],
				hipCmul(
					lookupTable[n*energyResolution + e],
					coefficients[n]
				)
			);
		}
	}
}

void ChebyshevExpander::loadLookupTableGPU(){
	if(getGlobalVerbose() && getVerbose())
		Streams::out << "CheyshevExpander::loadLookupTableGPU\n";

	TBTKAssert(
		generatingFunctionLookupTable != NULL,
		"ChebyshevExpander::loadLookupTableGPU()",
		"Lookup table has not been generated.",
		"Call ChebyshevExpander::generateLokupTable() to generate"
		<< " lookup table."
	);
	if(generatingFunctionLookupTable_device != NULL)
		destroyLookupTableGPU();

	complex<double> *generatingFunctionLookupTable_host
		= new complex<double>[
			lookupTableNumCoefficients*lookupTableResolution
		];
	for(int n = 0; n < lookupTableNumCoefficients; n++){
		for(int e = 0; e < lookupTableResolution; e++){
			generatingFunctionLookupTable_host[
				n*lookupTableResolution + e
			] = generatingFunctionLookupTable[n][e];
		}
	}

	int memoryRequirement
		= lookupTableNumCoefficients*lookupTableResolution*sizeof(
			complex<double>
		);
	if(getGlobalVerbose() && getVerbose()){
		Streams::out << "\tHIP memory requirement: ";
		if(memoryRequirement < 1024)
			Streams::out << memoryRequirement << "B\n";
		else if(memoryRequirement < 1024*1024)
			Streams::out << memoryRequirement/1024 << "KB\n";
		else
			Streams::out << memoryRequirement/1024/1024 << "MB\n";
	}

	generatingFunctionLookupTable_device = new complex<double>**[
		GPUResourceManager::getInstance().getNumDevices()
	];

	for(
		int n = 0;
		n < GPUResourceManager::getInstance().getNumDevices();
		n++
	){
		TBTKAssert(
			hipSetDevice(n) == hipSuccess,
			"ChebyshevExpander::loadLookupTableGPU()",
			"HIP set device error for device " << n << ".",
			""
		);

		TBTKAssert(
			hipMalloc(
				(void**)&generatingFunctionLookupTable_device[
					n
				],
				lookupTableNumCoefficients*lookupTableResolution*sizeof(
					complex<double>
				)
			)  == hipSuccess,
			"ChebyshevExpander::loadLookupTableGPU()",
			"HIP malloc error while allocating"
			<< " generatingFunctionLookupTable_device.",
			""
		);

		TBTKAssert(
			hipMemcpy(
				generatingFunctionLookupTable_device[n],
				generatingFunctionLookupTable_host,
				lookupTableNumCoefficients*lookupTableResolution*sizeof(
					complex<double>
				),
				hipMemcpyHostToDevice
			) == hipSuccess,
			"ChebyshevExpander::loadLookupTableGPU()",
			"HIP memcpy error while copying"
			<< " generatingFunctionLookupTable_device.",
			""
		);
	}

	delete [] generatingFunctionLookupTable_host;
}

void ChebyshevExpander::destroyLookupTableGPU(){
	if(getGlobalVerbose() && getVerbose())
		Streams::out << "ChebyshevExpander::destroyLookupTableGPU\n";

	TBTKAssert(
		generatingFunctionLookupTable_device != NULL,
		"ChebyshevExpander::destroyLookupTableGPU()",
		"No lookup table loaded onto GPU.\n",
		""
	);

	for(
		int n = 0;
		n < GPUResourceManager::getInstance().getNumDevices();
		n++
	){
		hipFree(generatingFunctionLookupTable_device[n]);
	}

	delete [] generatingFunctionLookupTable_device;
	generatingFunctionLookupTable_device = NULL;
}

//Property::GreensFunction* ChebyshevExpander::generateGreensFunctionGPU(
vector<complex<double>> ChebyshevExpander::generateGreensFunctionGPU(
	const vector<complex<double>> &coefficients,
//	Property::GreensFunction::Type type
	Type type
){
	int device = GPUResourceManager::getInstance().allocateDevice();

	TBTKAssert(
		hipSetDevice(device) == hipSuccess,
		"ChebyshevExpander::generateGreensFunctionGPU()",
		"HIP set device error for device " << device << ".",
		""
	);

	ensureLookupTableIsReady();

	if(getGlobalVerbose() && getVerbose())
		Streams::out << "ChebyshevExpander::generateGreensFunctionGPU\n";

	TBTKAssert(
		generatingFunctionLookupTable_device != NULL,
		"ChebyshevExpander::generateGreensFunctionGPU()",
		"No lookup table loaded onto GPU.",
		""
	);
	TBTKAssert(
//		type == Property::GreensFunction::Type::Retarded,
		type == Type::Retarded,
		"ChebyshevExpander::generateGreensFunctionGPU()",
		"Only evaluation of retarded Green's function is implemented"
		<< " for GPU so far.",
		"Use CPU evaluation instead."
	);

/*	complex<double> *greensFunctionData = new complex<double>[
		lookupTableResolution
	];

	for(int e = 0; e < lookupTableResolution; e++)
		greensFunctionData[e] = 0.;*/

	vector<complex<double>> greensFunctionData(lookupTableResolution, 0.);

	complex<double> *greensFunctionData_device;
	complex<double> *coefficients_device;

	TBTKAssert(
		hipMalloc(
			(void**)&greensFunctionData_device,
			lookupTableResolution*sizeof(complex<double>)
		)  == hipSuccess,
		"ChebyshevExpander::generateGreensFunctionGPU()",
		"HIP malloc error while allocating greensFunction_device.",
		""
	);
	TBTKAssert(
		hipMalloc(
			(void**)&coefficients_device,
			lookupTableNumCoefficients*sizeof(complex<double>)
		)  == hipSuccess,
		"ChebyshevExpander::generateGreensFunctionGPU()",
		"HIP malloc error while allocating coefficients_device.",
		""
	);

	TBTKAssert(
		hipMemcpy(
			greensFunctionData_device,
			greensFunctionData.data(),
			lookupTableResolution*sizeof(complex<double>),
			hipMemcpyHostToDevice
		) == hipSuccess,
		"ChebyshevExpander::generateGreensFunctionGPU()",
		"HIP memcpy error while copying greensFunctionData.",
		""
	);
	TBTKAssert(
		hipMemcpy(
			coefficients_device,
			coefficients.data(),
			lookupTableNumCoefficients*sizeof(complex<double>),
			hipMemcpyHostToDevice
		) == hipSuccess,
		"ChebyshevExpander::generateGreensFunctionGPU()",
		"HIP memcpy error while copying coefficients.",
		""
	);

	int block_size = 1024;
	int num_blocks = lookupTableResolution/block_size
		+ (lookupTableResolution%block_size == 0 ? 0:1);

	if(getGlobalVerbose() && getVerbose()){
		Streams::out << "\tHIP Block size: " << block_size << "\n";
		Streams::out << "\tHIP Num blocks: " << num_blocks << "\n";
	}

	calculateGreensFunction <<< num_blocks, block_size>>> (
		(hipDoubleComplex*)greensFunctionData_device,
		(hipDoubleComplex*)coefficients_device,
		(hipDoubleComplex*)generatingFunctionLookupTable_device[device],
		lookupTableNumCoefficients,
		lookupTableResolution
	);

	TBTKAssert(
		hipMemcpy(
			greensFunctionData.data(),
			greensFunctionData_device,
			lookupTableResolution*sizeof(complex<double>),
			hipMemcpyDeviceToHost
		) == hipSuccess,
		"ChebyshevExpander::generateGreensFunctionGPU()",
		"HIP memcpy error while copying greensFunction_device.",
		""
	);

	hipFree(greensFunctionData_device);
	hipFree(coefficients_device);

	GPUResourceManager::getInstance().freeDevice(device);

/*	Property::GreensFunction *greensFunction = new Property::GreensFunction(
		type,
//		Property::GreensFunction::Format::Array,
		lookupTableLowerBound,
		lookupTableUpperBound,
		lookupTableResolution,
		greensFunctionData
	);
	delete [] greensFunctionData;

	return greensFunction;*/

	return greensFunctionData;
}

};	//End of namespace Solver
};	//End of namespace TBTK
