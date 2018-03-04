#Print the path to the config file (Useful for finding which path that is
#actually used when debugging)
MESSAGE("Using config file: ${CMAKE_CURRENT_LIST_DIR}/TBTKConfig.cmake")

#Include TBTKTargets.cmake
INCLUDE("${CMAKE_CURRENT_LIST_DIR}/TBTKTargets.cmake")

#Add custom Find-modules
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/Modules/")

#Add link directory for global install
LINK_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/..)
#Add link directory for build directory
LINK_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../Lib/src/)

#Detect packages
FIND_PACKAGE(BLAS REQUIRED)
FIND_PACKAGE(LAPACK REQUIRED)
FIND_PACKAGE(HDF5 QUIET)
FIND_PACKAGE(ARPACK QUIET)
FIND_PACKAGE(CUDA QUIET)
FIND_PACKAGE(CURL QUIET)
FIND_PACKAGE(FFTW3 QUIET)
FIND_PACKAGE(OpenCV QUIET)
FIND_PACKAGE(OpenBLAS QUIET)
FIND_PACKAGE(OpenMP QUIET)
FIND_PACKAGE(SuperLU QUIET)
FIND_PACKAGE(wxWidgets QUIET)

#Add libraries
#LIST(APPEND TBTK_LIBRARIES "-lTBTK")
LIST(APPEND TBTK_LIBRARIES "TBTK")

IF(ARPACK_FOUND)
	MESSAGE("[X] ARPACK")
#	LIST(APPEND TBTK_LIBRARIES " -larpack")
	LIST(APPEND TBTK_LIBRARIES "arpack")
ELSE(ARPACK_FOUND)
	MESSAGE("[ ] ARPACK")
ENDIF(ARPACK_FOUND)

IF(CUDA_FOUND)
	MESSAGE("[X] CUDA")
#	SET(TBTK_LIBRARIES "${TBTK_LIBRARIES} -lcudart -lcusparse")
	LIST(APPEND TBTK_LIBRARIES "cudart")
	LIST(APPEND TBTK_LIBRARIES "cusparse")
ELSE(CUDA_FOUND)
	MESSAGE("[ ] CUDA")
ENDIF(CUDA_FOUND)

IF(CURL_FOUND)
	MESSAGE("[X] cURL")
#	SET(TBTK_LIBRARIES "${TBTK_LIBRARIES} -lcurl")
	LIST(APPEND TBTK_LIBRARIES "curl")
ELSE(CURL_FOUND)
	MESSAGE("[ ] cURL")
ENDIF(CURL_FOUND)

IF(FFTW3_FOUND)
	MESSAGE("[X] FFTW3")
#	SET(TBTK_LIBRARIES "${TBTK_LIBRARIES} -lfftw3")
	LIST(APPEND TBTK_LIBRARIES "fftw3")
ELSE(FFTW3_FOUND)
	MESSAGE("[ ] FFTW3")
ENDIF(FFTW3_FOUND)

IF(HDF5_FOUND)
	MESSAGE("[X] HDF5")
	LIST(APPEND TBTK_LIBRARIES "hdf5")
	LIST(APPEND TBTK_LIBRARIES "hdf5_cpp")
ELSE(HDF5_FOUND)
	MESSAGE("[ ] HDF5")
ENDIF(HDF5_FOUND)

IF(OpenCV_FOUND)
	MESSAGE("[X] OpenCV")
#	SET(TBTK_LIBRARIES "${TBTK_LIBRARIES} ${OpenCV_LIBRARIES}")
	LIST(APPEND TBTK_LIBRARIES ${OpenCV_LIBRARIES})
ELSE(OpenCV_FOUND)
	MESSAGE("[ ] OpenCV")
ENDIF(OpenCV_FOUND)

IF(OpenCV_FOUND)
	MESSAGE("[X] OpenBLAS")
ELSE(OpenCV_FOUND)
	MESSAGE("[ ] OpenBLAS")
ENDIF(OpenCV_FOUND)

IF(OpenMP_FOUND OR OPENMP_FOUND)
	MESSAGE("[X] OpenMP")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ELSE(OpenMP_FOUND OR OPENMP_FOUND)
	MESSAGE("[ ] OpenMP")
ENDIF(OpenMP_FOUND OR OPENMP_FOUND)

IF(SuperLU_FOUND)
	MESSAGE("[X] SuperLU")
#	SET(TBTK_LIBRARIES "${TBTK_LIBRARIES} -lsuperlu")
	LIST(APPEND TBTK_LIBRARIES "superlu")
	INCLUDE_DIRECTORIES(${SUPER_LU_INCLUDES})
ELSE(SuperLU_FOUND)
	MESSAGE("[ ] SuperLU")
ENDIF(SuperLU_FOUND)

IF(wxWidgets_FOUND)
	MESSAGE("[X] wxWidgets")
	INCLUDE(${wxWidgets_USE_FILE})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")
#	SET(TBTK_LIBRARIES "${TBTK_LIBRARIES} ${wxWidgets_LIBRARIES}")
	LIST(APPEND TBTK_LIBRARIES ${wxWidgets_LIBRARIES})
ELSE(wxWidgets_FOUND)
	MESSAGE("[ ] wxWidgets")
ENDIF(wxWidgets_FOUND)

#LIST(APPEND TBTK_LIBRARIES "-lhdf5 -lhdf5_cpp -llapack -lblas")
LIST(APPEND TBTK_LIBRARIES "lapack")
IF(OpenBLAS_FOUND)
	LIST(APPEND TBTK_LIBRARIES "openblas")
ELSE(OpenBLAS_FOUND)
	LIST(APPEND TBTK_LIBRARIES "blas")
ENDIF()
