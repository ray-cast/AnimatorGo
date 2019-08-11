#
# Build NvCloth
#

SET(GW_DEPS_ROOT $ENV{GW_DEPS_ROOT})

SET(NVCLOTH_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../../)

SET(NVCLOTH_PLATFORM_SOURCE_FILES
	${PROJECT_ROOT_DIR}/src/ps/unix/PsUnixAtomic.cpp
	${PROJECT_ROOT_DIR}/src/ps/unix/PsUnixFPU.h
	${PROJECT_ROOT_DIR}/src/ps/android/cpu-features.c
	${PROJECT_ROOT_DIR}/src/ps/android/cpu-features.h

	${PROJECT_ROOT_DIR}/src/neon/NeonCollision.cpp
	${PROJECT_ROOT_DIR}/src/neon/NeonSelfCollision.cpp
	${PROJECT_ROOT_DIR}/src/neon/NeonSolverKernel.cpp
	${PROJECT_ROOT_DIR}/src/neon/SwCollisionHelpers.h
)

SET(NVCLOTH_PLATFORM_INCLUDES
	${LL_SOURCE_DIR}/include/linux
	${ANDROID_NDK}/sources/android/cpufeatures
)

# Use generator expressions to set config specific preprocessor definitions
SET(NVCLOTH_COMPILE_DEFS 
	${NVCLOTH_ANDROID_COMPILE_DEFS};
	NvCloth_STATIC_LIB; # TODO: needed?
	
	NV_ANDROID
	NV_SIMD_SCALAR=1 # always compile scalar paths on android, so we may use them as fallback 
	NV_CLOTH_IMPORT=PX_DLL_EXPORT
	NV_CLOTH_ENABLE_DX11=0
	NV_CLOTH_ENABLE_CUDA=0

	$<$<CONFIG:debug>:${NVCLOTH_ANDROID_DEBUG_COMPILE_DEFS};>
	$<$<CONFIG:checked>:${NVCLOTH_ANDROID_CHECKED_COMPILE_DEFS};>
	$<$<CONFIG:profile>:${NVCLOTH_ANDROID_PROFILE_COMPILE_DEFS};>
	$<$<CONFIG:release>:${NVCLOTH_ANDROID_RELEASE_COMPILE_DEFS};>
)

# include common low level settings
INCLUDE(../common/NvCloth.cmake)
