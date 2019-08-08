# - Try to find PxShared
# Once done this will define
#  PXSHARED_FOUND - System has PxShared
#  PXSHARED_INCLUDE_DIRS - The PxShared include directories

# NOTE: We're including a version in this, but the first hint is without one - we should use that!
FIND_PATH(		PXSHARED_INCLUDE_DIRS include/cudamanager/PxGpuCopyDesc.h
				HINTS 
				${GW_DEPS_ROOT}/PxShared
				${GW_DEPS_ROOT}/sw/physx/PxShared/1.0/trunk/
				)
				
MESSAGE(${PXSHARED_INCLUDE_DIRS})
				
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PxShared DEFAULT_MSG PXSHARED_INCLUDE_DIRS)

mark_as_advanced(PXSHARED_INCLUDE_DIRS)
