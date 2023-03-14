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

/** @file Diagonalizer.cu.hip
 *
 *  @author Andreas Theiler
 */

#include "TBTK/Solver/Diagonalizer.h"
#include "TBTK/Streams.h"
#include "TBTK/TBTKMacros.h"
#include "TBTK/GPUResourceManager.h"

#define __HIP_PLATFORM_AMD__ //TODO set this during build
#include <hipsolver/hipsolver.h>
#include <hip/hip_runtime.h>

using namespace std;

namespace TBTK{
namespace Solver{

void Diagonalizer::solveGPU(CArray<complex<double>>& matrix, CArray<double>& eigenValues){
    //Initialize device
    int device = GPUResourceManager::getInstance().allocateDevice();
	TBTKAssert(
		hipSetDevice(device) == hipSuccess,
		"Diagonalizer::solveGPU()",
		"HIP set device error for device " << device << ".",
		""
	);

    hipStream_t stream = NULL;
    TBTKAssert(
        hipStreamCreateWithFlags(
            &stream, 
            hipStreamNonBlocking
        ) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "Failed to set up stream on device.",
        ""
    )
    
    //Create handle for hip Solver and set stream
    hipsolverHandle_t hipsolverHandle = NULL;
    TBTKAssert(
        hipsolverCreate(
            &hipsolverHandle
        ) == HIPSOLVER_STATUS_SUCCESS,
        "Diagonalizer::solveGPU()",
        "HIP error in hipsolverDnCreate().",
        ""
    )
    TBTKAssert(
        hipsolverSetStream(
            hipsolverHandle,
             stream
            ) == HIPSOLVER_STATUS_SUCCESS,
        "Diagonalizer::solveGPU()",
        "HIP error setting up stream for hipsolver.",
        ""
    ) 

    //Allocate memory on device for hamiltonian and corresponding output
    int n = getModel().getBasisSize();	//...nxn-matrix. //TODO calculate n from matrix, not model
    hipDoubleComplex *matrix_device;
    double *eigenValues_device;
    int *info_device = nullptr;

    TBTKAssert(
        hipMalloc(
            reinterpret_cast<void **>(&matrix_device), 
            sizeof(hipDoubleComplex) * matrix.getSize()
        ) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "HIP error allocating unified memory.",
        ""
    ) 
    TBTKAssert(
        hipMalloc(
            reinterpret_cast<void **>(&eigenValues_device),
            sizeof(double) * eigenValues.getSize()
        ) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "HIP error allocating memory on device.",
        ""
    ) 

    TBTKAssert(
        hipMalloc(
            reinterpret_cast<void **>(&info_device),
             sizeof(int)
        ) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "HIP error allocating memory on device.",
        ""
    )
    //Prefetching memory on the device (it is allowed to fail, if
    // memory oversubscription is needed)
    // hipMemPrefetchAsync(
    //     &eigenValues_device, 
    //     sizeof(double) * eigenValues.getSize(), 
    //     device, 
    //     stream
    // );
    // hipMemPrefetchAsync(
    //     &matrix_device, 
    //     sizeof(hipDoubleComplex) * matrix.getSize(), 
    //     device, 
    //     stream
    // );

    //Copy matrix to device
    TBTKAssert(
        hipMemcpyAsync(
            matrix_device, 
            matrix.getData(), 
            sizeof(hipDoubleComplex) * matrix.getSize(), 
            hipMemcpyHostToDevice,
            stream) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "HIP error copying to memory on device.",
        ""
    )


    //Set up the hipsolver routine
    hipsolverEigMode_t jobz = HIPSOLVER_EIG_MODE_VECTOR;		//...eigenvalues and eigenvectors...
    hipsolverFillMode_t uplo = HIPSOLVER_FILL_MODE_UPPER;		//...for an upper triangular Matrix...

    hipDoubleComplex *buffer_device = nullptr; // Device buffer memory
    int sizeBuffer_device = 0; //Size of buffer memory needed on device

    int info;

    hipsolverStatus_t status = hipsolverZheevd_bufferSize(
        hipsolverHandle, 
        jobz, 
        uplo, 
        n,
        matrix_device,
        n,
        eigenValues_device, 
        &sizeBuffer_device
    );
    cout << "hipsolverZheevd_bufferSize returns:" << endl;
    cout << status << endl;

    TBTKAssert(
        status == HIPSOLVER_STATUS_SUCCESS,
        "Diagonalizer::solveGPU()",
        "HIP error in hipsolverZheevd_bufferSize.",
        ""
    )

    
    //Check if buffer is needed and allocate accordingly
    // TBTKAssert(
    //     hipsolverZheevd_bufferSize(
    //         hipsolverHandle, 
    //         jobz, 
    //         uplo, 
    //         n,
    //         matrix_device,
    //         n,
    //         eigenValues_device, 
    //         &sizeBuffer_device
    //     ) == HIPSOLVER_STATUS_SUCCESS,
    //     "Diagonalizer::solveGPU()",
    //     "HIP error in hipsolverZheevd_bufferSize.",
    //     ""
    // )

    // HIP managed memory is used, instead of device memory, as this allocation
    // can become substancial for bigger hamiltonians
    TBTKAssert(
        hipMalloc(reinterpret_cast<void **>(&buffer_device),
            sizeof(hipDoubleComplex) * sizeBuffer_device
        ) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "Failed to allocate buffer memory on device.",
        "" 
    )
    hipMemPrefetchAsync(
        &buffer_device, 
        sizeof(hipDoubleComplex) * sizeBuffer_device, 
        device, 
        stream
    );


    //Run the diagonalization routine
    TBTKAssert(
        hipsolverZheevd(
        hipsolverHandle, 
        jobz, 
        uplo,
        n, 
        matrix_device,
        n,
        eigenValues_device,
        buffer_device,
        sizeBuffer_device,
        info_device
        ) == HIPSOLVER_STATUS_SUCCESS,
        "Diagonalizer::solveGPU()",
        "HIP error in hipsolverZheevd.",
        ""
    )

    TBTKAssert(
        hipMemcpyAsync(
            &info,
            info_device,
            sizeof(int),
            hipMemcpyDeviceToHost,
            stream
        ) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "HIP error copying to memory from device.",
        ""
    )
    TBTKAssert(
        info == 0,
        "Diagonalizer:solve()",
        "Diagonalization routine hipsolverZheevd exited with INFO=" + to_string(info) + ".",
        "See HIP documentation for hipsolverZheevd for further information."
    );

    TBTKAssert(
        hipMemcpyAsync(
            matrix.getData(),
            matrix_device,
            sizeof(hipDoubleComplex)*matrix.getSize(),
            hipMemcpyDeviceToHost,
            stream
        ) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "HIP error copying to memory from device.",
        ""
    )

    TBTKAssert(
        hipMemcpyAsync(
            eigenValues.getData(),
            eigenValues_device,
            sizeof(double)*eigenValues.getSize(),
            hipMemcpyDeviceToHost,
            stream
        ) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "HIP error copying to memory from device.",
        ""
    )

    TBTKAssert(
        hipStreamSynchronize(
            stream
        ) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "HIP error while synchronizing device stream.",
        ""
    )

    // Free device resources
    TBTKAssert(
        hipFree(
            matrix_device
        ) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "HIP error freeing device memory.",
        ""
    )
    TBTKAssert(
        hipFree(
            eigenValues_device
        ) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "HIP error freeing device memory.",
        ""
    )
    TBTKAssert(
        hipFree(
            info_device
        ) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "HIP error freeing device memory.",
        ""
    )
    TBTKAssert(
        hipFree(
            buffer_device
        ) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "HIP error freeing device memory.",
        ""
    )
    TBTKAssert(
       hipsolverDestroy(
           hipsolverHandle
        ) == HIPSOLVER_STATUS_SUCCESS,
        "Diagonalizer::solveGPU()",
        "HIP error destroying hipsolver handle.",
        ""
    )
    TBTKAssert(
        hipStreamDestroy(
            stream
        ) == hipSuccess,
        "Diagonalizer::solveGPU()",
        "HIP error destroying hip stream.",
        ""
    )
	GPUResourceManager::getInstance().freeDevice(device);
}

void Diagonalizer::setupBasisTransformationGPU(){
	//Get the OverlapAmplitudeSet.
	const OverlapAmplitudeSet &overlapAmplitudeSet
		= getModel().getOverlapAmplitudeSet();

	//Skip if the basis is assumed to be orthonormal.
	if(overlapAmplitudeSet.getAssumeOrthonormalBasis()){
		return;
    }

	//Fill the overlap matrix.
	int basisSize = getModel().getBasisSize();
	CArray<complex<double>> overlapMatrix(basisSize*basisSize);
	for(int n = 0; n < basisSize*basisSize; n++)
		overlapMatrix[n] = 0.;

	for(
		OverlapAmplitudeSet::ConstIterator iterator
			= overlapAmplitudeSet.cbegin();
		iterator != overlapAmplitudeSet.cend();
		++iterator
	){
		int row = getModel().getHoppingAmplitudeSet().getBasisIndex(
			(*iterator).getBraIndex()
		);
		int col = getModel().getHoppingAmplitudeSet().getBasisIndex(
			(*iterator).getKetIndex()
		);
		if(col >= row){
			overlapMatrix[row + col*basisSize]
				+= (*iterator).getAmplitude();
		}
	}

	//Diagonalize the overlap matrix.
	CArray<double> overlapMatrixEigenValues(basisSize);
    solveGPU( overlapMatrix, 
                overlapMatrixEigenValues);

	//Setup basisTransformation storage.
	basisTransformation = CArray<complex<double>>(basisSize*basisSize);

	//Calculate the basis transformation using canonical orthogonalization.
	//See for example section 3.4.5 in Moder Quantum Chemistry, Attila
	//Szabo and Neil S. Ostlund.
	for(int row = 0; row < basisSize; row++){
		for(int col = 0; col < basisSize; col++){
			basisTransformation[row + basisSize*col]
				= overlapMatrix[
					row + basisSize*col
				]/sqrt(
					overlapMatrixEigenValues[col]
				);
		}
	}
}

void Diagonalizer::transformToOrthonormalBasisGPU(){
	//Skip if no basis transformation has been set up (the original basis
	//is assumed to be orthonormal).
	if(basisTransformation.getData() == nullptr)
		return;

	int basisSize = getModel().getBasisSize();

	//Perform the transformation H' = U^{\dagger}HU, where U is the
	//transform to the orthonormal basis.
	Matrix<complex<double>> h(basisSize, basisSize);
	Matrix<complex<double>> U(basisSize, basisSize);
	Matrix<complex<double>> Udagger(basisSize, basisSize);
	for(int row = 0; row < basisSize; row++){
		for(int col = 0; col < basisSize; col++){
			if(col >= row){
				h.at(row, col)
					= hamiltonian[row + col*basisSize];
			}
			else{
				h.at(row, col) = conj(
					hamiltonian[col + row*basisSize]
				);
			}

			U.at(row, col)
				= basisTransformation[row + basisSize*col];

			Udagger.at(row, col) = conj(
				basisTransformation[col + basisSize*row]
			);
		}
	}

	Matrix<complex<double>> hp = Udagger*h*U;

	for(int row = 0; row < basisSize; row++){
		for(int col = 0; col < basisSize; col++){
			if(col >= row){
				hamiltonian[row + col*basisSize]
					= hp.at(row, col);
			}
		}
	}
}

};	//End of namespace Solver
};	//End of namespace TBTK
