CMAKE_MINIMUM_REQUIRED(VERSION 3.21)

set(TBTK_CXX_STANDARD 17)
set(COMPILE_HIP TRUE)





#------------------- Gather information about TBTK version -----------------
EXECUTE_PROCESS(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE TBTK_VERSION_GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
SET(TBTK_VERSION_GIT_HASH ${TBTK_VERSION_GIT_HASH})
ADD_DEFINITIONS(-DTBTK_VERSION_GIT_HASH=\"${TBTK_VERSION_GIT_HASH}\")
#Avoid printing setting and printing information multiple times.
IF(NOT TBTK_VERSION_DEFINED)
        SET(TBTK_VERSION_MAJOR 2)
        SET(TBTK_VERSION_MINOR 6)
        SET(TBTK_VERSION_PATCH 4)
        SET(
                TBTK_VERSION
                ${TBTK_VERSION_MAJOR}.${TBTK_VERSION_MINOR}.${TBTK_VERSION_PATCH}
        )

        ADD_DEFINITIONS(
                -DTBTK_VERSION_MAJOR=${TBTK_VERSION_MAJOR}
                -DTBTK_VERSION_MINOR=${TBTK_VERSION_MINOR}
                -DTBTK_VERSION_PATCH=${TBTK_VERSION_PATCH}
        )

        # INCLUDE(TBTKGitInfo)

        MESSAGE("================================== ABOUT TBTK ==================================")
        MESSAGE("Version:\t${TBTK_VERSION}")
        MESSAGE("Git hash:\t${TBTK_VERSION_GIT_HASH}")
        MESSAGE("================================================================================")
        Message("")

        SET(TBTK_VERSION_DEFINED TRUE)
ENDIF(NOT TBTK_VERSION_DEFINED)


#TODO test for minimum cmake version and uncomment accordingly
#if(${CMAKE_VERSION} VERSION_LESS 3.12)
#cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
#endif()

PROJECT(TBTK
        LANGUAGES CXX )

#---------------- Get packages ----------------------------------------
FIND_PACKAGE(OpenMP QUIET REQUIRED)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ADD_DEFINITIONS(-DTBTK_USE_OPEN_MP)

include(CheckLanguage)
# check_language(hip)

#-------------- Target json ----------------------------------
add_library(json INTERFACE)
target_include_directories(json INTERFACE json/)






#-------------- Target TBTK Core ----------------------------------
ADD_DEFINITIONS(-DTBTK_WRAP_PRIMITIVE_TYPES=1) #TODO ask Kristofer if this can be turned on in the headers
add_library(TBTKCore SHARED) #TODO change to interface
target_link_libraries(TBTKCore PUBLIC json)
# TODO Put TBTK exceptions into core
target_include_directories(TBTKCore PUBLIC 
                        Lib/include/Exceptions/
)
target_sources(TBTKCore 
                PRIVATE
                Lib/src/Exceptions/Exception.cpp
                Lib/src/Exceptions/ElementNotFoundException.cpp
                Lib/src/Exceptions/IndexException.cpp
)


target_include_directories(TBTKCore PUBLIC 
                        Lib/include/Core/
)
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
# TODO Put Atoms into core
target_include_directories(TBTKCore PUBLIC 
                        Lib/include/Elements/
)
target_sources(TBTKCore 
                PRIVATE
                Lib/src/Elements/Atom.cpp
)
#TODO including TBTK::Quantity into the core as well 
target_include_directories(TBTKCore PUBLIC Lib/include/)
target_sources(TBTKCore 
                PRIVATE
                Lib/src/Quantity/Base.cpp
                Lib/src/Quantity/Derived.cpp
                Lib/src/Quantity/Constants.cpp
)
#TODO including TBTK::Uncategorized into the core as well 
target_include_directories(TBTKCore PUBLIC Lib/include/Uncategorized/)
target_sources(TBTKCore 
                PRIVATE
                Lib/src/Uncategorized/BrillouinZone.cpp
                Lib/src/Uncategorized/ParametrizedLine.cpp
                Lib/src/Uncategorized/ParametrizedLine3d.cpp
                Lib/src/Uncategorized/ParallelepipedCell.cpp
                Lib/src/Uncategorized/SpacePartition.cpp
                Lib/src/Uncategorized/WignerSeitzCell.cpp
)
#TODO including StatesAndOperators into the core as well 
target_include_directories(TBTKCore PUBLIC Lib/include/StatesAndOperators/)
target_sources(TBTKCore 
                PRIVATE
                Lib/src/StatesAndOperators/AbstractState.cpp
                Lib/src/StatesAndOperators/AbstractOperator.cpp
                Lib/src/StatesAndOperators/ArrayState.cpp
                Lib/src/StatesAndOperators/BasicState.cpp
                Lib/src/StatesAndOperators/DefaultOperator.cpp
                # Lib/src/StatesAndOperators/GaussianBasisGenerator.cpp #TODO needs Eigen
                Lib/src/StatesAndOperators/GaussianState.cpp
                Lib/src/StatesAndOperators/ParallelepipedArrayState.cpp
                Lib/src/StatesAndOperators/StateSet.cpp
                Lib/src/StatesAndOperators/StateTreeNode.cpp
                Lib/src/StatesAndOperators/Sto3g.cpp
                Lib/src/StatesAndOperators/UnitCell.cpp
)
set_target_properties(TBTKCore PROPERTIES
                    CXX_STANDARD 17
                    CXX_STANDARD_REQUIRED ON)

#TODO including Builders into the core as well 
target_include_directories(TBTKCore PUBLIC Lib/include/Builders/)
target_sources(TBTKCore 
                PRIVATE
                Lib/src/Builders/ModelFactory.cpp
                Lib/src/Builders/RealLattice.cpp
                Lib/src/Builders/ReciprocalLattice.cpp
)
set_target_properties(TBTKCore PROPERTIES
                    CXX_STANDARD 17
                    CXX_STANDARD_REQUIRED ON)

#TODO including ManyBodyContext into the core as well 
target_include_directories(TBTKCore PUBLIC 
                Lib/include/ManyParticle/

)
target_sources(TBTKCore 
                PRIVATE
                Lib/src/ManyParticle/BitRegister.cpp
                Lib/src/ManyParticle/ExtensiveBitRegister.cpp
                Lib/src/ManyParticle/FockSpace.cpp
                Lib/src/ManyParticle/FockStateRuleSet.cpp
                Lib/src/ManyParticle/InteractionAmplitude.cpp
                Lib/src/ManyParticle/InteractionAmplitudeSet.cpp
                Lib/src/ManyParticle/ManyParticleContext.cpp
                Lib/src/FockStateMap/DefaultFockStateMap.cpp
                Lib/src/FockStateRule/DifferenceRule.cpp
                Lib/src/FockStateRule/FockStateRule.cpp
                Lib/src/FockStateRule/SumRule.cpp
                Lib/src/FockStateRule/WrapperRule.cpp
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
                Lib/src/Solver/Solver.cpp
                Lib/src/Solver/Diagonalizer.cpp
                Lib/src/Solver/ChebyshevExpander.cpp
)

if(COMPILE_HIP)
        target_sources(TBTKSolver PRIVATE
                Lib/src/hip/Diagonalizer.cpp
                Lib/src/hip/ChebyshevSolver.cpp
                Lib/src/hip/GPUResourceManager.cpp
        )
else()
        target_sources(TBTKSolver PRIVATE
                Lib/src/nocuda/Diagonalizer.cpp
                Lib/src/nocuda/ChebyshevSolver.cpp
                Lib/src/nocuda/GPUResourceManager.cpp
        ) 
endif()
#------------- Setup for hip files -------------



if(COMPILE_HIP)
        check_language(HIP)
        if(NOT CMAKE_HIP_COMPILER)
                 message(FATAL_ERROR "ERROR! Could not find HIP compiler!")
        endif()
        enable_language(HIP)
        find_package(HIP REQUIRED)
        find_package(HIPSOLVER)
        find_package(HIPSPARSE)
        set_source_files_properties(Lib/src/hip/Diagonalizer.cpp PROPERTIES LANGUAGE HIP) #TODO try all files in one
        set_source_files_properties(Lib/src/hip/ChebyshevSolver.cpp PROPERTIES LANGUAGE HIP)
        set_source_files_properties(Lib/src/hip/GPUResourceManager.cpp PROPERTIES LANGUAGE HIP)
        target_link_libraries(TBTKSolver PRIVATE 
                        hip::device
                        roc::hipsolver
                        roc::hipsparse
        )
        target_include_directories(TBTKSolver PRIVATE
                ${HIPSOLVER_INCLUDE_DIRS}
                ${HIP_INCLUDE_DIRS}
                ${HIPSPARSE_INCLUDE_DIRS}
        )
        cmake_policy(SET CMP0104 NEW)
        cmake_policy(SET CMP0105 NEW) #TODO Why?
        if(GPU_ARCH)
                set_target_properties(TBTKSolver PROPERTIES HIP_STANDARD ${TBTK_CXX_STANDARD}
                         HIP_STANDARD_REQUIRED ON)
                set(CMAKE_HIP_ARCHITECTURES "${GPU_ARCH}")
                set(HIP_ARCHITECTURES "${GPU_ARCH}")
                set_target_properties(TBTKSolver PROPERTIES HIP_ARCHITECTURES "${GPU_ARCH}") #TODO do that in one command
                set_target_properties(TBTKSolver PROPERTIES GPU_TARGETS "${GPU_ARCH}")
                message("Set GPU Arch")
        endif()
endif()

#---------------- Target Property extractors ----------------
add_library(TBTKProperty SHARED)
target_include_directories(TBTKProperty PUBLIC 
                Lib/include/Property/
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
)

#---------------- Target Property extractors ----------------
add_library(TBTKPropertyExtractor OBJECT)
target_include_directories(TBTKPropertyExtractor PUBLIC 
                Lib/include/TBTK/PropertyExtractor/
)
target_sources(TBTKPropertyExtractor PRIVATE
        Lib/src/PropertyExtractor/PropertyExtractor.cpp
        Lib/src/PropertyExtractor/IndexTreeGenerator.cpp
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

#----------------TODO in core: Target Utilities ----------------
# add_library(TBTKCore SHARED)
target_include_directories(TBTKCore PUBLIC 
                Lib/include/Utilities/
)
target_sources(TBTKCore PRIVATE
        Lib/src/Utilities/BandDiagramGenerator.cpp
        Lib/src/Utilities/Communicator.cpp
        Lib/src/Utilities/FileParser.cpp
        # Lib/src/Utilities/FileReader.cpp #TODO need Hdf5
        # Lib/src/Utilities/FileWriter.cpp
        Lib/src/Utilities/GPUResourceManager.cpp
        Lib/src/Utilities/HALinkedList.cpp
        # Lib/src/Utilities/IndexBasedHoppingAmplitudeFilter.cpp
        # Lib/src/Utilities/IndexBasedHoppingAmplitudeFilter.cpp
        # Lib/src/Utilities/MomentumSpaceContext.cpp
        # Lib/src/Utilities/NambuSpaceExtender.cpp
        # Lib/src/Utilities/PadeApproximator.cpp
        # Lib/src/Utilities/PadeApproximatorContinuousFractions.cpp
        Lib/src/Utilities/ParameterSet.cpp
        Lib/src/Utilities/Range.cpp
        Lib/src/Utilities/Serializable.cpp
        Lib/src/Utilities/SpinMatrix.cpp
        Lib/src/Utilities/Streams.cpp
        Lib/src/Utilities/Timer.cpp
        Lib/src/Utilities/UnitHandler.cpp
        Lib/src/Utilities/Vector2d.cpp
        Lib/src/Utilities/Vector3d.cpp
        Lib/src/Utilities/VectorNd.cpp
        # Lib/src/Utilities/WannierParser.cpp
)
# set_target_properties(TBTKUtilities PROPERTIES
#                     CXX_STANDARD 17
#                     CXX_STANDARD_REQUIRED ON
# )
# target_link_libraries(TBTKUtilities PUBLIC
#         TBTKCore
# )

#----------------------------- Testing -----------------------------------
enable_testing()
# if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
#         add_subdirectory(tests)
# endif()

add_executable(DiagonalizerTest 
        ${TBTK_SOURCE_DIR}/TestDiagonalizer.cpp
)
target_link_libraries(DiagonalizerTest PRIVATE
        json 
        TBTKCore
        TBTKSolver
        TBTKProperty
        TBTKPropertyExtractor
        # TBTKUtilities
)
set_target_properties(DiagonalizerTest PROPERTIES
                    CXX_STANDARD 17
                    CXX_STANDARD_REQUIRED ON)
        

add_test(
        NAME
        Diagonalizer
        COMMAND
        DiagonalizerTest
)
