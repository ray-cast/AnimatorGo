INCLUDE(ExternalProject)

add_custom_target(third_install)

ExternalProject_Add(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG 2f631bb8087a0355d2b23a75a28d936ce237659d
    GIT_SHALLOW true
    GIT_PROGRESS true
    PREFIX ${OCTOON_PATH_INSTALL}/source/Catch2
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:path=${OCTOON_PATH_INSTALL} -DBUILD_SHARED_LIBS=OFF
)

add_dependencies(third_install Catch2)

ExternalProject_Add(
    prorender
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/RadeonProRender-Baikal.git
    GIT_TAG 2d5a5d0eb2092d75adf637bf6f381d7e9307e986
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    PREFIX ${OCTOON_PATH_INSTALL}/source/RadeonProRender
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${OCTOON_PATH_INSTALL} -DBUILD_SHARED_LIBS=OFF
        -DBAIKAL_ENABLE_TESTS=OFF -DBAIKAL_ENABLE_STANDALONE=OFF -DBAIKAL_ENABLE_RPR=ON
)

add_dependencies(third_install prorender)

ExternalProject_Add(
    bullet
    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
    GIT_TAG 126b6762200b1a9ec8df39b8fd53cb2ad62fb146
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    PREFIX ${OCTOON_PATH_INSTALL}/source/bullet
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${OCTOON_PATH_INSTALL} -DBUILD_SHARED_LIBS=OFF
)

add_dependencies(third_install bullet)

IF(OCTOON_BUILD_PLATFORM_WINDOWS)
ExternalProject_Add(
    physx
    URL https://github.com/NVIDIAGameWorks/PhysX/archive/4.0.0.zip
    URL_HASH MD5=79004323C3C39152A6C0491967E0A609
    PREFIX ${OCTOON_PATH_INSTALL}/source/PhysX
    CONFIGURE_COMMAND ${OCTOON_PATH_INSTALL}/build_scripts/physx_config.bat ${OCTOON_PATH_INSTALL}/source/PhysX/src/physx/physx
    BUILD_COMMAND  cmake --build ${OCTOON_PATH_INSTALL}/source/PhysX/src/physx/physx/compiler/vc15win64
    INSTALL_COMMAND ""
)
ELSE()
	MESSAGE(FATAL_ERROR "Unsupported build platform for PhysX: " ${OCTOON_BUILD_PLATFORM})
ENDIF()

SET(PhysX_DIR ${OCTOON_PATH_INSTALL}/source/PhysX/src/physx/physx)
FIND_PACKAGE(PhysX)
add_dependencies(third_install physx)

SET_TARGET_ATTRIBUTE(Catch2 "contrib")
SET_TARGET_ATTRIBUTE(prorender "contrib")
SET_TARGET_ATTRIBUTE(bullet "contrib")
SET_TARGET_ATTRIBUTE(physx "contrib")