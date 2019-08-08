# - Try to find Targa
# Once done this will define
#  TARGA_FOUND - System has targa
#  TARGA_INCLUDE_DIR - The targa include directory
#  TARGA_SRC_DIR - The targa src directory

FIND_PATH(		TARGA_INCLUDE_DIR targa.h
				HINTS 
				${GW_DEPS_ROOT}/externals/targa/1
				${GW_DEPS_ROOT}/sw/physx/externals/targa/1
				PATH_SUFFIXES include)
				
FIND_PATH(		TARGA_SRC_DIR targa.cpp
				HINTS 
				${GW_DEPS_ROOT}/externals/targa/1
				${GW_DEPS_ROOT}/sw/physx/externals/targa/1
				PATH_SUFFIXES src)
				
INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Targa DEFAULT_MSG TARGA_INCLUDE_DIR TARGA_SRC_DIR)

mark_as_advanced(TARGA_INCLUDE_DIR TARGA_SRC_DIR)