#
# Build PxCudaContextManager
#
FIND_PACKAGE(CUDA REQUIRED)

SET(PXSHARED_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../../../src)

SET(LL_SOURCE_DIR ${PXSHARED_SOURCE_DIR}/cudamanager)

SET(CUDA_COMPILER_OPTION_DEBUG "--compiler-options=/W3,/nologo,/Ot,/Ox,/Zi,${WINCRT_DEBUG}")
SET(CUDA_COMPILER_OPTION_CHECKED "--compiler-options=/W3,/nologo,/Ot,/Ox,/Zi,${WINCRT_NDEBUG}")
SET(CUDA_COMPILER_OPTION_PROFILE "--compiler-options=/W3,/nologo,/Ot,/Ox,/Zi,${WINCRT_NDEBUG}")
SET(CUDA_COMPILER_OPTION_RELEASE "--compiler-options=/W3,/nologo,/Ot,/Ox,/Zi,${WINCRT_NDEBUG}")

# include PxCudaContextManager common
INCLUDE(../common/PxCudaContextManager.cmake)

# No linked libraries

# Use generator expressions to set config specific preprocessor definitions
TARGET_COMPILE_DEFINITIONS(PxCudaContextManager 

	# Common to all configurations
	PRIVATE ${PXSHARED_WINDOWS_COMPILE_DEFS};

	PRIVATE $<$<CONFIG:debug>:${PXSHARED_WINDOWS_DEBUG_COMPILE_DEFS};>
	PRIVATE $<$<CONFIG:checked>:${PXSHARED_WINDOWS_CHECKED_COMPILE_DEFS};>
	PRIVATE $<$<CONFIG:profile>:${PXSHARED_WINDOWS_PROFILE_COMPILE_DEFS};>
	PRIVATE $<$<CONFIG:release>:${PXSHARED_WINDOWS_RELEASE_COMPILE_DEFS};>
)

#TODO: Link flags
