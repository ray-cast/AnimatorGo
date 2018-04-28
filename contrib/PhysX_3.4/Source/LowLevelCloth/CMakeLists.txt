cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# -----------------------------------------------------------------------------------------------------------------------------
# FLAGS
# -----------------------------------------------------------------------------------------------------------------------------

if(APPLE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DPX_PHYSX_STATIC_LIB -DNDEBUG -DPX_SUPPORT_PVD=0 -pipe -mmacosx-version-min=10.7 -msse2 -ffast-math -fno-exceptions -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden -Werror -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -O3 -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DPX_PHYSX_STATIC_LIB -D_DEBUG -DPX_DEBUG=1 -DPX_CHECKED=1 -DPX_SUPPORT_PVD=1 -pipe -mmacosx-version-min=10.7 -msse2 -ffast-math -fno-exceptions -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden -Werror -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -g3 -gdwarf-2 -O0")
elseif(WIN32)
    # TODO: Set PX_PHYSX_CORE_EXPORTS flag and enable delay loads.
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /D \"DISABLE_CUDA_PHYSX\" /Zc:wchar_t /D \"PX_PHYSX_STATIC_LIB\" /D \"WIN32\" /D \"WIN64\" /D \"_CRT_SECURE_NO_DEPRECATE\" /D \"_CRT_NONSTDC_NO_DEPRECATE\" /D \"_WINSOCK_DEPRECATED_NO_WARNINGS\" /D \"PX_NVTX=1\" /D \"PX_SUPPORT_PVD=1\" /D \"_WINDLL\" /GF /Zc:forScope /GR- /Gd /MT /MP /Wall /wd4514 /wd4820 /wd4127 /wd4710 /wd4711 /wd4435 /wd4577 /wd4464 /d2Zi+ /Zc:inline /fp:fast")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D \"DISABLE_CUDA_PHYSX\" /Zc:wchar_t /D \"PX_PHYSX_STATIC_LIB\" /D \"WIN32\" /D \"WIN64\" /D \"_CRT_SECURE_NO_DEPRECATE\" /D \"_CRT_NONSTDC_NO_DEPRECATE\" /D \"_WINSOCK_DEPRECATED_NO_WARNINGS\" /D \"_DEBUG\" /D \"PX_DEBUG=1\" /D \"PX_CHECKED=1\" /D \"PX_NVTX=1\" /D \"PX_SUPPORT_PVD=1\" /D \"PX_PHYSX_DLL_NAME_POSTFIX=DEBUG\" /D \"_WINDLL\" /GF /Zc:forScope /RTCu /GR- /Gd /MTd /Zc:inline /fp:fast /MP /Wall /wd4514 /wd4820 /wd4127 /wd4710 /wd4711 /wd4435 /wd4577 /wd4464 /d2Zi+")
elseif(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DPX_PHYSX_STATIC_LIB -DNDEBUG -DPX_SUPPORT_PVD=0 -pipe -msse2 -ffast-math -fno-exceptions -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden -Wno-return-type-c-linkage -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -O3 -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DPX_PHYSX_STATIC_LIB -D_DEBUG -DPX_DEBUG=1 -DPX_CHECKED=1 -DPX_SUPPORT_PVD=1 -pipe -msse2 -ffast-math -fno-exceptions -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden -Wno-return-type-c-linkage -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -g3 -O0")
endif()

# -----------------------------------------------------------------------------------------------------------------------------
# FILES
# -----------------------------------------------------------------------------------------------------------------------------

# Headers
file(GLOB_RECURSE LOWLEVEL_CLOTH_HEADERS ${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Include/*.h 
                                         ${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/include/*.h 
                                         ${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/*.h)

# Sources
file(GLOB_RECURSE LOWLEVEL_CLOTH_SOURCES ${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/*.cpp)

# Remove AVX sources 
# TODO: Check for AVX support
if(NOT WIN32)
    list(REMOVE_ITEM LOWLEVEL_CLOTH_SOURCES "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/avx/SwSolveConstraints.cpp")
endif()
# Remove NEON sources 
# TODO: Check for NEON support
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/neon/SwCollisionHelpers.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_SOURCES "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/neon/NeonCollision.cpp")
list(REMOVE_ITEM LOWLEVEL_CLOTH_SOURCES "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/neon/NeonSelfCollision.cpp")
list(REMOVE_ITEM LOWLEVEL_CLOTH_SOURCES "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/neon/NeonSolverKernel.cpp")
# Remove Scalar sources
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/scalar/SwCollisionHelpers.h")
# Remove SSE2 source
# TODO: Check for SSE2 support
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/sse2/SwCollisionHelpers.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/sse2/SwSolveConstraints.h")

# Remove windows-specifc headers
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/ClothClone.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuCheckSuccess.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuCloth.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuClothCreate.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuClothData.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuCollision.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuContextLock.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuDeviceVector.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuFabric.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuFactory.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuPhaseConfig.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuPinnedAllocator.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuSelfCollision.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuSolver.h")
list(REMOVE_ITEM LOWLEVEL_CLOTH_HEADERS "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuSolverKernel.h")
# Remove windows-specific sources
list(REMOVE_ITEM LOWLEVEL_CLOTH_SOURCES "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuCloth.cpp")
list(REMOVE_ITEM LOWLEVEL_CLOTH_SOURCES "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuClothClone.cpp")
list(REMOVE_ITEM LOWLEVEL_CLOTH_SOURCES "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuClothCreate.cpp")
list(REMOVE_ITEM LOWLEVEL_CLOTH_SOURCES "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuClothData.cpp")
list(REMOVE_ITEM LOWLEVEL_CLOTH_SOURCES "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuContextLock.cpp")
list(REMOVE_ITEM LOWLEVEL_CLOTH_SOURCES "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuFabric.cpp")
list(REMOVE_ITEM LOWLEVEL_CLOTH_SOURCES "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuFactory.cpp")
list(REMOVE_ITEM LOWLEVEL_CLOTH_SOURCES "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows/CuSolver.cpp")

# if(WIN32 OR APPLE)
#     list(REMOVE_ITEM PHYSX_SOURCES "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysX/src/device/linux/PhysXIndicatorLinux.cpp")
# endif()

# Source groups
source_group("Headers" FILES ${LOWLEVEL_CLOTH_HEADERS})
source_group("Sources" FILES ${LOWLEVEL_CLOTH_SOURCES})

# -----------------------------------------------------------------------------------------------------------------------------
# TARGETS
# -----------------------------------------------------------------------------------------------------------------------------

add_library(LowLevelCloth ${LOWLEVEL_CLOTH_HEADERS} ${LOWLEVEL_CLOTH_SOURCES})

set_target_properties(LowLevelCloth PROPERTIES FOLDER "PhysX")

target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/Common/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PxShared/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PxShared/src/foundation/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PxShared/src/fastxml/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PxShared/src/pvd/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PxShared/src/NvSimd/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Include/characterkinematic")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Include/common")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Include/extensions")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Include/geometry")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Include/GeomUtils")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Include/pvd")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Include/particles")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Include/cloth")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Include/gpu")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Include/vehicle")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/Common/src")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/GeomUtils/headers")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/GeomUtils/src")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/GeomUtils/src/contact")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/GeomUtils/src/common")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/GeomUtils/src/convex")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/GeomUtils/src/distance")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/GeomUtils/src/sweep")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/GeomUtils/src/gjk")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/GeomUtils/src/mesh")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/GeomUtils/src/hf")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/GeomUtils/src/pcm")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/GeomUtils/src/ccd")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/GeomUtils/src/intersection")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevel/API/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevel/software/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevel/common/include/pipeline")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevel/common/include/utils")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevel/common/include/collision")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelAABB/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelDynamics/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelDynamics/src")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelParticles/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysX/src")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysX/src/buffering")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysX/src/particles")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysX/src/cloth")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/SimulationController/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/SimulationController/src")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/SimulationController/src/particles")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/SimulationController/src/cloth")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysXCooking/src")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysXCooking/src/mesh")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysXCooking/src/convex")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/SceneQuery/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysXGpu/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysXMetaData/core/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysXVehicle/src")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysXVehicle/src/PhysXMetaData/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysXMetaData/extensions/include")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysXExtensions/src/serialization/Xml")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysXExtensions/src/serialization/File")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysXExtensions/src/serialization/Binary")
target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/PhysXExtensions/src")

if(WIN32)
    target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src/windows")
    target_include_directories(LowLevelCloth PUBLIC "${PROJECT_SOURCE_DIR}/src/PhysX_3.4/Source/LowLevelCloth/src")
endif()