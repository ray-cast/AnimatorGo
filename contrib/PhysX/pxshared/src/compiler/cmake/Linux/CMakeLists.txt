cmake_minimum_required(VERSION 3.3)
include(../common/CMakeLists.txt)

STRING(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWERCASE)

IF(NOT DEFINED TARGET_BUILD_PLATFORM) # Not defined, default to Linux
	SET(TARGET_BUILD_PLATFORM "Linux")
ENDIF()

SET(PLATFORM_LIST Linux)

IF (NOT ${TARGET_BUILD_PLATFORM} IN_LIST PLATFORM_LIST)
	MESSAGE(FATAL_ERROR "Invalid platform:" ${TARGET_BUILD_PLATFORM})
ENDIF()

IF (${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "x86_64-unknown-linux-gnu" OR ${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "x86_64-linux-gnu")
	IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	  # using Clang	  
	  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -ffast-math -ffunction-sections -fdata-sections -Werror -ferror-limit=0 -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-noreturn -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-covered-switch-default -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-unused-member-function -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-deprecated -Wno-non-virtual-dtor -Wno-invalid-noreturn -Wno-return-type-c-linkage -Wno-reserved-id-macro -Wno-c++98-compat-pedantic -Wno-unused-local-typedef -Wno-old-style-cast -Wno-newline-eof -Wno-unused-private-field -Wno-undefined-func-template -Wno-format-nonliteral -Wno-implicit-fallthrough -Wno-undefined-reinterpret-cast")
	ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	  # using GCC
	  SET(LIBPATH_SUFFIX "x64")
	  SET(CMAKE_CXX_FLAGS "-Werror -m64 -fPIC -msse2 -mfpmath=sse -ffast-math -fno-exceptions -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden -Wall -Wextra -fno-strict-aliasing -fdiagnostics-show-option -Wno-invalid-offsetof -Wno-uninitialized -Wno-missing-field-initializers")	  
	ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")	
ELSEIF(${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "arm-unknown-linux-gnueabihf")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -mfpu=neon -fno-rtti -fno-exceptions -ffast-math -ffunction-sections -fdata-sections -Werror -ferror-limit=0 -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-noreturn -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-covered-switch-default -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-unused-member-function -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-deprecated -Wno-non-virtual-dtor -Wno-old-style-cast -Wno-return-type-c-linkage -Wno-format-nonliteral -Wno-implicit-fallthrough")
ELSEIF(${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "aarch64-unknown-linux-gnueabi")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -ffast-math -ffunction-sections -fdata-sections -Werror -ferror-limit=0 -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-noreturn -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-covered-switch-default -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-unused-member-function -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-deprecated -Wno-non-virtual-dtor -Wno-old-style-cast -Wno-return-type-c-linkage -Wno-format-nonliteral -Wno-unused-local-typedef -Wno-implicit-fallthrough")
ELSE(${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "x86_64-unknown-linux-gnu" OR ${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "x86_64-linux-gnu")
	MESSAGE(FATAL_ERROR "Unknown CMAKE_LIBRARY_ARCHITECTURE ${CMAKE_LIBRARY_ARCHITECTURE}")
ENDIF(${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "x86_64-unknown-linux-gnu" OR ${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "x86_64-linux-gnu")


SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -gdwarf-3")
SET(CMAKE_CXX_FLAGS_CHECKED "-O3 -g -gdwarf-3")
SET(CMAKE_CXX_FLAGS_PROFILE "-O3 -g -gdwarf-3")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -gdwarf-3")

IF(DEFINED PX_GENERATE_GPU_PROJECTS)
SET(PXSHARED_LINUX_COMPILE_DEFS _LIB)
ELSE()
# Disable cuda and dx for all projects on windows
SET(PXSHARED_LINUX_COMPILE_DEFS _LIB;DISABLE_CUDA_PHYSX;)
ENDIF()
SET(PXSHARED_LINUX_DEBUG_COMPILE_DEFS _DEBUG;PX_DEBUG=1;PX_CHECKED=1)
SET(PXSHARED_LINUX_CHECKED_COMPILE_DEFS NDEBUG;PX_CHECKED=1)
SET(PXSHARED_LINUX_PROFILE_COMPILE_DEFS NDEBUG;PX_PROFILE=1)
SET(PXSHARED_LINUX_RELEASE_COMPILE_DEFS NDEBUG)

IF(DEFINED LIBPATH_SUFFIX)
	SET(CMAKE_DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}_${LIBPATH_SUFFIX}")
	SET(CMAKE_PROFILE_POSTFIX "${CMAKE_PROFILE_POSTFIX}_${LIBPATH_SUFFIX}")
	SET(CMAKE_CHECKED_POSTFIX "${CMAKE_CHECKED_POSTFIX}_${LIBPATH_SUFFIX}")
	SET(CMAKE_RELEASE_POSTFIX "${CMAKE_RELEASE_POSTFIX}_${LIBPATH_SUFFIX}")
ENDIF()

# NOTE: PxCudaContextManager excluded on this platform

# Include project cmake files here
IF(DEFINED PX_SELECT_COMPONENTS)
	if ("PxFoundation" IN_LIST PX_SELECT_COMPONENTS)
		INCLUDE(PxFoundation.cmake)
	endif()
	if ("PsFastXml" IN_LIST PX_SELECT_COMPONENTS)
		INCLUDE(PsFastXml.cmake)
	endif()
	if ("PxPvdSDK" IN_LIST PX_SELECT_COMPONENTS)
		INCLUDE(PxPvdSDK.cmake)
	endif()
	if ("PxTask" IN_LIST PX_SELECT_COMPONENTS)
		INCLUDE(PxTask.cmake)
	endif()
	if ("PxCudaContextManager" IN_LIST PX_SELECT_COMPONENTS)
		IF(DEFINED PX_GENERATE_GPU_PROJECTS)
			INCLUDE(PxCudaContextManager.cmake)
		ENDIF()
	endif()
ELSE()
INCLUDE(PxFoundation.cmake)
INCLUDE(PsFastXml.cmake)
INCLUDE(PxPvdSDK.cmake)
INCLUDE(PxTask.cmake)
IF(DEFINED PX_GENERATE_GPU_PROJECTS)
	INCLUDE(PxCudaContextManager.cmake)
ENDIF()
ENDIF()

