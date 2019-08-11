# - Try to find nvToolsExt
# Once done this will define
#  NVTOOLSEXT_FOUND - System has nvToolsExt
#  NVTOOLSEXT_INCLUDE_DIRS - The nvToolsExt include directories
#  NVTOOLSEXT_LIBRARIES - The libraries needed to use nvToolsExt
#  NVTOOLSEXT_DEFINITIONS - Compiler switches required for using nvToolsExt

FIND_PATH(		NVTOOLSEXT_INCLUDE_DIRS nvToolsExt.h
				HINTS 
				${GW_DEPS_ROOT}/nvToolsExt
				${GW_DEPS_ROOT}/Externals/nvToolsExt/1
				${GW_DEPS_ROOT}/sw/physx/externals/nvToolsExt/1
				PATH_SUFFIXES include)

INCLUDE(FindPackageHandleStandardArgs)
				
IF(TARGET_BUILD_PLATFORM STREQUAL "Windows")
	# NOTE: Doesn't make sense for all platforms - ARM
	IF(CMAKE_CL_64)
		SET(NVTOOLSEXT_LIBNAME "nvToolsExt64_1")
		SET(NVTOOLSEXT_LIBPATH_SUFFIX "x64")
	ELSE(CMAKE_CL_64)
		SET(NVTOOLSEXT_LIBNAME nvToolsExt32_1)
		SET(NVTOOLSEXT_LIBPATH_SUFFIX "Win32")
	ENDIF(CMAKE_CL_64)				
					
					
	FIND_LIBRARY(	NVTOOLSEXT_LIBRARIES ${NVTOOLSEXT_LIBNAME}
					${GW_DEPS_ROOT}/nvToolsExt
					${GW_DEPS_ROOT}/Externals/nvToolsExt/1/lib/${NVTOOLSEXT_LIBPATH_SUFFIX}
					${GW_DEPS_ROOT}/sw/physx/externals/nvToolsExt/1/lib/${NVTOOLSEXT_LIBPATH_SUFFIX}
					)
	
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(nvToolsExt DEFAULT_MSG NVTOOLSEXT_LIBRARIES NVTOOLSEXT_INCLUDE_DIRS)
ELSE()
	# Exclude the libraries for non-windows platforms
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(nvToolsExt DEFAULT_MSG NVTOOLSEXT_INCLUDE_DIRS)
ENDIF()

mark_as_advanced(NVTOOLSEXT_INCLUDE_DIRS NVTOOLSEXT_LIBRARIES)