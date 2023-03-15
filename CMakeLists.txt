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






