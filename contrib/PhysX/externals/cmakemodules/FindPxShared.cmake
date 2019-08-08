# - Try to find PxShared
# Once done this will define
#  PXSHARED_FOUND - System has PxShared
#  PXSHARED_ROOT_DIR - The root of PxShared

# NOTE: We're including a version in this, but the first hint is without one - we should use that!
FIND_PATH(		PXSHARED_ROOT_DIR include/cudamanager/PxGpuCopyDesc.h
				HINTS 
				${GW_DEPS_ROOT}/PxShared
				${GW_DEPS_ROOT}/sw/physx/PxShared/1.0/trunk
				)
				

				
				
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PxShared DEFAULT_MSG PXSHARED_ROOT_DIR)

mark_as_advanced(PXSHARED_ROOT_DIR)
