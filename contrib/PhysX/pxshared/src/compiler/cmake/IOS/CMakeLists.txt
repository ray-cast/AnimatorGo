cmake_minimum_required(VERSION 3.3)
include(../common/CMakeLists.txt)


IF(NOT DEFINED TARGET_BUILD_PLATFORM) # Not defined, default to IOS
	SET(TARGET_BUILD_PLATFORM "IOS")
ENDIF()

SET(PLATFORM_LIST IOS)

IF (NOT ${TARGET_BUILD_PLATFORM} IN_LIST PLATFORM_LIST)
	MESSAGE(FATAL_ERROR "Invalid platform:" ${TARGET_BUILD_PLATFORM})
ENDIF()

SET(CMAKE_CXX_FLAGS "-std=c++11 -fno-rtti -fno-exceptions -ffast-math -ffunction-sections -fdata-sections -Werror -ferror-limit=0 -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-noreturn -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-covered-switch-default -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-unused-member-function -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-deprecated -Wno-non-virtual-dtor -Wno-invalid-noreturn -Wno-return-type-c-linkage -Wno-reserved-id-macro -Wno-c++98-compat-pedantic -Wno-unused-local-typedef -Wno-old-style-cast -Wno-newline-eof -Wno-unused-private-field -Wno-undefined-reinterpret-cast -Wno-invalid-offsetof -gdwarf-2")

SET(CMAKE_SHARED_LINKER_FLAGS "")

SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_CXX_FLAGS_CHECKED "-O3 -g")
SET(CMAKE_CXX_FLAGS_PROFILE "-O3 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")

SET(CMAKE_OSX_DEPLOYMENT_TARGET "")

SET(CMAKE_OSX_ARCHITECTURES "armv7 armv7s arm64")

SET(IOS_PLATFORM_LOCATION "iPhoneOS.platform")

# Setup iOS developer location unless specified manually with CMAKE_IOS_DEVELOPER_ROOT
EXEC_PROGRAM(/usr/bin/xcode-select ARGS -print-path OUTPUT_VARIABLE CMAKE_XCODE_DEVELOPER_DIR)
SET(XCODE_ROOT "${CMAKE_XCODE_DEVELOPER_DIR}/Platforms/${IOS_PLATFORM_LOCATION}/Developer")
IF(NOT DEFINED CMAKE_IOS_DEVELOPER_ROOT)
	IF(EXISTS ${XCODE_ROOT})
		SET(CMAKE_IOS_DEVELOPER_ROOT ${XCODE_ROOT})
	ENDIF(EXISTS ${XCODE_ROOT})
ENDIF(NOT DEFINED CMAKE_IOS_DEVELOPER_ROOT)
SET(CMAKE_IOS_DEVELOPER_ROOT ${CMAKE_IOS_DEVELOPER_ROOT} CACHE PATH "Location of iOS Platform")

# Find and use the most recent iOS sdk unless specified manually with CMAKE_IOS_SDK_ROOT
IF(NOT DEFINED CMAKE_IOS_SDK_ROOT)
	FILE(GLOB _CMAKE_IOS_SDKS "${CMAKE_IOS_DEVELOPER_ROOT}/SDKs/*")
	IF(_CMAKE_IOS_SDKS) 
		LIST(SORT _CMAKE_IOS_SDKS)
		LIST(REVERSE _CMAKE_IOS_SDKS)
		LIST(GET _CMAKE_IOS_SDKS 0 CMAKE_IOS_SDK_ROOT)
	ELSE(_CMAKE_IOS_SDKS)
		MESSAGE(FATAL_ERROR "No iOS SDK's found in default search path ${CMAKE_IOS_DEVELOPER_ROOT}. Manually set CMAKE_IOS_SDK_ROOT or install the iOS SDK.")
	ENDIF(_CMAKE_IOS_SDKS)
	MESSAGE(STATUS "Toolchain using default iOS SDK: ${CMAKE_IOS_SDK_ROOT}")
ENDIF(NOT DEFINED CMAKE_IOS_SDK_ROOT)
SET(CMAKE_IOS_SDK_ROOT ${CMAKE_IOS_SDK_ROOT} CACHE PATH "Location of the selected iOS SDK")

# Set the sysroot default to the most recent SDK
SET(CMAKE_OSX_SYSROOT ${CMAKE_IOS_SDK_ROOT} CACHE PATH "Sysroot used for iOS support")

SET(CMAKE_XCODE_ATTRIBUTE_SUPPORTED_PLATFORMS "iphoneos")
SET(CMAKE_XCODE_ATTRIBUTE_SDKROOT ${CMAKE_IOS_SDK_ROOT})
SET(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "7.0")

SET(PXSHARED_IOS_COMPILE_DEFS _LIB;DISABLE_CUDA_PHYSX;DISABLE_COMPUTE_PHYSX)
SET(PXSHARED_IOS_DEBUG_COMPILE_DEFS _DEBUG;PX_DEBUG=1;PX_CHECKED=1)
SET(PXSHARED_IOS_CHECKED_COMPILE_DEFS NDEBUG;PX_CHECKED=1)
SET(PXSHARED_IOS_PROFILE_COMPILE_DEFS NDEBUG;PX_PROFILE=1)
SET(PXSHARED_IOS_RELEASE_COMPILE_DEFS NDEBUG)

# NOTE: PxCudaContextManager excluded on this platform

# Include project cmake files here
INCLUDE(PxFoundation.cmake)
INCLUDE(PsFastXml.cmake)
INCLUDE(PxPvdSDK.cmake)
INCLUDE(PxTask.cmake)
