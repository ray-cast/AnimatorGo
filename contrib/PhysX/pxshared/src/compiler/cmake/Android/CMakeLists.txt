cmake_minimum_required(VERSION 3.3)
include(../common/CMakeLists.txt)

STRING(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWERCASE)

IF(NOT DEFINED TARGET_BUILD_PLATFORM) # Not defined, default to Android
	SET(TARGET_BUILD_PLATFORM "Android")
ENDIF()

SET(PLATFORM_LIST Android)

IF (NOT ${TARGET_BUILD_PLATFORM} IN_LIST PLATFORM_LIST)
	MESSAGE(FATAL_ERROR "Invalid platform:" ${TARGET_BUILD_PLATFORM})
ENDIF()

if(${ANDROID_ABI} STREQUAL "armeabi-v7a")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++11 -fno-rtti -fno-exceptions -ffast-math -ffunction-sections -fdata-sections -D__STDC_LIMIT_MACROS -Wno-invalid-offsetof ")
elseif(${ANDROID_ABI} STREQUAL "arm64-v8a")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++11 -fno-rtti -fno-exceptions -ffast-math -ffunction-sections -fdata-sections -D__STDC_LIMIT_MACROS -Wno-invalid-offsetof ")
elseif(${ANDROID_ABI} STREQUAL "x86")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++11 -fno-rtti -fno-exceptions -ffast-math -ffunction-sections -fdata-sections -D__STDC_LIMIT_MACROS -Wno-invalid-offsetof -fpack-struct=8 -malign-double ")
elseif(${ANDROID_ABI} STREQUAL "x86_64")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++11 -fno-rtti -fno-exceptions -ffast-math -ffunction-sections -fdata-sections -D__STDC_LIMIT_MACROS -Wno-invalid-offsetof -mstackrealign -msse3  ")
endif()
	
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_CXX_FLAGS_CHECKED "-O2")
SET(CMAKE_CXX_FLAGS_PROFILE "-O2")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2")

SET(PXSHARED_ANDROID_COMPILE_DEFS _LIB;)
SET(PXSHARED_ANDROID_DEBUG_COMPILE_DEFS _DEBUG;PX_DEBUG=1;PX_CHECKED=1)
SET(PXSHARED_ANDROID_CHECKED_COMPILE_DEFS NDEBUG;PX_CHECKED=1)
SET(PXSHARED_ANDROID_PROFILE_COMPILE_DEFS NDEBUG;PX_PROFILE=1)
SET(PXSHARED_ANDROID_RELEASE_COMPILE_DEFS NDEBUG)

# NOTE: PxCudaContextManager excluded on this platform

# Include project cmake files here
INCLUDE(PxFoundation.cmake)
INCLUDE(PsFastXml.cmake)
INCLUDE(PxPvdSDK.cmake)
INCLUDE(PxTask.cmake)
