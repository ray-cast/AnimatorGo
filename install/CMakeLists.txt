INCLUDE(ExternalProject)

# add_custom_target(third_install)

ExternalProject_Add(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG 2f631bb8087a0355d2b23a75a28d936ce237659d
    GIT_SHALLOW true
    GIT_PROGRESS true
    PREFIX ${OCTOON_PATH_INSTALL}/source/Catch2
    CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF
    INSTALL_COMMAND ""
)

# add_dependencies(third_install Catch2)

# ExternalProject_Add(
#     prorender
#     GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/RadeonProRender-Baikal.git
#     GIT_TAG 2d5a5d0eb2092d75adf637bf6f381d7e9307e986
#     GIT_SHALLOW TRUE
#     GIT_PROGRESS TRUE
#     PREFIX ${OCTOON_PATH_INSTALL}/source/RadeonProRender
#     CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF
#         -DBAIKAL_ENABLE_TESTS=OFF -DBAIKAL_ENABLE_STANDALONE=OFF -DBAIKAL_ENABLE_RPR=ON
#     INSTALL_COMMAND ""
# )

# add_dependencies(third_install prorender)

ExternalProject_Add(
    bullet
    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
    GIT_TAG 126b6762200b1a9ec8df39b8fd53cb2ad62fb146
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    PREFIX ${OCTOON_PATH_INSTALL}/source/bullet
    CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF
    INSTALL_COMMAND ""
)

# add_dependencies(third_install bullet)


IF(OCTOON_BUILD_PLATFORM_WINDOWS)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_WINCRT "True")
        set(PHYSX_BUILD_TYPE "debug")
    elseif(CMAKE_BUILD_TYPE MATCHES "Release")
        set(DEBUG_WINCRT "False")
        set(PHYSX_BUILD_TYPE "release")
    endif()

    if(OCTOON_BUILD_MUTILTHREAD_DLL)
        set(STATIC_WINCRT "False")
        set(PHYSX_STATIC_WINCRT FALSE)
    else()
        set(STATIC_WINCRT "True")
        set(PHYSX_STATIC_WINCRT TRUE)
    endif()

    ExternalProject_Add(
        physx
        URL https://github.com/NVIDIAGameWorks/PhysX/archive/4.0.0.zip
        URL_HASH MD5=79004323C3C39152A6C0491967E0A609
        PREFIX ${OCTOON_PATH_INSTALL}/source/PhysX
        CONFIGURE_COMMAND python ${OCTOON_PATH_INSTALL}/build_scripts/physx_option.py
            --path=${OCTOON_PATH_INSTALL}/source/PhysX/src/physx/physx/buildtools/presets/public/vc15win64.xml
            --build_snippets=False
            --build_public_samples=False
            --use_static_wincrt=${STATIC_WINCRT}
            --use_debug_wincrt=${DEBUG_WINCRT}
        COMMAND ${OCTOON_PATH_INSTALL}/build_scripts/physx_config.bat ${OCTOON_PATH_INSTALL}/source/PhysX/src/physx/physx vc15win64
        BUILD_COMMAND ${CMAKE_COMMAND} --build ${OCTOON_PATH_INSTALL}/source/PhysX/src/physx/physx/compiler/vc15win64 --config ${PHYSX_BUILD_TYPE}
        INSTALL_COMMAND ""
    )
ELSE()
	MESSAGE(FATAL_ERROR "Unsupported build platform for PhysX: " ${OCTOON_BUILD_PLATFORM})
ENDIF()

SET(PHYSX_DIR ${OCTOON_PATH_INSTALL}/source/PhysX/src/physx)
FIND_PACKAGE(PhysX)
# add_dependencies(third_install physx)

SET_TARGET_ATTRIBUTE(Catch2 "contrib")
# SET_TARGET_ATTRIBUTE(prorender "contrib")
SET_TARGET_ATTRIBUTE(bullet "contrib")
SET_TARGET_ATTRIBUTE(physx "contrib")