# Define the options up front

OPTION(APPEND_CONFIG_NAME "Append config (DEBUG, CHECKED, PROFILE or '' for release) to outputted binaries." OFF)
OPTION(USE_GAMEWORKS_OUTPUT_DIRS "Use new GameWorks folder structure for binary output." ON)
OPTION(USE_STATIC_WINCRT "Use the statically linked windows CRT" OFF)
OPTION(USE_DEBUG_WINCRT "Use the debug version of the CRT" OFF)
OPTION(GENERATE_STATIC_LIBRARIES "Generate static libraries" OFF)

INCLUDE(SetOutputPaths)

	# Set the WINCRT_DEBUG and WINCRT_NDEBUG variables for use in project compile settings
	# Really only relevant to windows

	SET(DISABLE_ITERATOR_DEBUGGING "/D \"_HAS_ITERATOR_DEBUGGING=0\" /D \"_ITERATOR_DEBUG_LEVEL=0\"")
	SET(DISABLE_ITERATOR_DEBUGGING_CUDA "-D_HAS_ITERATOR_DEBUGGING=0 -D_ITERATOR_DEBUG_LEVEL=0")
	SET(CRT_DEBUG_FLAG "/D \"_DEBUG\"")
	SET(CRT_NDEBUG_FLAG "/D \"NDEBUG\"")

	# Need a different format for CUDA
   	SET(CUDA_DEBUG_FLAG "-DNDEBUG ${DISABLE_ITERATOR_DEBUGGING_CUDA}")
   	SET(CUDA_NDEBUG_FLAG "-DNDEBUG")
   	
	SET(CUDA_CRT_COMPILE_OPTIONS_NDEBUG "")
	SET(CUDA_CRT_COMPILE_OPTIONS_DEBUG "")


	IF(USE_STATIC_WINCRT)
	    SET(WINCRT_NDEBUG "/MT ${DISABLE_ITERATOR_DEBUGGING} ${CRT_NDEBUG_FLAG}" CACHE INTERNAL "Windows CRT build setting")
		SET(CUDA_CRT_COMPILE_OPTIONS_NDEBUG "/MT")

	    IF (USE_DEBUG_WINCRT)
		   	SET(CUDA_DEBUG_FLAG "-D_DEBUG")
	    	SET(WINCRT_DEBUG "/MTd ${CRT_DEBUG_FLAG}" CACHE INTERNAL "Windows CRT build setting")
			SET(CUDA_CRT_COMPILE_OPTIONS_DEBUG "/MTd")
	    ELSE()
	    	SET(WINCRT_DEBUG "/MT ${DISABLE_ITERATOR_DEBUGGING} ${CRT_NDEBUG_FLAG}" CACHE INTERNAL "Windows CRT build setting")
			SET(CUDA_CRT_COMPILE_OPTIONS_DEBUG "/MT")
	    ENDIF()
	ELSE()
	    SET(WINCRT_NDEBUG "/MD ${DISABLE_ITERATOR_DEBUGGING} ${CRT_NDEBUG_FLAG}")
		SET(CUDA_CRT_COMPILE_OPTIONS_NDEBUG "/MD")

	    IF(USE_DEBUG_WINCRT)
		   	SET(CUDA_DEBUG_FLAG "-D_DEBUG")
	    	SET(WINCRT_DEBUG "/MDd ${CRT_DEBUG_FLAG}" CACHE INTERNAL "Windows CRT build setting")
			SET(CUDA_CRT_COMPILE_OPTIONS_DEBUG "/MDd")
	    ELSE()
	    	SET(WINCRT_DEBUG "/MD ${DISABLE_ITERATOR_DEBUGGING} ${CRT_NDEBUG_FLAG}" CACHE INTERNAL "Windows CRT build setting")
			SET(CUDA_CRT_COMPILE_OPTIONS_DEBUG "/MD")
	    ENDIF()
	ENDIF()


IF(USE_GAMEWORKS_OUTPUT_DIRS)

	# New bitness suffix
	IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
		SET(LIBPATH_SUFFIX "64")
		# Set default exe suffix. Unset on platforms that don't need it. Include underscore since it's optional
		SET(EXE_SUFFIX "_64")
	ELSE()
		SET(LIBPATH_SUFFIX "32")
		# Set default exe suffix. Unset on platforms that don't need it. Include underscore since it's optional
		SET(EXE_SUFFIX "_32")
	ENDIF()				
	
	IF (NOT DEFINED PX_OUTPUT_LIB_DIR)
		MESSAGE(FATAL_ERROR "When using the GameWorks output structure you must specify PX_OUTPUT_LIB_DIR as the base")
	ENDIF()
	
	IF (NOT DEFINED PX_OUTPUT_BIN_DIR)
		MESSAGE(FATAL_ERROR "When using the GameWorks output structure you must specify PX_OUTPUT_BIN_DIR as the base")
	ENDIF()


	INCLUDE(GetCompilerAndPlatform)
	# Get the compiler and platform string into COMPILER_AND_PLATFORM
	GetCompilerAndPlatform(COMPILER_AND_PLATFORM)
	
	GetStaticCRTString(CRT_STRING)

	IF(NOT CRT_STRING) # Non windows returns empty string
		IF (NOT DEFINED PX_OUTPUT_ARCH)  # platforms with fixed arch like ps4 dont need to have arch defined
			# Set the path the DLLs and static libs go to
			SET(PX_ROOT_LIB_DIR "bin/${COMPILER_AND_PLATFORM}" CACHE INTERNAL "Relative root of the lib output directory")
			SET(PX_ROOT_EXE_DIR "bin/${COMPILER_AND_PLATFORM}" CACHE INTERNAL "Relative root dir of the exe output directory")

			# If there's no arch, then no bitness either.

			SET(EXE_SUFFIX "")
		ELSE()
			# Set the path the DLLs and static libs go to
			SET(PX_ROOT_LIB_DIR "bin/${COMPILER_AND_PLATFORM}/${PX_OUTPUT_ARCH}" CACHE INTERNAL "Relative root of the lib output directory")
			SET(PX_ROOT_EXE_DIR "bin/${COMPILER_AND_PLATFORM}/${PX_OUTPUT_ARCH}" CACHE INTERNAL "Relative root dir of the exe output directory")		
		ENDIF()
	ELSE()
		# Set the path the DLLs and static libs go to
		SET(PX_ROOT_LIB_DIR "bin/${COMPILER_AND_PLATFORM}/${PX_OUTPUT_ARCH}/${CRT_STRING}" CACHE INTERNAL "Relative root of the lib output directory")
		SET(PX_ROOT_EXE_DIR "bin/${COMPILER_AND_PLATFORM}/${PX_OUTPUT_ARCH}/${CRT_STRING}" CACHE INTERNAL "Relative root dir of the exe output directory")
	ENDIF()

	# Override our normal PX_ROOT_LIB_DIR for Android
	IF(TARGET_BUILD_PLATFORM STREQUAL "Android")
		SET(PX_ROOT_LIB_DIR "bin/${COMPILER_AND_PLATFORM}/${ANDROID_ABI}/${CM_ANDROID_FP}" CACHE INTERNAL "Relative root of the lib output directory")
	ENDIF()

	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   "${PX_OUTPUT_LIB_DIR}/${PX_ROOT_LIB_DIR}/debug"   )
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_PROFILE "${PX_OUTPUT_LIB_DIR}/${PX_ROOT_LIB_DIR}/profile" )
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_CHECKED "${PX_OUTPUT_LIB_DIR}/${PX_ROOT_LIB_DIR}/checked" )
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PX_OUTPUT_LIB_DIR}/${PX_ROOT_LIB_DIR}/release" )

	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${PX_OUTPUT_LIB_DIR}/${PX_ROOT_LIB_DIR}/debug"   )
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_PROFILE "${PX_OUTPUT_LIB_DIR}/${PX_ROOT_LIB_DIR}/profile" )
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_CHECKED "${PX_OUTPUT_LIB_DIR}/${PX_ROOT_LIB_DIR}/checked" )
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PX_OUTPUT_LIB_DIR}/${PX_ROOT_LIB_DIR}/release" )

	# Set the RUNTIME output directories - this is executables, etc.
	# Override our normal PX_ROOT_EXE_DIR for Android
	IF(TARGET_BUILD_PLATFORM STREQUAL "Android")
		SET(PX_ROOT_EXE_DIR "bin/${COMPILER_AND_PLATFORM}/${ANDROID_ABI}/${CM_ANDROID_FP}" CACHE INTERNAL "Relative root dir of the exe output directory")
	ENDIF()	

	# RFC 108, we're doing EXEs as the special case since there will be presumable be less of those.
	SET(PX_EXE_OUTPUT_DIRECTORY_DEBUG 		"${PX_OUTPUT_BIN_DIR}/${PX_ROOT_EXE_DIR}/debug"	  CACHE INTERNAL "Directory to put debug exes in")
	SET(PX_EXE_OUTPUT_DIRECTORY_PROFILE  	"${PX_OUTPUT_BIN_DIR}/${PX_ROOT_EXE_DIR}/profile" CACHE INTERNAL "Directory to put profile exes in")
	SET(PX_EXE_OUTPUT_DIRECTORY_CHECKED 	"${PX_OUTPUT_BIN_DIR}/${PX_ROOT_EXE_DIR}/checked" CACHE INTERNAL "Directory to put checked exes in")
	SET(PX_EXE_OUTPUT_DIRECTORY_RELEASE  	"${PX_OUTPUT_BIN_DIR}/${PX_ROOT_EXE_DIR}/release" CACHE INTERNAL "Directory to put release exes in")

	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 	${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG}			)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_PROFILE  ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_PROFILE}		)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_CHECKED 	${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_CHECKED}  		)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE  ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE}		)


ELSE()
	# old bitness suffix
	IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
		SET(LIBPATH_SUFFIX "x64")
	ELSE()
		SET(LIBPATH_SUFFIX "x86")
	ENDIF()				


	IF(DEFINED PX_OUTPUT_EXE_DIR)
		SetExeOutputPath(${PX_OUTPUT_EXE_DIR})
	ENDIF()
	IF(DEFINED PX_OUTPUT_DLL_DIR)
		SetDllOutputPath(${PX_OUTPUT_DLL_DIR})
	ENDIF()
	IF(DEFINED PX_OUTPUT_LIB_DIR)
		SetLibOutputPath(${PX_OUTPUT_LIB_DIR})
	ENDIF()
	# All EXE/DLL/LIB output will be overwritten if PX_OUTPUT_ALL_DIR is defined
	IF(DEFINED PX_OUTPUT_ALL_DIR)
		SetSingleOutputPath(${PX_OUTPUT_ALL_DIR})
	ENDIF()
ENDIF()

IF(APPEND_CONFIG_NAME)
	SET(CMAKE_DEBUG_POSTFIX   "DEBUG_${LIBPATH_SUFFIX}")
	SET(CMAKE_PROFILE_POSTFIX "PROFILE_${LIBPATH_SUFFIX}")
	SET(CMAKE_CHECKED_POSTFIX "CHECKED_${LIBPATH_SUFFIX}")
	SET(CMAKE_RELEASE_POSTFIX "_${LIBPATH_SUFFIX}")
ELSE()
	IF (DEFINED PX_OUTPUT_ARCH)  # platforms with fixed arch like ps4 dont need to have arch defined, then dont add bitness
		SET(CMAKE_DEBUG_POSTFIX "_${LIBPATH_SUFFIX}")
		SET(CMAKE_PROFILE_POSTFIX "_${LIBPATH_SUFFIX}")
		SET(CMAKE_CHECKED_POSTFIX "_${LIBPATH_SUFFIX}")
		SET(CMAKE_RELEASE_POSTFIX "_${LIBPATH_SUFFIX}")
	ENDIF()
ENDIF()

# Can no longer just use LIBPATH_SUFFIX since it depends on build type
IF(CMAKE_CL_64)
	SET(RESOURCE_LIBPATH_SUFFIX "x64")
ELSE(CMAKE_CL_64)
	SET(RESOURCE_LIBPATH_SUFFIX "x86")
ENDIF(CMAKE_CL_64)				


