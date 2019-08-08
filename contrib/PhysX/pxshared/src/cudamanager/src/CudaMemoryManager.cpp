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
// Copyright (c) 2008-2017 NVIDIA Corporation. All rights reserved.

#include "cudamanager/PxCudaContextManager.h"
#include "foundation/PxMath.h"
#include "foundation/PxMemory.h"

#include "CudaMemoryManager.h"
#include "HeapManagerRef.h"

#include <cuda.h>

#define DEVICE_BASE_SIZE (0)
#define DEVICE_PAGE_SIZE ( 2 * 1024*1024)
#define PINNED_BASE_SIZE (0)
#define PINNED_PAGE_SIZE ( 2 * 1024*1024)
#define WC_BASE_SIZE (0)
#define WC_PAGE_SIZE ( 2 * 1024*1024)
#define MIN_BLOCK_SIZE 2048


#define CMM_DELETE_SINGLE(x)	{ if(x) delete x; }
#define CMM_DELETE_ARRAY(x)		{ if(x) delete [] x; }

using namespace physx;

CudaMemMgr::CudaMemMgr(PxCudaContextManager& mgr, physx::PxErrorCallback& errorCallback)
	: mErrorCallback(errorCallback)
	, mBufferPool("mBufferPool", 1024)
	, mInitialized(false)
	, mMgr(mgr)
	, mDebugDisableAllocs(false)
{
	for (uint32_t i = 0; i < PxCudaBufferMemorySpace::COUNT; i++)
	{
		mHeap[i] = NULL;
		mMemoryAllocator[i] = NULL;
		mMemoryMaxSize[i] = size_t(-1);
	}

	mMemoryBaseSize[PxCudaBufferMemorySpace::T_GPU] = DEVICE_BASE_SIZE;
	mMemoryBaseSize[PxCudaBufferMemorySpace::T_PINNED_HOST] = PINNED_BASE_SIZE;
	mMemoryBaseSize[PxCudaBufferMemorySpace::T_WRITE_COMBINED] = WC_BASE_SIZE;
	mMemoryBaseSize[PxCudaBufferMemorySpace::T_HOST] = 0;

	mMemoryPageSize[PxCudaBufferMemorySpace::T_GPU] = DEVICE_PAGE_SIZE;
	mMemoryPageSize[PxCudaBufferMemorySpace::T_PINNED_HOST] = PINNED_PAGE_SIZE;
	mMemoryPageSize[PxCudaBufferMemorySpace::T_WRITE_COMBINED] = WC_PAGE_SIZE;
	mMemoryPageSize[PxCudaBufferMemorySpace::T_HOST] = PINNED_PAGE_SIZE;
}


CudaMemMgr::~CudaMemMgr()
{
	for (uint32_t i = 0; i < PxCudaBufferMemorySpace::COUNT; i++)
	{
		CMM_DELETE_SINGLE(mHeap[i]);
		CMM_DELETE_SINGLE(mMemoryAllocator[i]);
	}
}


PX_INLINE bool CudaMemMgr::initialize()
{
	if (mInitialized)
	{
		return true;
	}

	for (uint32_t i = 0; i < PxCudaBufferMemorySpace::COUNT; i++)
	{
		mHeap[i] = PX_NEW(HeapManagerRef)(mErrorCallback, false);
		PX_ASSERT(mHeap[i]);
	}

	mMemoryAllocator[PxCudaBufferMemorySpace::T_GPU]				= PX_NEW(DeviceMemAllocator)(mMgr, mMemoryMaxSize[PxCudaBufferMemorySpace::T_GPU]);
	mMemoryAllocator[PxCudaBufferMemorySpace::T_PINNED_HOST]		= PX_NEW(PinnedMemAllocator)(mMgr, mMemoryMaxSize[PxCudaBufferMemorySpace::T_PINNED_HOST]);
	mMemoryAllocator[PxCudaBufferMemorySpace::T_WRITE_COMBINED]	= PX_NEW(WriteCombinedMemAllocator)(mMgr, mMemoryMaxSize[PxCudaBufferMemorySpace::T_WRITE_COMBINED]);
	mMemoryAllocator[PxCudaBufferMemorySpace::T_HOST]				= PX_NEW(HostMemAllocator)(mMemoryMaxSize[PxCudaBufferMemorySpace::T_HOST]);

	bool succ = true;
	for (uint32_t i = 0; i < PxCudaBufferMemorySpace::COUNT; i++)
	{
		succ &= mHeap[i]->init(mMemoryAllocator[i], mMemoryBaseSize[i], mMemoryPageSize[i], MIN_BLOCK_SIZE);
		PX_ASSERT(succ);
	}

	for (uint32_t i = 0; i < PxCudaBufferMemorySpace::COUNT; i++)
	{
		succ &= mHeap[i] && mMemoryAllocator[i];
	}

	if (!succ)
	{
		for (uint32_t i = 0; i < PxCudaBufferMemorySpace::COUNT; i++)
		{
			CMM_DELETE_SINGLE(mHeap[i]);
			CMM_DELETE_SINGLE(mMemoryAllocator[i]);
		}
		mInitialized = false;
	}

	return mInitialized = succ;;
}


bool CudaMemMgr::setPageSize(const PxCudaBufferType& type, size_t size)
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

	if (!mInitialized)
	{
		mMemoryPageSize[type.memorySpace] = PxMax(mMemoryPageSize[type.memorySpace], size);
		return true;
	}
	else
	{
		bool ret = mHeap[type.memorySpace]->setPageSize(size);
		mMemoryPageSize[type.memorySpace] = ret ? size : mMemoryPageSize[type.memorySpace];
		return ret;
	}
}


bool CudaMemMgr::setBaseSize(const PxCudaBufferType& type, size_t size)
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

	if (mInitialized || (((size - 1)&size) != 0))
	{
		return false;
	}
	else
	{
		mMemoryBaseSize[type.memorySpace] = PxMax(mMemoryBaseSize[type.memorySpace], size);
		return true;
	}
}


size_t CudaMemMgr::getBaseSize(const PxCudaBufferType& type)
{
	return mMemoryBaseSize[type.memorySpace];
}


size_t CudaMemMgr::getPageSize(const PxCudaBufferType& type)
{
	return mMemoryPageSize[type.memorySpace];
}


bool CudaMemMgr::setMaxMemorySize(const PxCudaBufferType& type, size_t size)
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

	if (mInitialized)
	{
		switch (type.memorySpace)
		{
		case PxCudaBufferMemorySpace::T_GPU:
			static_cast<DeviceMemAllocator*>(mMemoryAllocator[type.memorySpace])->setMaxSize(size);
			return true;
			break;
		case PxCudaBufferMemorySpace::T_PINNED_HOST:
			static_cast<PinnedMemAllocator*>(mMemoryAllocator[type.memorySpace])->setMaxSize(size);
			return true;
			break;
		case PxCudaBufferMemorySpace::T_HOST:
			static_cast<HostMemAllocator*>(mMemoryAllocator[type.memorySpace])->setMaxSize(size);
			return true;
			break;
		case PxCudaBufferMemorySpace::T_WRITE_COMBINED:
			static_cast<WriteCombinedMemAllocator*>(mMemoryAllocator[type.memorySpace])->setMaxSize(size);
			return true;
			break;
		case PxCudaBufferMemorySpace::COUNT:
		default:
			PX_ASSERT(!"unknown memory type");
			break;
		}
	}
	else
	{
		mMemoryMaxSize[type.memorySpace] = PxMax(mMemoryMaxSize[type.memorySpace], size);
		return true;
	}

	return false;
}

size_t CudaMemMgr::getMaxMemorySize(const PxCudaBufferType& type)
{
	if (mInitialized)
	{
		switch (type.memorySpace)
		{
		case PxCudaBufferMemorySpace::T_GPU:
			return static_cast<DeviceMemAllocator*>(mMemoryAllocator[type.memorySpace])->getMaxSize();
			break;
		case PxCudaBufferMemorySpace::T_PINNED_HOST:
			return static_cast<PinnedMemAllocator*>(mMemoryAllocator[type.memorySpace])->getMaxSize();
			break;
		case PxCudaBufferMemorySpace::T_HOST:
			return static_cast<HostMemAllocator*>(mMemoryAllocator[type.memorySpace])->getMaxSize();
			break;
		case PxCudaBufferMemorySpace::T_WRITE_COMBINED:
			return static_cast<WriteCombinedMemAllocator*>(mMemoryAllocator[type.memorySpace])->getMaxSize();
			break;
		case PxCudaBufferMemorySpace::COUNT:
		default:
			PX_ASSERT(!"unknown memory type");
			break;
		}
	}
	return 0;
}

bool CudaMemMgr::reserve(const PxCudaBufferType& type, size_t size)
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

	if (!mInitialized)
	{
		if (!initialize())
		{
			return false;
		}
	}

	return mHeap[type.memorySpace]->reserve(size);
}


PxCudaBuffer* CudaMemMgr::alloc(const PxCudaBufferType& type, size_t size, PX_ALLOC_INFO_PARAMS_DEF())
{
	PxCudaBufferPtr addr = alloc(type.memorySpace, size, PX_ALLOC_INFO_PARAMS_INPUT());

	shdfnd::Mutex::ScopedLock lock(mMutex);
	CudaBuffer* buffer = NULL;
	if (addr)
	{
		buffer = mBufferPool.construct(type);
		if (buffer)
		{
			buffer->init(addr, size, *this, PX_ALLOC_INFO_PARAMS_INPUT());
		}
	}
	return buffer;
}

PxCudaBufferPtr CudaMemMgr::alloc(PxCudaBufferMemorySpace::Enum memorySpace, size_t size, PX_ALLOC_INFO_PARAMS_DEF())
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

	if (!mInitialized)
	{
		if (!initialize())
		{
			return 0;
		}
	}

	if (mDebugDisableAllocs)
	{
		return 0;
	}

	return reinterpret_cast<PxCudaBufferPtr>(mHeap[memorySpace]->alloc(size, PX_ALLOC_INFO_PARAMS_INPUT()));
}

bool CudaMemMgr::free(PxCudaBufferMemorySpace::Enum memorySpace, PxCudaBufferPtr addr)
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

	if (!mInitialized)
	{
		return false;
	}

	if (addr)
	{
		return mHeap[memorySpace]->free((void*)(addr));
	}
	else
	{
		return false;
	}
}

bool CudaMemMgr::realloc(PxCudaBufferMemorySpace::Enum memorySpace, PxCudaBufferPtr addr, size_t size, PX_ALLOC_INFO_PARAMS_DEF())
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

	if (!mInitialized)
	{
		return false;
	}

	if (!size)
	{
		return false;
	}

	if (mDebugDisableAllocs)
	{
		return NULL;
	}

	bool ret = false;
	if (addr)
	{
		ret = mHeap[memorySpace]->realloc((void*)(addr), size, PX_ALLOC_INFO_PARAMS_INPUT());
	}

	return ret;
}

void CudaMemMgr::getStats(const PxCudaBufferType& type, PxCudaMemoryManagerStats& outStats)
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

	if (!mInitialized)
	{
		return;
	}

	ApexHeapStats hpStats;
	mHeap[type.memorySpace]->getStats(hpStats, HeapStatsFlags::F_BASIC_STATS | HeapStatsFlags::F_ALLOC_ID_STATS);

	outStats.heapSize = hpStats.heapSize;
	outStats.totalAllocated = hpStats.totalAllocated;
	outStats.maxAllocated = hpStats.maxAllocated;
	PxMemCopy(outStats.allocIdStats, hpStats.allocIdStats, sizeof(PxAllocIdStats)*PxAllocId::NUM_IDS);
}


bool CudaMemMgr::free(CudaBuffer& buffer)
{
	PxCudaBufferMemorySpace::Enum memSpace = buffer.getTypeFast().memorySpace;
	PxCudaBufferPtr addr = buffer.getPtrFast();

	{
		shdfnd::Mutex::ScopedLock lock(mMutex);
		mBufferPool.destroy(&buffer);
	}

	return free(memSpace, addr);
}


bool CudaMemMgr::realloc(CudaBuffer& buffer, size_t size, PX_ALLOC_INFO_PARAMS_DEF())
{
	return realloc(buffer.getTypeFast().memorySpace, buffer.getPtrFast(), size, PX_ALLOC_INFO_PARAMS_INPUT());
}

PxCudaBufferPtr CudaMemMgr::getMappedPinnedPtr(PxCudaBufferPtr hostPtr)
{
	shdfnd::Mutex::ScopedLock lock(mMutex);
	void* base = mHeap[PxCudaBufferMemorySpace::T_PINNED_HOST]->findBaseAddress((void*)hostPtr);
	if (base)
	{
		size_t offset = ((PinnedMemAllocator*)mMemoryAllocator[PxCudaBufferMemorySpace::T_PINNED_HOST])->getMappedPinnedOffset(base);
		return hostPtr + offset;
	}
	return 0;
}

bool CudaBuffer::free()
{
	return mMemManager->free(*this);
}


bool CudaBuffer::realloc(size_t size, PX_ALLOC_INFO_PARAMS_DEF())
{
	return mMemManager->realloc(*this, size, PX_ALLOC_INFO_PARAMS_INPUT());
}


DeviceMemAllocator::DeviceMemAllocator(PxCudaContextManager& mgr, size_t maxSize)
	: mMgr(mgr)
	, mMaxSize(maxSize)
	, mAllocSize(0)
{}


DeviceMemAllocator::~DeviceMemAllocator()
{
	PX_ASSERT(mAllocSize == 0);
}


void* DeviceMemAllocator::alloc(const size_t size)
{
	if (mAllocSize + size > mMaxSize)
	{
		return NULL;
	}
	else
	{
		PxScopedCudaLock lock(mMgr);
		CUdeviceptr dPtr;
		CUresult result = cuMemAlloc(&dPtr,  uint32_t(size));

		if (result == CUDA_SUCCESS)
		{
			mAllocSize += size;
			return (void*)(size_t)(dPtr);
		}
		else
		{
			PX_ASSERT_WITH_MESSAGE(0, "Failed to allocate device memory.");
			return NULL;
		}
	}
}


void DeviceMemAllocator::free(void* addr, const size_t size)
{
	PxScopedCudaLock lock(mMgr);
	PX_ASSERT(mAllocSize >= size);
	CUresult result = cuMemFree((CUdeviceptr)(size_t)(addr));
	PX_UNUSED(result);
	PX_ASSERT(result == CUDA_SUCCESS);
	mAllocSize -= size;
}


PinnedMemAllocator::PinnedMemAllocator(PxCudaContextManager& mgr, size_t maxSize)
	: mMgr(mgr)
	, mMaxSize(maxSize)
	, mAllocSize(0)
{
}


PinnedMemAllocator::~PinnedMemAllocator()
{
	PX_ASSERT(mAllocSize == 0);
}


void* PinnedMemAllocator::alloc(const size_t size)
{
	if (mAllocSize + size > mMaxSize)
	{
		return NULL;
	}
	else
	{
		PxScopedCudaLock lock(mMgr);
		void* hPtr;
		unsigned int flags = 0;

		if (mMgr.canMapHostMemory())
		{
			flags |= CU_MEMHOSTALLOC_DEVICEMAP;
		}
		CUresult result = cuMemHostAlloc(&hPtr, uint32_t(size), flags);

		if (result == CUDA_SUCCESS)
		{
			if (hPtr)
			{
				mAllocSize += size;
			}

			if (mMgr.canMapHostMemory())
			{
				CUdeviceptr dptr = 0;
				cuMemHostGetDevicePointer(&dptr, hPtr, 0);
				mMappedPinnedPtrs.insert(hPtr, size_t(dptr));
			}

			return hPtr;
		}
		else
		{
			PX_ASSERT_WITH_MESSAGE(0, "Failed to allocate pinned memory.");			
			return NULL;
		}
	}
}


void PinnedMemAllocator::free(void* addr, const size_t size)
{
	PxScopedCudaLock lock(mMgr);
	PX_ASSERT(mAllocSize >= size);

	if (mMgr.canMapHostMemory())
	{
		PX_ASSERT(mMappedPinnedPtrs.find(addr));
		mMappedPinnedPtrs.erase(addr);
	}
	
	CUresult result = cuMemFreeHost(addr);
	PX_UNUSED(result);
	PX_ASSERT(result == CUDA_SUCCESS);
	mAllocSize -= size;
}


WriteCombinedMemAllocator::WriteCombinedMemAllocator(PxCudaContextManager& mgr, size_t maxSize)
	: mMgr(mgr)
	, mMaxSize(maxSize)
	, mAllocSize(0)
{
	mWcMemSupport = mMgr.getDriverVersion() >= 2020 ? WcMem::SUPPORTED : WcMem::NOT_SUPPORTED;
}


WriteCombinedMemAllocator::~WriteCombinedMemAllocator()
{
	PX_ASSERT(mAllocSize == 0);
}


bool WriteCombinedMemAllocator::isWcMemSupported()
{
	if (mWcMemSupport == WcMem::SUPPORTED)
	{
		return true;
	}
	else
	{
		PX_ASSERT(mWcMemSupport == WcMem::NOT_SUPPORTED);
		return false;
	}
}


void* WriteCombinedMemAllocator::alloc(const size_t size)
{
	if (mAllocSize + size > mMaxSize)
	{
		return NULL;
	}
	else
	{
		PxScopedCudaLock lock(mMgr);
		void* hPtr = NULL;

		unsigned int flags = CU_MEMHOSTALLOC_WRITECOMBINED;

		if (mMgr.canMapHostMemory())
		{
			flags |= CU_MEMHOSTALLOC_DEVICEMAP;
		}

		bool success = isWcMemSupported() && (cuMemHostAlloc(&hPtr, size, flags) == CUDA_SUCCESS);
		if (success)
		{
			if (hPtr)
			{
				mAllocSize += size;
			}

			return hPtr;
		}
		else
		{
			PX_ASSERT_WITH_MESSAGE(0, "Failed to allocate write combined memory.");			
			return NULL;
		}
	}
}


void WriteCombinedMemAllocator::free(void* addr, const size_t size)
{
	PxScopedCudaLock lock(mMgr);
	PX_ASSERT(mAllocSize >= size);
	CUresult result = cuMemFreeHost(addr);
	PX_ASSERT(result == CUDA_SUCCESS);
	PX_UNUSED(result);
	mAllocSize -= size;
}


HostMemAllocator::HostMemAllocator(size_t maxSize)
	: mMaxSize(maxSize)
	, mAllocSize(0)
{
}


HostMemAllocator::~HostMemAllocator()
{
	PX_ASSERT(mAllocSize == 0);
}


void* HostMemAllocator::alloc(const size_t size)
{
	if (mAllocSize + size > mMaxSize)
	{
		return NULL;
	}
	else
	{
		void* ret = PX_ALLOC(size, "host memory");
		if (ret)
		{
			mAllocSize += size;
			return ret;
		}
		else
		{
			PX_ASSERT_WITH_MESSAGE(0, "Failed to allocate host memory.");			
			return NULL;
		}
	}
}


void HostMemAllocator::free(void* addr, const size_t size)
{
	PX_ASSERT(mAllocSize >= size);
	PX_FREE(addr);
	mAllocSize -= size;
}

