// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2014 NVIDIA Corporation. All rights reserved.

#include "foundation/PxAssert.h"
#include "foundation/PxErrorCallback.h"
#include "foundation/PxMath.h"
#include "foundation/PxPreprocessor.h"

#include "cudamanager/PxCudaContextManager.h"
#include "task/PxGpuDispatcher.h"

#include "CudaMemoryManager.h"
#include "GpuDispatcher.h"
#include "PhysXDeviceSettings.h"

#include "PsMutex.h"
#include "PsThread.h"
#include "PsUserAllocated.h"
#include "PsString.h"

#include <cuda.h>

#if PX_WIN32 || PX_WIN64

#ifdef PX_SECURE_LOAD_LIBRARY
#include "nvSecureLoadLibrary.h"
#endif

#pragma warning (push)
#pragma warning (disable : 4668) //'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
#include <windows.h>
#pragma warning (pop)

class IDirect3DDevice9;
class IDirect3DResource9;
class IDirect3DVertexBuffer9;
#include <cudad3d9.h>

class IDXGIAdapter;
class ID3D10Device;
class ID3D10Resource;
#include <cudad3d10.h>

struct ID3D11Device;
struct ID3D11Resource;
#include <cudad3d11.h>

#endif // PX_WINDOWS_FAMILY

#if PX_LINUX
#include <dlfcn.h>
static void* GetProcAddress(void* handle, const char* name) { return dlsym(handle, name); }
#endif

#include <GL/gl.h>
#include <cudaGL.h>
#include <assert.h>

#include "foundation/PxErrors.h"
#include "foundation/PxErrorCallback.h"

#define CU_INIT_UUID
#include "CudaNode3DLowLatencyInterface.h"

#define ENABLE_DEVICE_INFO_BRINGUP	0

#include "GPUProfile.h"

#if ENABLE_CUDA_DEVICE_RESET
#include "cudaProfiler.h"
#endif

#if USE_PERFKIT
#pragma warning (push)
#pragma warning (disable : 4099)
#pragma warning (disable : 4191)
#define NVPM_INITGUID
#include <stdio.h>
#include "cuda.h"
#include "../../../../../../../externals/nvPerfKit/4.1.0.14260/inc/NvPmApi.Manager.h"
static NvPmApiManager S_NVPMManager;
extern NvPmApiManager *GetNvPmApiManager() {return &S_NVPMManager;}
const NvPmApi *GetNvPmApi() {return S_NVPMManager.Api();}
NVPMContext hNVPMContext(0);

void initPerfKit()
{
	//Sync with GPU
	cuCtxSynchronize();
	
	// Reset counters
	uint32_t nCount;
	GetNvPmApi()->Sample(hNVPMContext, NULL, &nCount);
}

void endPerfKit()
{
	//Sync with GPU
	cuCtxSynchronize();
	
	uint32_t nCount;
	GetNvPmApi()->Sample(hNVPMContext, NULL, &nCount);
	
	uint64_t value;
	uint64_t cycle;
	
	uint64_t sum = 0;
	uint64_t maxVal = 0;
	char name[512];

	int nvStatus = 0;

	PX_UNUSED(value);
	PX_UNUSED(cycle);
	PX_UNUSED(sum);
	PX_UNUSED(maxVal);
	PX_UNUSED(name);
	PX_UNUSED(nvStatus);

	printf("counters:\n");

#if COUNT_L2_TO_L1_BYTES
	nvStatus |= GetNvPmApi()->GetCounterValueByName(hNVPMContext, "l2_read_bytes", 0, &value, &cycle);
	printf("L2->L1 bytes %d\n",value);
#elif COUNT_SM_TO_L1_QUERIES
	nvStatus |= GetNvPmApi()->GetCounterValueByName(hNVPMContext, "tex_cache_sector_queries", 0, &value, &cycle);
	printf("SM->L1 queries %d\n",value);
#endif

#if COUNT_INST_EXECUTED || COUNT_STORE_INST_EXECUTED || COUNT_ACTIVE_CYCLES || COUNT_ACTIVE_WARPS
	for (int i = 0; i != SM_COUNT; i++)
	{
#if COUNT_INST_EXECUTED
		sprintf_s(name, 512, "sm_inst_executed_vsm%d", i);
#elif COUNT_STORE_INST_EXECUTED
		sprintf_s(name, 512, "sm_inst_executed_global_stores_vsm%d", i);
#elif COUNT_ACTIVE_CYCLES
		sprintf_s(name, 512, "sm_active_cycles_vsm%d", i);
#elif COUNT_ACTIVE_WARPS
		sprintf_s(name, 512, "sm_active_warps_vsm%d", i);
#endif
		nvStatus |= GetNvPmApi()->GetCounterValueByName(hNVPMContext, name, 0, &value, &cycle);
		
		sum += value;
		maxVal = physx::PxMax(maxVal, value);
	}
#if COUNT_ACTIVE_CYCLES
	printf("sum %I64d\n", sum);
#else
	printf("sum %I64d\n", sum);
#endif

	if (!nvStatus)
	{
		PX_ASSERT(false);
	}
#endif
}

#pragma warning (pop)
#endif

namespace physx
{

#if PX_VC
#pragma warning(disable: 4191)	//'operator/operation' : unsafe conversion from 'type of expression' to 'type required'
#endif

#define MIN_SM_MAJOR_VERSION	2
#define MIN_SM_MINOR_VERSION	0

class CudaCtxMgr : public PxCudaContextManager, public shdfnd::UserAllocated
{
public:
	CudaCtxMgr(const PxCudaContextManagerDesc& desc, PxErrorCallback& errorCallback);
	~CudaCtxMgr();

	bool            safeDelayImport(PxErrorCallback& errorCallback);
	CUcontext       acquireContext();
	void            releaseContext();

	/* All these methods can be called without acquiring the context */

	PxCudaMemoryManager* getMemoryManager();
	PxGpuDispatcher* getGpuDispatcher();

	bool            contextIsValid() const;
	bool            supportsArchSM10() const;  // G80
	bool            supportsArchSM11() const;  // G92
	bool            supportsArchSM12() const;
	bool            supportsArchSM13() const;  // GT200
	bool            supportsArchSM20() const;  // GF100
	bool            supportsArchSM30() const;  // GK100
	bool            supportsArchSM35() const;  // GK110
	bool            supportsArchSM50() const;  // GM100
	bool            supportsArchSM52() const;  // GM200
	bool            isIntegrated() const;      // true if GPU is integrated (MCP) part
	bool            canMapHostMemory() const;  // true if GPU map host memory to GPU
	int             getDriverVersion() const;
	size_t          getDeviceTotalMemBytes() const;
	int				getMultiprocessorCount() const;
	int             getSharedMemPerBlock() const;
	int             getSharedMemPerMultiprocessor() const;
	unsigned int	getMaxThreadsPerBlock() const;
	unsigned int	getClockRate() const;

	const char*     getDeviceName() const;
	const CUdevprop* getDeviceProperties() const;

	PxCudaInteropMode::Enum	getInteropMode() const;

	void			setUsingConcurrentStreams(bool);
	bool			getUsingConcurrentStreams() const;

	bool registerResourceInCudaD3D(CUgraphicsResource& resource, void* resourcePointer, PxCudaInteropRegisterFlags flags);
	bool registerResourceInCudaGL(CUgraphicsResource& resource, uint32_t buffer, PxCudaInteropRegisterFlags flags);
	bool unregisterResourceInCuda(CUgraphicsResource resource);

	/*
	\brief Determine if the user has configured a dedicated PhysX GPU in the NV Control Panel
	\returns 1 if there is a dedicated PhysX GPU
	\returns 0 if there is NOT a dedicated PhysX GPU
	\returns -1 if the routine is not implemented
	*/
	int				usingDedicatedGPU() const;

	void            release();

private:

	int             mSceneCount;
	bool            mIsValid;
	bool			mOwnContext;
	CUdevice        mDevHandle;
	CUcontext       mCtx;
	CudaMemMgr* 	mMemMgr;

	GpuDispatcherImpl* mDispatcher;
    CUetblPhysXInterface* m_physXInterface;

	/* Cached device attributes, so threads can query w/o context */
	int             mComputeCapMajor;
	int             mComputeCapMinor;
	int				mIsIntegrated;
	int				mCanMapHost;
	int				mDriverVersion;
	size_t			mTotalMemBytes;
	int				mMultiprocessorCount;
	int				mMaxThreadsPerBlock;
	char            mDeviceName[128];
	int				mSharedMemPerBlock;
	int				mSharedMemPerMultiprocessor;
	int				mClockRate;
	PxCudaInteropMode::Enum mInteropMode;
	bool            mUsingConcurrentStreams;

#if PX_DEBUG
	static uint32_t    mManagerRefCount;
	static uint32_t    mContextRefCountTls;
#endif
};

#if PX_DEBUG
uint32_t CudaCtxMgr::mManagerRefCount = 0;
uint32_t CudaCtxMgr::mContextRefCountTls = 0;
#endif

bool CudaCtxMgr::contextIsValid() const
{
	return mIsValid;
}
bool CudaCtxMgr::supportsArchSM10() const
{
	return mIsValid;
}
bool CudaCtxMgr::supportsArchSM11() const
{
	return mIsValid && (mComputeCapMinor >= 1 || mComputeCapMajor > 1);
}
bool CudaCtxMgr::supportsArchSM12() const
{
	return mIsValid && (mComputeCapMinor >= 2 || mComputeCapMajor > 1);
}
bool CudaCtxMgr::supportsArchSM13() const
{
	return mIsValid && (mComputeCapMinor >= 3 || mComputeCapMajor > 1);
}
bool CudaCtxMgr::supportsArchSM20() const
{
	return mIsValid && mComputeCapMajor >= 2;
}
bool CudaCtxMgr::supportsArchSM30() const
{
	return mIsValid && mComputeCapMajor >= 3;
}
bool CudaCtxMgr::supportsArchSM35() const
{
	return mIsValid && ((mComputeCapMajor > 3) || (mComputeCapMajor == 3 && mComputeCapMinor >= 5));
}
bool CudaCtxMgr::supportsArchSM50() const
{
	return mIsValid && mComputeCapMajor >= 5;
}
bool CudaCtxMgr::supportsArchSM52() const
{
	return mIsValid && ((mComputeCapMajor > 5) || (mComputeCapMajor == 5 && mComputeCapMinor >= 2));
}

bool CudaCtxMgr::isIntegrated() const
{
	return mIsValid && mIsIntegrated;
}
bool CudaCtxMgr::canMapHostMemory() const
{
	return mIsValid && mCanMapHost;
}
int  CudaCtxMgr::getDriverVersion() const
{
	return mDriverVersion;
}
size_t  CudaCtxMgr::getDeviceTotalMemBytes() const
{
	return mTotalMemBytes;
}
int	CudaCtxMgr::getMultiprocessorCount() const
{
	return mMultiprocessorCount;
}
int CudaCtxMgr::getSharedMemPerBlock() const
{
	return mSharedMemPerBlock;
}
int CudaCtxMgr::getSharedMemPerMultiprocessor() const
{
	return mSharedMemPerMultiprocessor;
}
unsigned int CudaCtxMgr::getMaxThreadsPerBlock() const
{
	return (unsigned int)mMaxThreadsPerBlock;
}
unsigned int CudaCtxMgr::getClockRate() const
{
	return (unsigned int)mClockRate;
}

const char* CudaCtxMgr::getDeviceName() const
{
	if (mIsValid)
	{
		return mDeviceName;
	}
	else
	{
		return "Invalid";
	}
}

PxCudaInteropMode::Enum CudaCtxMgr::getInteropMode() const
{
	return mInteropMode;
}

void CudaCtxMgr::setUsingConcurrentStreams(bool value)
{
	mUsingConcurrentStreams = value;
}

bool CudaCtxMgr::getUsingConcurrentStreams() const
{
	return mUsingConcurrentStreams;
}

PxCudaMemoryManager* CudaCtxMgr::getMemoryManager()
{
	if (mIsValid)
	{
		return mMemMgr;
	}
	else
	{
		return NULL;
	}
}

PxGpuDispatcher* CudaCtxMgr::getGpuDispatcher()
{
	if (mIsValid)
	{
		return mDispatcher;
	}
	else
	{
		return NULL;
	}
}

int CudaCtxMgr::usingDedicatedGPU() const
{
	if (PxCudaInteropMode::NO_INTEROP == getInteropMode())
	{
		return PhysXDeviceSettings::isUsingDedicatedGPU();
	}
	else
	{
		return 0; // not a dedicated GPU
	}
}

#define CUT_SAFE_CALL(call)  { CUresult ret = call;	\
		if( CUDA_SUCCESS != ret ) { PX_ASSERT(0); } }

/* If a context is not provided, an ordinal must be given */
CudaCtxMgr::CudaCtxMgr(const PxCudaContextManagerDesc& desc, PxErrorCallback& errorCallback)
	: mSceneCount(0)
	, mOwnContext(false)
	, mMemMgr(0)
	, mDispatcher(0)
	, m_physXInterface(0)
	, mInteropMode(desc.interopMode)
	, mUsingConcurrentStreams(true)
{
	CUresult status;
	mIsValid = false;
	mDeviceName[0] = 0;

	if (safeDelayImport(errorCallback) == false)
	{
		// The table where this info is found is here: https://wiki.nvidia.com/nvcompute/index.php/NVCompute#CUDA_Planning
		errorCallback.reportError(PxErrorCode::eDEBUG_INFO, "NVIDIA Release 331 graphics driver and above is required for GPU acceleration.", __FILE__, __LINE__);
		return;
	}

	if (desc.ctx == 0)
	{
		int flags = CU_CTX_LMEM_RESIZE_TO_MAX | CU_CTX_SCHED_BLOCKING_SYNC | CU_CTX_MAP_HOST;
		class FoundationErrorReporter : public PxErrorCallback
		{
		public:
			FoundationErrorReporter(PxErrorCallback& ec)
			: errorCallback(&ec)
			{
			}

			virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
			{
				errorCallback->reportError( code, message, file, line);
			}

			PxErrorCallback* errorCallback;
		} foundationErrorReporter(errorCallback);

		int devOrdinal = PhysXDeviceSettings::getSuggestedCudaDeviceOrdinal(foundationErrorReporter);
		if (devOrdinal < 0)
		{
			errorCallback.reportError(PxErrorCode::eDEBUG_INFO, "No PhysX capable GPU suggested.", __FILE__, __LINE__);
			errorCallback.reportError(PxErrorCode::eDEBUG_INFO, "If you have a PhysX capable GPU, verify that PhysX is not set to CPU in the NVIDIA Control Panel.", __FILE__, __LINE__);
			return;
		}

		status = cuInit(0);
		if (CUDA_SUCCESS != status)
		{
			errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "cuInit failed", __FILE__, __LINE__);
			return;
		}
		
		// Try to create the context on Node3DLowLatency.
		// If that does not work, try to create the cuda context using cuCtxCreatePhysX,
		// since we must be on a driver that does not support cuCtxCreateOnNode3DLowLatency.
		cuGetExportTable((const void**)&m_physXInterface, (const CUuuid*)&CU_ETID_PhysXInterface);

		// if using a dedicated GPU or SLI we disable D3D interop (which is not supported over multiple GPUs)
		// this ensures the users control panel setting is always respected
		bool sliEnabled = false;
		if (mInteropMode != PxCudaInteropMode::NO_INTEROP && desc.graphicsDevice != NULL)
		{
			sliEnabled = PhysXDeviceSettings::isSLIEnabled(desc.graphicsDevice) == 1 ? true : false;
		}

		if (PhysXDeviceSettings::isUsingDedicatedGPU() == 1 || sliEnabled)
		{
			if (mInteropMode == PxCudaInteropMode::D3D9_INTEROP ||
				mInteropMode == PxCudaInteropMode::D3D10_INTEROP ||
				mInteropMode == PxCudaInteropMode::D3D11_INTEROP)
			{
				mInteropMode = PxCudaInteropMode::NO_INTEROP;
				if (sliEnabled)
				{
					errorCallback.reportError(PxErrorCode::eDEBUG_INFO, "D3D/CUDA interop cannot be used in tandem with SLI, disabling interop.  Query PxCudaContextManager::getInteropMode() for interop status.",
						__FILE__,__LINE__);
				}
			}
		}

		if (mInteropMode == PxCudaInteropMode::NO_INTEROP)
		{
			status = cuDeviceGet(&mDevHandle, devOrdinal);
			if (CUDA_SUCCESS != status)
			{
				errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "cuDeviceGet failed",__FILE__,__LINE__);
				return;
			}

			if (m_physXInterface)
				status = m_physXInterface->cuCtxCreateOnNode3DLowLatency(&mCtx, (unsigned int)flags, mDevHandle);
			else
				status = cuCtxCreate(&mCtx, (unsigned int)flags, mDevHandle);
			if (CUDA_SUCCESS != status)
			{
				errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "cuCtxCreate failed",__FILE__,__LINE__);
				return;
			}
			mOwnContext = true;
		}
		else if (mInteropMode == PxCudaInteropMode::OGL_INTEROP)
		{
			status = cuDeviceGet(&mDevHandle, devOrdinal);
			if (CUDA_SUCCESS != status)
			{
				errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "cuDeviceGet failed",__FILE__,__LINE__);
				return;
			}

			status = cuGLCtxCreate(&mCtx, (unsigned int)flags, mDevHandle);
			if (CUDA_SUCCESS != status)
			{
				errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "cuCtxGLCreate failed",__FILE__,__LINE__);
				return;
			}

			status = cuGLInit();
			if (CUDA_SUCCESS != status)
			{
				errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "cuGLInit failed",__FILE__,__LINE__);
				return;
			}
			mOwnContext = true;
		}
#if PX_WIN32 || PX_WIN64
		else if (mInteropMode == PxCudaInteropMode::D3D9_INTEROP)
		{
			status = cuD3D9CtxCreate(&mCtx, &mDevHandle, (unsigned int)flags,
			                         reinterpret_cast<IDirect3DDevice9*>(desc.graphicsDevice));

			if (CUDA_SUCCESS != status)
			{
				errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "cuD3D9CtxCreate failed",__FILE__,__LINE__);
				return;
			}
			mOwnContext = true;
		}
		else if (mInteropMode == PxCudaInteropMode::D3D10_INTEROP)
		{
			status = cuD3D10CtxCreate(&mCtx, &mDevHandle, (unsigned int)flags,
			                          reinterpret_cast<ID3D10Device*>(desc.graphicsDevice));

			if (CUDA_SUCCESS != status)
			{
				errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "cuD3D10CtxCreate failed",__FILE__,__LINE__);
				return;
			}
			mOwnContext = true;
		}
		else if (mInteropMode == PxCudaInteropMode::D3D11_INTEROP)
		{
			status = cuD3D11CtxCreate(&mCtx, &mDevHandle, (unsigned int)flags,
			                          reinterpret_cast<ID3D11Device*>(desc.graphicsDevice));

			if (CUDA_SUCCESS != status)
			{
				errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "cuD3D11CtxCreate failed",__FILE__,__LINE__);
				return;
			}
			mOwnContext = true;
		}
#endif //PX_WIN32 || PX_WIN64
		else
		{
			errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "Requested interop type is not supported!",__FILE__,__LINE__);
			return;
		}
	}
	else
	{
		mCtx = *desc.ctx;
		status = cuCtxGetDevice(&mDevHandle);
		if (CUDA_SUCCESS != status)
		{
			errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "cuCtxGetDevice failed",__FILE__,__LINE__);
			return;
		}
	}

	// Verify we can at least allocate a CUDA event from this context
	CUevent testEvent;
	if (CUDA_SUCCESS == cuEventCreate(&testEvent, 0))
	{
		cuEventDestroy(testEvent);
	}
	else
	{
		errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "CUDA context validation failed",__FILE__,__LINE__);
		return;
	}

	status = cuDeviceGetName(mDeviceName, sizeof(mDeviceName), mDevHandle);
	if (CUDA_SUCCESS != status)
	{
		errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "cuDeviceGetName failed",__FILE__,__LINE__);
		return;
	}

	cuDeviceGetAttribute(&mSharedMemPerBlock, CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK, mDevHandle);
	cuDeviceGetAttribute(&mSharedMemPerMultiprocessor, CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_MULTIPROCESSOR, mDevHandle);
	cuDeviceGetAttribute(&mClockRate, CU_DEVICE_ATTRIBUTE_CLOCK_RATE, mDevHandle);
	cuDeviceGetAttribute(&mComputeCapMajor, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, mDevHandle);
	cuDeviceGetAttribute(&mComputeCapMinor, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, mDevHandle);
	cuDeviceGetAttribute(&mIsIntegrated, CU_DEVICE_ATTRIBUTE_INTEGRATED, mDevHandle);
	cuDeviceGetAttribute(&mCanMapHost, CU_DEVICE_ATTRIBUTE_CAN_MAP_HOST_MEMORY, mDevHandle);
	cuDeviceGetAttribute(&mMultiprocessorCount, CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, mDevHandle);
	cuDeviceGetAttribute(&mMaxThreadsPerBlock, CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK, mDevHandle);

	status = cuDeviceTotalMem((size_t*)&mTotalMemBytes, mDevHandle);
	if (CUDA_SUCCESS != status)
	{
		errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "cuDeviceTotalMem failed",__FILE__,__LINE__);
		return;
	}

	// minimum compute capability is MIN_SM_MAJOR_VERSION.MIN_SM_MINOR_VERSION
	if ((mComputeCapMajor < MIN_SM_MAJOR_VERSION)	||
		(mComputeCapMajor == MIN_SM_MAJOR_VERSION && mComputeCapMinor < MIN_SM_MINOR_VERSION))
	{
		char buffer[256];
		physx::shdfnd::snprintf(buffer, 256, "Minimum GPU compute capability %d.%d is required", MIN_SM_MAJOR_VERSION, MIN_SM_MINOR_VERSION);
		errorCallback.reportError(PxErrorCode::eDEBUG_WARNING,buffer,__FILE__,__LINE__);
		return;
	}

	mMemMgr = PX_NEW(CudaMemMgr)(*this, errorCallback);
	if (mMemMgr == NULL)
	{
		errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "CudaMemMgr failed: Unable to allocate heaps",__FILE__,__LINE__);
		return;
	}

	bool succ = true;
	for (uint32_t i = 0; i < PxCudaBufferMemorySpace::COUNT; i++)
	{
		PxCudaBufferType type(PxCudaBufferMemorySpace::Enum(i), PxCudaBufferFlags::F_READ_WRITE);
		succ &= mMemMgr->setBaseSize(type, desc.memoryBaseSize[i]);
		succ &= mMemMgr->setPageSize(type, desc.memoryPageSize[i]);
		succ &= mMemMgr->setMaxMemorySize(type, desc.maxMemorySize[i]);
		PX_ASSERT(succ);
		if (!succ)
		{
			errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "CudaMemMgr failed: Invalid memory parameter",__FILE__,__LINE__);
			return;
		}
	}

#if PX_DEBUG
	if(!mManagerRefCount++)
		mContextRefCountTls = shdfnd::TlsAlloc();
	if(!shdfnd::TlsGet(mContextRefCountTls))
		CUT_SAFE_CALL(cuCtxSetCurrent(0));
#endif

	mIsValid = true;
	mDispatcher = PX_NEW(GpuDispatcherImpl)(errorCallback, *this);
	if (!mDispatcher || mDispatcher->failureDetected())
	{
		errorCallback.reportError(PxErrorCode::eDEBUG_WARNING, "Failed to create functional GPU dispatcher",__FILE__,__LINE__);
		mIsValid = false;
		return;
	}

	mDispatcher->start();

#if ENABLE_DEVICE_INFO_BRINGUP
	// Device info (Enable for Amodel and Emulator testing)
	errorCallback.reportError(PxErrorCode::eDEBUG_INFO, "Device Name: %s", mDeviceName);
	errorCallback.reportError(PxErrorCode::eDEBUG_INFO, "Shared Memory Per Block: %d", mSharedMemPerBlock);
	errorCallback.reportError(PxErrorCode::eDEBUG_INFO, "Shared Memory Per Multiprocessor: %d", mSharedMemPerMultiprocessor);
	errorCallback.reportError(PxErrorCode::eDEBUG_INFO, "Number of SM: %d", mMultiprocessorCount);
	errorCallback.reportError(PxErrorCode::eDEBUG_INFO, "Max Threads Per Block: %d", mMaxThreadsPerBlock);
#endif

#if USE_PERFKIT
	{
#if _WIN64
		wchar_t * dllName = L"..\\..\\..\\..\\..\\externals\\nvPerfKit\\4.1.0.14260\\bin\\win7_x64\\NvPmApi.Core.dll";
#else
		wchar_t * dllName = L"..\\..\\..\\..\\..\\externals\\nvPerfKit\\4.1.0.14260\\bin\\win7_x86\\NvPmApi.Core.dll";
#endif

		NVPMRESULT nvResult;

		if ((nvResult = GetNvPmApiManager()->Construct(dllName)) != NVPM_OK)
		{
			printf("perfkit error 1\n");
			return; 
		}

		if ((nvResult = GetNvPmApi()->Init()) != NVPM_OK)
		{
			printf("perfkit error 2\n");
			return; 
		}

		acquireContext();

		CUcontext ctx;
		cuCtxGetCurrent(&ctx);
		if ((nvResult = GetNvPmApi()->CreateContextFromCudaContext((APIContextHandle)ctx, &hNVPMContext)) != NVPM_OK)
		{
			printf("perfkit error 3\n");
			return; // This is an error condition
		}

		uint32_t nvStatus = 0;

#if COUNT_L2_TO_L1_BYTES
		nvStatus |= GetNvPmApi()->AddCounterByName(hNVPMContext, "l2_read_bytes");
#elif COUNT_SM_TO_L1_QUERIES
		nvStatus |= GetNvPmApi()->AddCounterByName(hNVPMContext, "tex_cache_sector_queries");
#endif

#if COUNT_INST_EXECUTED || COUNT_STORE_INST_EXECUTED || COUNT_ACTIVE_CYCLES || COUNT_ACTIVE_WARPS
		char name[512];
		for (int i = 0; i != SM_COUNT; i++)
		{
#if COUNT_INST_EXECUTED
			sprintf_s(name,512,"sm_inst_executed_vsm%d",i);
#elif COUNT_STORE_INST_EXECUTED
			sprintf_s(name, 512, "sm_inst_executed_global_stores_vsm%d",i);
#elif COUNT_ACTIVE_CYCLES
			sprintf_s(name, 512, "sm_active_cycles_vsm%d",i);
#elif COUNT_ACTIVE_WARPS
			sprintf_s(name, 512, "sm_active_warps_vsm%d",i);
#endif
			nvStatus |= GetNvPmApi()->AddCounterByName(hNVPMContext, name);
		}
#elif COUNT_GPU_BUSY
		nvStatus |= GetNvPmApi()->AddCounterByName(hNVPMContext, "gpu_busy");
#endif

		if (nvStatus != 0)
		{
			printf("perfkit error 4\n");
			return; // This is an error condition
		}
	}
#endif
}

/* Some driver version mismatches can cause delay import crashes.  Load NVCUDA.dll
 * manually, verify its version number, then allow delay importing to bind all the
 * APIs.
 */
bool CudaCtxMgr::safeDelayImport(PxErrorCallback& errorCallback)
{
#if PX_WIN32 || PX_WIN64
#ifdef PX_SECURE_LOAD_LIBRARY
	HMODULE hCudaDriver = nvLoadSystemLibrary("nvcuda.dll");
#else
	HMODULE hCudaDriver = LoadLibrary("nvcuda.dll");
#endif
#elif PX_LINUX
	void*	hCudaDriver = dlopen("libcuda.so", RTLD_NOW);
#endif
	if (!hCudaDriver)
	{
		errorCallback.reportError(PxErrorCode::eDEBUG_INFO, "nvcuda.dll not found or could not be loaded.", __FILE__, __LINE__);
		return false;
	}

	typedef CUresult(CUDAAPI * pfnCuDriverGetVersion_t)(int*);
	pfnCuDriverGetVersion_t pfnCuDriverGetVersion = (pfnCuDriverGetVersion_t) GetProcAddress(hCudaDriver, "cuDriverGetVersion");
	if (!pfnCuDriverGetVersion)
	{
		errorCallback.reportError(PxErrorCode::eDEBUG_INFO, "cuDriverGetVersion missing in nvcuda.dll.", __FILE__, __LINE__);
		return false;
	}

	CUresult status = pfnCuDriverGetVersion(&mDriverVersion);
	if (status != CUDA_SUCCESS)
	{
		errorCallback.reportError(PxErrorCode::eDEBUG_INFO, "Retrieving CUDA driver version failed.", __FILE__, __LINE__);
		return false;
	}

	/* Let's require a driver version >= to the version we compile against
	 * Currently, CUDA_VERSION is 6000 or 6.0, but APEX still uses CUDA 5.0 so we can't assert on 6.0 yet.
	 */
	PX_COMPILE_TIME_ASSERT(5000 <= CUDA_VERSION);

	if (mDriverVersion < CUDA_VERSION)
	{
		char buffer[256];
		physx::shdfnd::snprintf(buffer, 256, "CUDA driver version is %u, expected at least %u.", mDriverVersion, CUDA_VERSION);
		errorCallback.reportError(PxErrorCode::eDEBUG_INFO, buffer, __FILE__,__LINE__);
		return false;
	}

	/* Now trigger delay import and API binding */
	status = cuDriverGetVersion(&mDriverVersion);
	if (status != CUDA_SUCCESS)
	{
		errorCallback.reportError(PxErrorCode::eDEBUG_INFO, "Failed to bind CUDA API.", __FILE__, __LINE__);
		return false;
	}

	/* Not strictly necessary, but good practice */
#if PX_WIN32 | PX_WIN64
	FreeLibrary(hCudaDriver);
#elif PX_LINUX
	dlclose(hCudaDriver);
#endif
	

	return true;
}

void CudaCtxMgr::release()
{
	delete this;
}

CudaCtxMgr::~CudaCtxMgr()
{
	if (mDispatcher)
	{
		releaseGpuDispatcher(*mDispatcher);
	}
	if (mMemMgr)
	{
		delete mMemMgr;
	}
	if (mOwnContext)
	{
		CUT_SAFE_CALL(cuCtxDestroy(mCtx));
	}

#if PX_DEBUG
	if(!--mManagerRefCount)
		 shdfnd::TlsFree(mContextRefCountTls);
#endif

#if ENABLE_CUDA_DEVICE_RESET
	CUT_SAFE_CALL(cuProfilerStop());
#endif
}

bool CudaCtxMgr::registerResourceInCudaGL(CUgraphicsResource& resource, uint32_t buffer, PxCudaInteropRegisterFlags flags)
{
	CUresult ret = CUDA_ERROR_UNKNOWN;

	acquireContext();

	PX_ASSERT(mInteropMode == PxCudaInteropMode::OGL_INTEROP);

	ret = cuGraphicsGLRegisterBuffer(&resource, (GLuint) buffer, uint32_t(flags));

	releaseContext();

	return ret == CUDA_SUCCESS;
}

bool CudaCtxMgr::registerResourceInCudaD3D(CUgraphicsResource& resource, void* resourcePointer, PxCudaInteropRegisterFlags flags)
{
	CUresult ret = CUDA_ERROR_UNKNOWN;
#if PX_WINDOWS_FAMILY
	acquireContext();

	switch (mInteropMode)
	{
	case PxCudaInteropMode::D3D9_INTEROP:
		ret = cuGraphicsD3D9RegisterResource(&resource, (IDirect3DResource9*)resourcePointer, uint32_t(flags));
		break;
	case PxCudaInteropMode::D3D10_INTEROP:
		ret = cuGraphicsD3D10RegisterResource(&resource, (ID3D10Resource*)resourcePointer, uint32_t(flags));
		break;
	case PxCudaInteropMode::D3D11_INTEROP:
		ret = cuGraphicsD3D11RegisterResource(&resource, (ID3D11Resource*)resourcePointer, uint32_t(flags));
		break;
	case PxCudaInteropMode::NO_INTEROP:
	case PxCudaInteropMode::OGL_INTEROP:
	case PxCudaInteropMode::COUNT:
	default:
		PX_ALWAYS_ASSERT_MESSAGE("unexpected state in registerResourceInCuda3D");
	}

	releaseContext();
#else
	PX_UNUSED(resource);
	PX_UNUSED(resourcePointer);
	PX_UNUSED(flags);
#endif //PX_WINDOWS_FAMILY
	return ret == CUDA_SUCCESS;
}

bool CudaCtxMgr::unregisterResourceInCuda(CUgraphicsResource resource)
{
	CUresult ret = CUDA_ERROR_UNKNOWN;

	acquireContext();

	ret = cuGraphicsUnregisterResource(resource);

	releaseContext();

	return ret == CUDA_SUCCESS;
}

CUcontext CudaCtxMgr::acquireContext()
{
	CUcontext ctx = 0;
	CUT_SAFE_CALL(cuCtxGetCurrent(&ctx));

	if (ctx != mCtx)
	{
#if PX_DEBUG
		PX_ASSERT(!shdfnd::TlsGet(mContextRefCountTls));
#endif
		CUT_SAFE_CALL(cuCtxSetCurrent(mCtx));
	}

#if PX_DEBUG
	char* refCount = (char*)shdfnd::TlsGet(mContextRefCountTls);
	shdfnd::TlsSet(mContextRefCountTls, ++refCount);
#endif

	return mCtx;
}

void CudaCtxMgr::releaseContext()
{
#if PX_DEBUG
	char* refCount = (char*)shdfnd::TlsGet(mContextRefCountTls);
	shdfnd::TlsSet(mContextRefCountTls, --refCount);
	// see DE8475
	if(!refCount)
		CUT_SAFE_CALL(cuCtxSetCurrent(0));
#endif
}

#if PX_SUPPORT_GPU_PHYSX
extern "C" void initUtilKernels();

PxCudaContextManager* createCudaContextManager(const PxCudaContextManagerDesc& desc, PxErrorCallback& errorCallback)
{
	//this call is needed to force UtilKernels linkage in case someone links PxCudaContextManager as Static Library!
	initUtilKernels();

	return PX_NEW(CudaCtxMgr)(desc, errorCallback);
}

#endif

} // end physx namespace


