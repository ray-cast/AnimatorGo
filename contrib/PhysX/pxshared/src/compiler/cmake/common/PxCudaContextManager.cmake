#
# Build PxCudaContextManager common
#


# CUDA!
SET(CUDA_NVCC_FLAGS "-lineinfo -use_fast_math -ftz=true -prec-div=false -prec-sqrt=false  -gencode arch=compute_30,code=sm_30 -gencode arch=compute_35,code=sm_35 -gencode arch=compute_50,code=sm_50 -gencode arch=compute_50,code=compute_50 -D_CONSOLE -D_WIN32_WINNT=0x0501")

CUDA_INCLUDE_DIRECTORIES(
	${PXSHARED_SOURCE_DIR}/../include
	${PXSHARED_SOURCE_DIR}/foundation/include
	${PXSHARED_SOURCE_DIR}/cudamanager/include
)

SET(CUDA_PROPAGATE_HOST_FLAGS OFF)

# Compile the CuKernelSolver - it has different options than the other CUDA files
SET(CUDA_NVCC_FLAGS_DEBUG   "-D_DEBUG -D_CONSOLE -D_WIN32_WINNT=0x0501 ${CUDA_COMPILER_OPTION_DEBUG}")
SET(CUDA_NVCC_FLAGS_CHECKED "-DNDEBUG -D_CONSOLE -D_WIN32_WINNT=0x0501 ${CUDA_COMPILER_OPTION_CHECKED}")
SET(CUDA_NVCC_FLAGS_PROFILE "-DNDEBUG -D_CONSOLE -D_WIN32_WINNT=0x0501 ${CUDA_COMPILER_OPTION_PROFILE}")
SET(CUDA_NVCC_FLAGS_RELEASE "-DNDEBUG -D_CONSOLE -D_WIN32_WINNT=0x0501 ${CUDA_COMPILER_OPTION_RELEASE}")

SET(CUDACONTEXTMANAGER_HEADERS	
	${PXSHARED_SOURCE_DIR}/../include/cudamanager/PxCudaContextManager.h
	${PXSHARED_SOURCE_DIR}/../include/cudamanager/PxCudaMemoryManager.h
	${PXSHARED_SOURCE_DIR}/../include/cudamanager/PxGpuCopyDesc.h
	${PXSHARED_SOURCE_DIR}/../include/cudamanager/PxGpuCopyDescQueue.h
)
SOURCE_GROUP(include FILES ${CUDACONTEXTMANAGER_HEADERS})

SET(CUDACONTEXTMANAGER_KERNELS
	${LL_SOURCE_DIR}/src/CUDA/UtilKernels.cu
)
SOURCE_GROUP("src kernels" FILES ${CUDACONTEXTMANAGER_KERNELS})

SET(CUDACONTEXTMANAGER_SOURCE
	${LL_SOURCE_DIR}/src/CudaContextManager.cpp
	${LL_SOURCE_DIR}/src/CudaKernelWrangler.cpp
	${LL_SOURCE_DIR}/src/CudaMemoryManager.cpp
	${LL_SOURCE_DIR}/src/HeapManagerRef.cpp
	${LL_SOURCE_DIR}/src/GpuDispatcher.cpp
	${LL_SOURCE_DIR}/src/BlockingWait.cpp
	${LL_SOURCE_DIR}/src/PhysXDeviceSettings.cpp
)
SOURCE_GROUP(src\\src FILES ${CUDACONTEXTMANAGER_SOURCE})

SET(CUDACONTEXTMANAGER_SOURCE_HEADERS
	${LL_SOURCE_DIR}/include/CudaContextManager.h
	${LL_SOURCE_DIR}/include/CudaKernelWrangler.h
	${LL_SOURCE_DIR}/include/GpuDispatcher.h
	${LL_SOURCE_DIR}/include/PhysXDeviceSettings.h
)
SOURCE_GROUP(src\\src FILES ${CUDACONTEXTMANAGER_SOURCE_HEADERS})

CUDA_ADD_LIBRARY(PxCudaContextManager STATIC 
	${CUDACONTEXTMANAGER_HEADERS}
	${CUDACONTEXTMANAGER_SOURCE}
	${CUDACONTEXTMANAGER_SOURCE_HEADERS}
	
	${CUDACONTEXTMANAGER_KERNELS}	
)

# Target specific compile options


TARGET_INCLUDE_DIRECTORIES(PxCudaContextManager 
	PRIVATE ${PXSHARED_SOURCE_DIR}/../include
	PRIVATE ${PXSHARED_SOURCE_DIR}/foundation/include
	PRIVATE ${PXSHARED_SOURCE_DIR}/task/include
	PRIVATE ${PXSHARED_SOURCE_DIR}/cudamanager/include
	PRIVATE ${LL_SOURCE_DIR}/include
	PRIVATE ${CUDA_INCLUDE_DIRS}

)



