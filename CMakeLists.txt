CMAKE_MINIMUM_REQUIRED(VERSION 3.21)

set(TBTK_CXX_STANDARD 17)

#TODO test for minimum cmake version and uncomment accordingly
#if(${CMAKE_VERSION} VERSION_LESS 3.12)
#cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
#endif()

PROJECT(TBTK
        LANGUAGES CXX )

include(CheckLanguage)
# check_language(hip)

#-------------- Target json ----------------------------------
add_library(json INTERFACE)
target_include_directories(json INTERFACE json/)


#-------------- Target TBTK Core ----------------------------------
ADD_DEFINITIONS(-DTBTK_WRAP_PRIMITIVE_TYPES=1) #TODO ask Kristofer if this can be turned on in the headers
add_library(TBTKCore SHARED)
target_link_libraries(TBTKCore PUBLIC json)

target_include_directories(TBTKCore PUBLIC Lib/include/Core/)
target_sources(TBTKCore 
                PRIVATE
                Lib/src/Core/BasisStateSet.cpp
                Lib/src/Core/BlockStructureDescriptor.cpp
                Lib/src/Core/BoundaryCondition.cpp 
                Lib/src/Core/Geometry.cpp
                Lib/src/Core/HoppingAmplitude.cpp
                Lib/src/Core/HoppingAmplitudeTree.cpp
                Lib/src/Core/HoppingAmplitudeList.cpp
                Lib/src/Core/HoppingAmplitudeSet.cpp
                Lib/src/Core/Index.cpp
                Lib/src/Core/IndexTree.cpp
                Lib/src/Core/Model.cpp
                Lib/src/Core/OverlapAmplitude.cpp
                Lib/src/Core/OverlapAmplitudeSet.cpp
                Lib/src/Core/SingleParticleContext.cpp
                Lib/src/Core/SourceAmplitude.cpp
                Lib/src/Core/SourceAmplitudeSet.cpp
                Lib/src/Core/TBTK.cpp                          
)
set_target_properties(TBTKCore PROPERTIES
                    CXX_STANDARD 17
                    CXX_STANDARD_REQUIRED ON)

#-------------- Target TBTK Solvers ----------------------------------
# Only Diagonalizer and Chebyshev for now

add_library(TBTKSolver SHARED
        Lib/include/TBTK/Solver/Solver.h
        Lib/include/TBTK/Solver/Diagonalizer.h
        Lib/include/TBTK/Solver/ChebyshevExpander.h
)
set_target_properties(TBTKSolver PROPERTIES
                    CXX_STANDARD 17
                    CXX_STANDARD_REQUIRED ON)
target_link_libraries(TBTKSolver PUBLIC
        json 
        TBTKCore
)

target_include_directories(TBTKSolver PUBLIC 
        
)

target_sources(TBTKSolver PRIVATE
                Lib/src/Solver/Diagonalizer.cpp
                Lib/src/Solver/ChebyshevExpander.cpp
)

#------------- Setup for hip files -------------


# enable_language(HIP)
# include(CheckLanguage)
# find_package(HIP REQUIRED)

# target_include_directories(TBTKDiagonalizer PUBLIC 
#                 Lib/include/TBTK/Utilities/
# )
# target_sources(TBTKDiagonalizer PRIVATE
#                 Lib/src/hip/Diagonalizer.cpp
#                 Lib/src/hip/ChebyshevSolver.cpp
#                 Lib/src/hip/GPUResourceManager.cpp
#                 Lib/src/Utilities/GPUResourceManager.cpp
# )


#---------------- Target Property extractors ----------------
add_library(TBTKProperty SHARED)
target_include_directories(TBTKProperty PUBLIC 
                Lib/include/TBTK/Property/
)
target_sources(TBTKProperty PRIVATE
        Lib/src/Property/Density.cpp
        Lib/src/Property/DOS.cpp
        Lib/src/Property/EigenValues.cpp
        Lib/src/Property/GreensFunction.cpp
        Lib/src/Property/IndexDescriptor.cpp
        Lib/src/Property/InteractionVertex.cpp
        Lib/src/Property/LDOS.cpp
        Lib/src/Property/Magnetization.cpp
        Lib/src/Property/Property.cpp
        Lib/src/Property/SelfEnergy.cpp
        Lib/src/Property/SpectralFunction.cpp
        Lib/src/Property/SpinPolarizedLDOS.cpp
        Lib/src/Property/Susceptibility.cpp
        Lib/src/Property/TransmissionRate.cpp
        Lib/src/Property/WaveFunctions.cpp
)
set_target_properties(TBTKProperty PROPERTIES
                    CXX_STANDARD 17
                    CXX_STANDARD_REQUIRED ON)
target_link_libraries(TBTKProperty PUBLIC
        json 
        TBTKCore
        TBTKSolver
)

#---------------- Target Property extractors ----------------
add_library(TBTKPropertyExtractor SHARED)
target_include_directories(TBTKPropertyExtractor PUBLIC 
                Lib/include/TBTK/PropertyExtractor/
)
target_sources(TBTKPropertyExtractor PRIVATE
        Lib/src/PropertyExtractor/PropertyExtractor.cpp
        Lib/src/PropertyExtractor/Diagonalizer.cpp
        Lib/src/PropertyExtractor/ChebyshevExpander.cpp
)
set_target_properties(TBTKPropertyExtractor PROPERTIES
                    CXX_STANDARD 17
                    CXX_STANDARD_REQUIRED ON)
target_link_libraries(TBTKPropertyExtractor PUBLIC
        json 
        TBTKCore
        TBTKSolver
)


