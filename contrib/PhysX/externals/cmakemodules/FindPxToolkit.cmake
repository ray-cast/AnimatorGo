# - Try to find PXTOOLKIT
# Once done this will define
#  PXTOOLKIT_FOUND - System has PXTOOLKIT
#  PXTOOLKIT_ROOT - The PXTOOLKIT root directory
#  PXTOOLKIT_INCLUDE_DIR - The PXTOOLKIT include directory
#  PXTOOLKIT_SRC_DIR - The PXTOOLKIT src directory

FIND_PATH(		PXTOOLKIT_INCLUDE_DIR PxTkBmpLoader.h
				HINTS 
				${GW_DEPS_ROOT}/PxToolKit
				${GW_DEPS_ROOT}/PhysX_3.4/Samples/PxToolkit				
				${GW_DEPS_ROOT}/sw/physx/PhysXSDK/3.4/trunk/Samples/PxToolkit
				PATH_SUFFIXES include)
				
FIND_PATH(		PXTOOLKIT_SRC_DIR PxTkBmpLoader.cpp
				HINTS 
				${GW_DEPS_ROOT}/PxToolKit
				${GW_DEPS_ROOT}/PhysX_3.4/Samples/PxToolkit
				${GW_DEPS_ROOT}/sw/physx/PhysXSDK/3.4/trunk/Samples/PxToolkit
				PATH_SUFFIXES src)
				
INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(PxToolKit DEFAULT_MSG PXTOOLKIT_INCLUDE_DIR PXTOOLKIT_SRC_DIR)

mark_as_advanced(PXTOOLKIT_INCLUDE_DIR PXTOOLKIT_SRC_DIR)
