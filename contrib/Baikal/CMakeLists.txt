CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

PROJECT(Baikal CXX)

OPTION(BAIKAL_ENABLE_RAYMASK "Enable visibility flags for shapes (slows down an intersector)" OFF)
OPTION(BAIKAL_ENABLE_DENOISER "Use denoising on output" ON)
OPTION(BAIKAL_ENABLE_RPR "Enable RadeonProRender API lib" ON)
OPTION(BAIKAL_EMBED_KERNELS "Embed CL kernels into binary module" OFF)

#Sanity checks
IF(BAIKAL_ENABLE_GLTF AND NOT BAIKAL_ENABLE_RPR)
    MESSAGE(FATAL_ERROR "glTF support requires RPR but it's disabled")
ENDIF(BAIKAL_ENABLE_GLTF AND NOT BAIKAL_ENABLE_RPR)

#global settings
if (WIN32)
    add_definitions(/MP -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
elseif (UNIX)
    add_definitions(-fvisibility=hidden -Wno-ignored-attributes)
endif (WIN32)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#Find required packages
FIND_PACKAGE(Threads REQUIRED)

#Configure RadeonRays build
SET(RR_EMBED_KERNELS ON CACHE BOOL "Embed CL kernels into binary module")
SET(RR_ALLOW_CPU_DEVICES OFF CACHE BOOL "Allows CPU Devices")
SET(RR_USE_OPENCL ON CACHE BOOL "Use OpenCL for GPU hit testing")
SET(RR_USE_EMBREE OFF CACHE BOOL "Use Intel(R) Embree for CPU hit testing")
SET(RR_USE_VULKAN OFF CACHE BOOL "Use vulkan for GPU hit testing")
SET(RR_NO_TESTS ON CACHE BOOL "Don't add any unit tests and remove any test functionality from the library")
SET(RR_ENABLE_STATIC OFF CACHE BOOL "Create static libraries rather than dynamic")
SET(RR_SHARED_CALC OFF CACHE BOOL "Link Calc(compute abstraction layer) dynamically")
SET(RR_ENABLE_RAYMASK $BAIKAL_ENABLE_RAYMASK CACHE BOOL "Enable ray masking in intersection kernels")
SET(RR_TUTORIALS OFF CACHE BOOL "Add tutorials projects")
SET(RR_SAFE_MATH OFF CACHE BOOL "use safe math")

ADD_SUBDIRECTORY(Baikal)

IF(BAIKAL_ENABLE_RPR)
    FIND_PACKAGE(OpenGL REQUIRED)
    SET(RprSupport_HEADER "${Baikal_SOURCE_DIR}/3rdparty/RprSupport/include/")
    ADD_SUBDIRECTORY(Rpr)
ENDIF(BAIKAL_ENABLE_RPR)

SET_TARGET_ATTRIBUTE(Baikal "contrib/Baikal SDK")
SET_TARGET_ATTRIBUTE(BaikalKernelsDir "contrib/Baikal SDK")
SET_TARGET_ATTRIBUTE(RadeonProRender64 "contrib/Baikal SDK")