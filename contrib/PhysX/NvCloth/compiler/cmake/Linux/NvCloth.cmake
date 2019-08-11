#
# Build NvCloth (PROJECT not SOLUTION)
#

MESSAGE("[NvCloth]cmake/linux/NvCloth.cmake")

SET(GW_DEPS_ROOT $ENV{GW_DEPS_ROOT})
FIND_PACKAGE(PxShared REQUIRED)

IF(NOT DEFINED NV_CLOTH_ENABLE_CUDA)
MESSAGE("NV_CLOTH_ENABLE_CUDA is not defined, setting to 0")
SET(NV_CLOTH_ENABLE_CUDA 0)
ENDIF()
IF(NOT DEFINED NV_CLOTH_ENABLE_DX11)
SET(NV_CLOTH_ENABLE_DX11 0)
ENDIF()

MESSAGE("NV_CLOTH_ENABLE_CUDA = " ${NV_CLOTH_ENABLE_CUDA})

IF(${NV_CLOTH_ENABLE_CUDA})
FIND_PACKAGE(CUDA 8 REQUIRED)
MESSAGE("Found CUDA:" ${CUDA_INCLUDE_DIRS})
ENDIF()

#FIND_PACKAGE(nvToolsExt REQUIRED)

SET(NVCLOTH_PLATFORM_INCLUDES
	${NVTOOLSEXT_INCLUDE_DIRS}
	PRIVATE ${CUDA_INCLUDE_DIRS}
)

SET(NVCLOTH_PLATFORM_SOURCE_FILES
	${PROJECT_ROOT_DIR}/src/ps/unix/PsUnixAtomic.cpp
	${PROJECT_ROOT_DIR}/src/ps/unix/PsUnixFPU.h
	#${PROJECT_ROOT_DIR}/src/neon/NeonCollision.cpp
	#${PROJECT_ROOT_DIR}/src/neon/NeonSelfCollision.cpp
	#${PROJECT_ROOT_DIR}/src/neon/NeonSolverKernel.cpp
	#${PROJECT_ROOT_DIR}/src/neon/SwCollisionHelpers.h
)
IF(${NV_CLOTH_ENABLE_CUDA})
LIST(APPEND NVCLOTH_PLATFORM_SOURCE_FILES
	${PROJECT_ROOT_DIR}/src/cuda/CuCheckSuccess.h
	${PROJECT_ROOT_DIR}/src/cuda/CuCloth.cpp
	${PROJECT_ROOT_DIR}/src/cuda/CuCloth.h
	${PROJECT_ROOT_DIR}/src/cuda/CuClothClone.cpp
	${PROJECT_ROOT_DIR}/src/cuda/CuClothData.cpp
	${PROJECT_ROOT_DIR}/src/cuda/CuClothData.h
	${PROJECT_ROOT_DIR}/src/cuda/CuCollision.h
	${PROJECT_ROOT_DIR}/src/cuda/CuContextLock.cpp
	${PROJECT_ROOT_DIR}/src/cuda/CuContextLock.h
	${PROJECT_ROOT_DIR}/src/cuda/CuDevicePointer.h
	${PROJECT_ROOT_DIR}/src/cuda/CuDeviceVector.h
	${PROJECT_ROOT_DIR}/src/cuda/CuFabric.cpp
	${PROJECT_ROOT_DIR}/src/cuda/CuFabric.h
	${PROJECT_ROOT_DIR}/src/cuda/CuFactory.cpp
	${PROJECT_ROOT_DIR}/src/cuda/CuFactory.h
	${PROJECT_ROOT_DIR}/src/cuda/CuPhaseConfig.h
	${PROJECT_ROOT_DIR}/src/cuda/CuPinnedAllocator.h
	${PROJECT_ROOT_DIR}/src/cuda/CuSelfCollision.h
	${PROJECT_ROOT_DIR}/src/cuda/CuSolver.cpp
	${PROJECT_ROOT_DIR}/src/cuda/CuSolver.h
	${PROJECT_ROOT_DIR}/src/cuda/CuSolverKernel.h
	${PROJECT_ROOT_DIR}/src/cuda/CuSolverKernelBlob.h
)
ENDIF()


IF(${NV_CLOTH_ENABLE_CUDA})
set(
	CUDA_NVCC_FLAGS
	${CUDA_NVCC_FLAGS};

	# Options are documented on http://docs.nvidia.com/cuda/cuda-compiler-driver-nvcc/index.html#command-option-types-and-notation
	# Important: to be able to debug CUDA kernel (correct stepping and workable warp watches),
	# you should add the -G (short for --device-debug) option to the line below.
	# Don't forget to remove it afterwards, since the code execution will be very slow with this flag.
	-use_fast_math -ftz=true -prec-div=false -prec-sqrt=false -lineinfo -gencode arch=compute_20,code=sm_20 -gencode arch=compute_30,code=sm_30 -gencode arch=compute_35,code=sm_35 -gencode arch=compute_50,code=sm_50 -gencode arch=compute_50,code=compute_50 -D_USE_MATH_DEFINES -DNDEBUG -DPX_PROFILE=1
)
SET(NVCLOTH_CUDA_FILES
	${PROJECT_ROOT_DIR}/src/cuda/CuSolverKernel.cu
)
set(CUDA_BLOB "${PROJECT_ROOT_DIR}/src/cuda/CuSolverKernelBlob.h")
cuda_include_directories(
	${PXSHARED_ROOT_DIR}/include
	${PXSHARED_ROOT_DIR}/src/foundation/include
	${PROJECT_ROOT_DIR}/include/NvCloth
)
cuda_compile_fatbin(CUDA_FATBIN ${NVCLOTH_CUDA_FILES})
SET(NVCLOTH_PLATFORM_SOURCE_FILES ${NVCLOTH_PLATFORM_SOURCE_FILES} ${CUDA_BLOB} ${NVCLOTH_CUDA_FILES})
add_custom_command(
	OUTPUT ${CUDA_BLOB}
COMMAND ${CUDA_TOOLKIT_ROOT_DIR}/bin/bin2c --name kCuSolverKernel ${CUDA_FATBIN} > ${CUDA_BLOB}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
DEPENDS ${CUDA_FATBIN}
	VERBATIM
)
ENDIF()



# Use generator expressions to set config specific preprocessor definitions
SET(NVCLOTH_COMPILE_DEFS
	NV_CLOTH_IMPORT=PX_DLL_EXPORT
	NV_CLOTH_ENABLE_DX11=0

	# Common to all configurations
	${PHYSX_LINUX_COMPILE_DEFS};PX_PHYSX_CORE_EXPORTS

	$<$<CONFIG:debug>:${PHYSX_LINUX_DEBUG_COMPILE_DEFS};PX_PHYSX_DLL_NAME_POSTFIX=DEBUG;>
	$<$<CONFIG:checked>:${PHYSX_LINUX_CHECKED_COMPILE_DEFS};PX_PHYSX_DLL_NAME_POSTFIX=CHECKED;>
	$<$<CONFIG:profile>:${PHYSX_LINUX_PROFILE_COMPILE_DEFS};PX_PHYSX_DLL_NAME_POSTFIX=PROFILE;>
	$<$<CONFIG:release>:${PHYSX_LINUX_RELEASE_COMPILE_DEFS};>
)
IF(${NV_CLOTH_ENABLE_CUDA})
LIST(APPEND NVCLOTH_COMPILE_DEFS
	NV_CLOTH_ENABLE_CUDA=1
)
ELSE()
LIST(APPEND NVCLOTH_COMPILE_DEFS
	NV_CLOTH_ENABLE_CUDA=0
)
ENDIF()

IF(DEFINED PX_STATIC_LIBRARIES)
	SET(NVCLOTH_LIBTYPE STATIC)
ELSE()
	SET(NVCLOTH_LIBTYPE SHARED)
ENDIF()

# include common NvCloth settings
INCLUDE(../common/NvCloth.cmake)

TARGET_LINK_LIBRARIES(NvCloth PUBLIC ${CUDA_CUDA_LIBRARY})

SET_TARGET_PROPERTIES(NvCloth PROPERTIES 
	LINK_FLAGS_DEBUG ""
	LINK_FLAGS_CHECKED ""
	LINK_FLAGS_PROFILE ""
	LINK_FLAGS_RELEASE ""
)

# enable -fPIC so we can link static libs with the editor
SET_TARGET_PROPERTIES(NvCloth PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
MESSAGE("[NvCloth]cmake/linux/NvCloth.cmake END")
