# - Try to find GoogleTest
# Once done this will define
#  GOOGLETEST_FOUND - System has GoogleTest
#  GOOGLETEST_INCLUDE_DIR - The GoogleTest include directory
#  GOOGLETEST_SRC_DIR - The GoogleTest src directory

FIND_PATH(		GOOGLETEST_INCLUDE_DIR gtest/gtest.h
				HINTS 
				${GW_DEPS_ROOT}/GoogleTest
				${GW_DEPS_ROOT}/sw/physx/externals/GoogleTest/gtest-1.4.0
				PATH_SUFFIXES include)
				
FIND_PATH(		GOOGLETEST_SRC_DIR gtest.cpp
				HINTS 
				${GW_DEPS_ROOT}/GoogleTest
				${GW_DEPS_ROOT}/sw/physx/externals/GoogleTest/gtest-1.4.0
				PATH_SUFFIXES src)
				
INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(GoogleTest DEFAULT_MSG GOOGLETEST_INCLUDE_DIR GOOGLETEST_SRC_DIR)

mark_as_advanced(GOOGLETEST_INCLUDE_DIR GOOGLETEST_SRC_DIR)