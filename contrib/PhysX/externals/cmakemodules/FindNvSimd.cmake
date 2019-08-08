# - Try to find NvSimd
# Once done this will define
#  NVSIMD_FOUND - System has NvSimd
#  NVSIMD_INCLUDE_DIR - The NvSimd include directories

# TODO: Support version
FIND_PATH(		NVSIMD_INCLUDE_DIR include/NvSimd4f.h
				HINTS 
				${GW_DEPS_ROOT}/NvSimd
				${GW_DEPS_ROOT}/PxShared/NvSimd
				${GW_DEPS_ROOT}/PxShared/src/NvSimd
				${GW_DEPS_ROOT}/sw/devrel/GameWorks/Shared/NvSimd/${PACKAGE_FIND_VERSION}/trunk
				${GW_DEPS_ROOT}/sw/devrel/GameWorks/Shared/NvSimd/1.1/trunk
				)

MESSAGE(${NVSIMD_INCLUDE_DIR})
				
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NvSimd DEFAULT_MSG NVSIMD_INCLUDE_DIR)

mark_as_advanced(NVSIMD_INCLUDE_DIR)
