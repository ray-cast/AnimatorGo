#
# Build PxCudaContextManager
#

FIND_PACKAGE(CUDA REQUIRED)

SET(PXSHARED_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../../../src)

SET(LL_SOURCE_DIR ${PXSHARED_SOURCE_DIR}/cudamanager)

SET(CUDA_COMPILER_OPTION_DEBUG "--compiler-options=-Wall,-O3,-fPIC,-msse2,-mfpmath=sse,-malign-double,-m64,-fvisibility=hidden")
SET(CUDA_COMPILER_OPTION_CHECKED "--compiler-options=-Wall,-O3,-fPIC,-msse2,-mfpmath=sse,-malign-double,-m64,-fvisibility=hidden")
SET(CUDA_COMPILER_OPTION_PROFILE "--compiler-options=-Wall,-O3,-fPIC,-msse2,-mfpmath=sse,-malign-double,-m64,-fvisibility=hidden")
SET(CUDA_COMPILER_OPTION_RELEASE "--compiler-options=-Wall,-O3,-fPIC,-msse2,-mfpmath=sse,-malign-double,-m64,-fvisibility=hidden")

# include PxCudaContextManager common
INCLUDE(../common/PxCudaContextManager.cmake)

# Use generator expressions to set config specific preprocessor definitions
TARGET_COMPILE_DEFINITIONS(PxCudaContextManager 

	# Common to all configurations
	PRIVATE ${PXSHARED_LINUX_COMPILE_DEFS};

	PRIVATE $<$<CONFIG:debug>:${PXSHARED_LINUX_DEBUG_COMPILE_DEFS};>
	PRIVATE $<$<CONFIG:checked>:${PXSHARED_LINUX_CHECKED_COMPILE_DEFS};>
	PRIVATE $<$<CONFIG:profile>:${PXSHARED_LINUX_PROFILE_COMPILE_DEFS};>
	PRIVATE $<$<CONFIG:release>:${PXSHARED_LINUX_RELEASE_COMPILE_DEFS};>
)
