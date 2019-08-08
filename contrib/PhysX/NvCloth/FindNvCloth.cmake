# - Try to find NvCloth
# Once done this will define
#  NVCLOTH_FOUND - System has NvCloth
#  NVCLOTH_ROOT_DIR - The root of NvCloth

# NOTE: We're including a version in this, but the first hint is without one - we should use that!
FIND_PATH(		NVCLOTH_ROOT_DIR include/NvCloth/Fabric.h
				HINTS 
				${GW_DEPS_ROOT}/NvCloth
				${GW_DEPS_ROOT}/sw/devrel/libdev/NvCloth/trunk
				)
				

				
				
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NvCloth DEFAULT_MSG NVCLOTH_ROOT_DIR)

mark_as_advanced(NVCLOTH_ROOT_DIR)
