#
# Build NvCloth
#

#FIND_PACKAGE(CUDA 8 REQUIRED)

SET(NVCLOTH_PLATFORM_INCLUDES PRIVATE ${CUDA_INCLUDE_DIRS})

SET(NVCLOTH_PLATFORM_SOURCE_FILES
	#${PROJECT_SOURCE_DIR}/src/cuda/CuCheckSuccess.h
	#${PROJECT_SOURCE_DIR}/src/cuda/CuCloth.cpp
	#${PROJECT_SOURCE_DIR}/src/cuda/CuCloth.h
	#${PROJECT_SOURCE_DIR}/src/cuda/CuClothClone.cpp
	#${PROJECT_SOURCE_DIR}/src/cuda/CuClothData.cpp
	#${PROJECT_SOURCE_DIR}/src/cuda/CuClothData.h
	#${PROJECT_SOURCE_DIR}/src/cuda/CuCollision.h
	#${PROJECT_SOURCE_DIR}/src/cuda/CuContextLock.cpp
	#${PROJECT_SOURCE_DIR}/src/cuda/CuContextLock.h
	#${PROJECT_SOURCE_DIR}/src/cuda/CuDevicePointer.h
	#${PROJECT_SOURCE_DIR}/src/cuda/CuDeviceVector.h
	#${PROJECT_SOURCE_DIR}/src/cuda/CuFabric.cpp
	#${PROJECT_SOURCE_DIR}/src/cuda/CuFabric.h
	#${PROJECT_SOURCE_DIR}/src/cuda/CuFactory.cpp
	#${PROJECT_SOURCE_DIR}/src/cuda/CuFactory.h
	#${PROJECT_SOURCE_DIR}/src/cuda/CuPhaseConfig.h
	#${PROJECT_SOURCE_DIR}/src/cuda/CuPinnedAllocator.h
	#${PROJECT_SOURCE_DIR}/src/cuda/CuSelfCollision.h
	#${PROJECT_SOURCE_DIR}/src/cuda/CuSolver.cpp
	#${PROJECT_SOURCE_DIR}/src/cuda/CuSolver.h
	#${PROJECT_SOURCE_DIR}/src/cuda/CuSolverKernel.h
	#${PROJECT_SOURCE_DIR}/src/cuda/CuSolverKernelBlob.h
	${PROJECT_SOURCE_DIR}/src/dx/DxBatchedVector.h
	${PROJECT_SOURCE_DIR}/src/dx/DxCheckSuccess.h
	${PROJECT_SOURCE_DIR}/src/dx/DxCloth.cpp
	${PROJECT_SOURCE_DIR}/src/dx/DxCloth.h
	${PROJECT_SOURCE_DIR}/src/dx/DxClothClone.cpp
	${PROJECT_SOURCE_DIR}/src/dx/DxClothData.cpp
	${PROJECT_SOURCE_DIR}/src/dx/DxClothData.h
	${PROJECT_SOURCE_DIR}/src/dx/DxContextLock.cpp
	${PROJECT_SOURCE_DIR}/src/dx/DxContextLock.h
	${PROJECT_SOURCE_DIR}/src/dx/DxDeviceVector.h
	${PROJECT_SOURCE_DIR}/src/dx/DxFabric.cpp
	${PROJECT_SOURCE_DIR}/src/dx/DxFabric.h
	${PROJECT_SOURCE_DIR}/src/dx/DxFactory.cpp
	${PROJECT_SOURCE_DIR}/src/dx/DxFactory.h
	${PROJECT_SOURCE_DIR}/src/dx/DxSolver.cpp
	${PROJECT_SOURCE_DIR}/src/dx/DxSolver.h
	${PROJECT_SOURCE_DIR}/src/dx/DxSolverKernelBlob.h
	${PROJECT_SOURCE_DIR}/src/dx/DxSortKernel.inc
	#${PROJECT_SOURCE_DIR}/src/neon/NeonCollision.cpp
	#${PROJECT_SOURCE_DIR}/src/neon/NeonSelfCollision.cpp
	#${PROJECT_SOURCE_DIR}/src/neon/NeonSolverKernel.cpp
	#${PROJECT_SOURCE_DIR}/src/neon/SwCollisionHelpers.h
)

SET(NVCLOTH_AVX_SOURCE_FILES
		${PROJECT_SOURCE_DIR}/src/avx/SwSolveConstraints.cpp
)

set_source_files_properties(${NVCLOTH_AVX_SOURCE_FILES} PROPERTIES COMPILE_FLAGS "/arch:AVX")

SET(NVCLOTH_HLSL_FILES
	${PROJECT_SOURCE_DIR}/src/dx/DxSolverKernel.hlsl
)
set_source_files_properties(${NVCLOTH_HLSL_FILES} PROPERTIES VS_SHADER_TYPE Compute VS_SHADER_MODEL 5.0 VS_SHADER_FLAGS "/Vn gDxSolverKernel /Fh ${PROJECT_SOURCE_DIR}/src/dx/DxSolverKernelBlob.h")
SET(NVCLOTH_PLATFORM_SOURCE_FILES ${NVCLOTH_PLATFORM_SOURCE_FILES} ${NVCLOTH_HLSL_FILES} ${NVCLOTH_AVX_SOURCE_FILES})

set(
	CUDA_NVCC_FLAGS
	${CUDA_NVCC_FLAGS};
	-use_fast_math -ftz=true -prec-div=false -prec-sqrt=false -lineinfo -gencode arch=compute_20,code=sm_20 -gencode arch=compute_30,code=sm_30 -gencode arch=compute_35,code=sm_35 -gencode arch=compute_50,code=sm_50 -gencode arch=compute_50,code=compute_50 -D_USE_MATH_DEFINES -DNDEBUG -DPX_PROFILE=1
)
SET(NVCLOTH_CUDA_FILES
	${PROJECT_SOURCE_DIR}/src/cuda/CuSolverKernel.cu
)
set(CUDA_BLOB "${PROJECT_SOURCE_DIR}/src/cuda/CuSolverKernelBlob.h")
#cuda_include_directories(
#	${PXSHARED_ROOT_DIR}/include
#	${PXSHARED_ROOT_DIR}/src/foundation/include
#	${PROJECT_SOURCE_DIR}/include/NvCloth
#)
#cuda_compile_fatbin(CUDA_FATBIN ${NVCLOTH_CUDA_FILES})
#SET(NVCLOTH_PLATFORM_SOURCE_FILES ${NVCLOTH_PLATFORM_SOURCE_FILES} ${CUDA_BLOB} ${NVCLOTH_CUDA_FILES})
#add_custom_command(
#	OUTPUT ${CUDA_BLOB}
#COMMAND ${CUDA_TOOLKIT_ROOT_DIR}/bin/bin2c.exe --name kCuSolverKernel ${CUDA_FATBIN} > ${CUDA_BLOB}
#	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#DEPENDS ${CUDA_FATBIN}
#	VERBATIM
#)

SET(NVCLOTH_COMPILE_DEFS
	NV_CLOTH_IMPORT=PX_DLL_EXPORT
	NV_CLOTH_ENABLE_DX11=1
	NV_CLOTH_ENABLE_CUDA=0

	$<$<CONFIG:debug>:${NVCLOTH_WINDOWS_DEBUG_COMPILE_DEFS};PX_PHYSX_DLL_NAME_POSTFIX=DEBUG;>
	$<$<CONFIG:checked>:${NVCLOTH_WINDOWS_CHECKED_COMPILE_DEFS};PX_PHYSX_DLL_NAME_POSTFIX=CHECKED;>
	$<$<CONFIG:profile>:${NVCLOTH_WINDOWS_PROFILE_COMPILE_DEFS};PX_PHYSX_DLL_NAME_POSTFIX=PROFILE;>
	$<$<CONFIG:release>:${NVCLOTH_WINDOWS_RELEASE_COMPILE_DEFS};>
)

SET(NVCLOTH_LIBTYPE SHARED)

INCLUDE(../common/NvCloth.cmake)