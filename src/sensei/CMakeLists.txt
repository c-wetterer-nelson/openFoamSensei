#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0002 NEW)   # Policy CMP0002 required for for cmake >= 3

# Fail if not building out-of-source
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds disallowed. Use a separate build directory")
endif()

#-------------------------------------------------------------------------------
# Standard definitions when using OpenFOAM

if (NOT EXISTS "$ENV{WM_PROJECT_DIR}")
    message(FATAL_ERROR "WM_PROJECT_DIR not set - no OpenFOAM??")
endif()

# WM_OSTYPE env variable is optional (only POSIX anyhow)
if (EXISTS "$ENV{WM_OSTYPE}")
    set(WM_OSTYPE "$ENV{WM_OSTYPE}")
else()
    set(WM_OSTYPE "POSIX")
endif()

# Convenience name, as per wmake internals
set(LIB_SRC "$ENV{WM_PROJECT_DIR}/src")

# Needed for internal label-size, scalar-size, templates
add_definitions(
    -DWM_$ENV{WM_PRECISION_OPTION}
    -DWM_LABEL_SIZE=$ENV{WM_LABEL_SIZE}
    -DNoRepository
)

set(CMAKE_CXX_FLAGS_DEBUG
    "-g -O0 -std=c++11 -Wall -Wextra -Wno-unused-parameter -Wnon-virtual-dtor -Wno-overloaded-virtual"
)
set(CMAKE_CXX_FLAGS_RELEASE
    "-O3 -std=c++11 -Wall -Wextra -Wno-unused-parameter -Wnon-virtual-dtor -Wno-overloaded-virtual"
)

#-------------------------------------------------------------------------------
# All messages

message("================")
message("Using OpenFOAM = $ENV{WM_PROJECT_DIR}")
message("install prefix = ${CMAKE_INSTALL_PREFIX}")
message("Using Sensei   = $ENV{Sensei_DIR}")
message("================")

#-------------------------------------------------------------------------------
project(ofSensei)
set(CMAKE_BUILD_TYPE Release)

#-------------------------------------------------------------------------------

include_directories(
    ${LIB_SRC}/OpenFOAM/include
    ${LIB_SRC}/OpenFOAM/lnInclude
    ${LIB_SRC}/OSspecific/${WM_OSTYPE}/lnInclude
    ${LIB_SRC}/finiteArea/lnInclude
    ${LIB_SRC}/finiteVolume/lnInclude
    ${LIB_SRC}/fileFormats/lnInclude
    ${LIB_SRC}/conversion/lnInclude
    ${LIB_SRC}/meshTools/lnInclude
    ${LIB_SRC}/lagrangian/basic/lnInclude
    ${LIB_SRC}/overset/lnInclude
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

link_directories(
    $ENV{FOAM_LIBBIN}
    $ENV{FOAM_LIBBIN}/dummy
)


find_package(SENSEI REQUIRED)
find_package(MPI REQUIRED)

file(GLOB SOURCE_FILES
    senseiFunctionObject.C

    cloud/senseiCloud.C
    cloud/foamVtkCloudAdaptor.C

    areaMesh/senseiFaMesh.C
    areaMesh/foamVtkFaMeshAdaptor.C
    areaMesh/foamVtkFaMeshAdaptorGeom.C
    areaMesh/foamVtkFaMeshAdaptorFields.C

    volMesh/senseiFvMesh.C
    volMesh/foamVtkFvMeshAdaptor.C
    volMesh/foamVtkFvMeshAdaptorGeom.C
    volMesh/foamVtkFvMeshAdaptorFields.C
)

set(OPENFOAM_LIBRARIES
    OpenFOAM
    Pstream
    finiteArea
    finiteVolume
    fileFormats
    meshTools
    conversion
    lagrangian
    overset
)



add_library(
    senseiFoam
    SHARED
    ${SOURCE_FILES}
)

target_link_libraries(
    senseiFoam
    sensei
    MPI::MPI_CXX
    ${OPENFOAM_LIBRARIES}
)

install(TARGETS senseiFoam DESTINATION lib)

#-----------------------------------------------------------------------------


#-----------------------------------------------------------------------------
