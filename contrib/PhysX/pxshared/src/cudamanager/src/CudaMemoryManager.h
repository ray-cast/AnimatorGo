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

#ifndef PXCUDACONTEXTMANAGER_CUDAMEMORYMANAGER_H
#define PXCUDACONTEXTMANAGER_CUDAMEMORYMANAGER_H

#include "task/PxTaskDefine.h"
#include "HeapManagerInterface.h"

#include "PsPool.h"
#include "PsMutex.h"
#include "PsUserAllocated.h"
#include "PsHashMap.h"

namespace physx
{

class CudaBuffer;
class HeapManagerInterface;
class CudaMemMgr;
class PxCudaContextManager;

class CudaBuffer: public PxCudaBuffer
{
public:
	PX_INLINE CudaBuffer(const PxCudaBufferType& type)
		: mType(type)
	{}

// Ni Interface
	bool									free();
	bool									realloc(size_t size, PX_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED));
	const PxCudaBufferType&					getType() const
	{
		return getTypeFast();
	}
	PxCudaBufferPtr							getPtr() const
	{
		return getPtrFast();
	}
	size_t									getSize() const
	{
		return getSizeFast();
	}
	PxCudaMemoryManager*					getCudaMemoryManager() const
	{
		return getCudaMemoryManagerFast();
	}
//
	PX_INLINE	const PxCudaBufferType&		getTypeFast() const
	{
		return mType;
	}
	PX_INLINE	PxCudaBufferPtr				getPtrFast() const
	{
		return mPtr;
	}
	PX_INLINE	void						setPtr(PxCudaBufferPtr val)
	{
		mPtr = val;
	}
	PX_INLINE	size_t						getSizeFast() const
	{
		return mSize;
	}
	PX_INLINE	void						setSize(size_t val)
	{
		mSize = val;
	}
	PX_INLINE	PxCudaMemoryManager*			getCudaMemoryManagerFast() const
	{
		return reinterpret_cast<PxCudaMemoryManager*>(mMemManager);
	}
	PX_INLINE	void						init(PxCudaBufferPtr ptr, size_t size, CudaMemMgr& manager, PX_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED))
	{
		mPtr = ptr;
		mSize = size;
		mMemManager = &manager;
		mAllocInfo = PxAllocInfo(PX_ALLOC_INFO_PARAMS_INPUT());
	}

	void operator=(const CudaBuffer& in)
	{
		const_cast<PxCudaBufferType&>(mType) = in.mType;
		mPtr = in.mPtr;
		mSize = in.mSize;
		mMemManager = in.mMemManager;
	}

private:
	const PxCudaBufferType	mType;
	PxCudaBufferPtr			mPtr;
	size_t					mSize;
	CudaMemMgr*				mMemManager;
	PxAllocInfo				mAllocInfo;
};


class CudaMemMgr: public PxCudaMemoryManager, public shdfnd::UserAllocated
{
	PX_NOCOPY(CudaMemMgr)
public:
	CudaMemMgr(PxCudaContextManager& mMgr, physx::PxErrorCallback& errorCallback);
	virtual ~CudaMemMgr();

	PxCudaBuffer*				alloc(const PxCudaBufferType& type, size_t size, PX_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED));
	PxCudaBufferPtr				alloc(PxCudaBufferMemorySpace::Enum memorySpace, size_t size, PX_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED));
	bool						free(PxCudaBufferMemorySpace::Enum memorySpace, PxCudaBufferPtr addr);
	bool						realloc(PxCudaBufferMemorySpace::Enum memorySpace, PxCudaBufferPtr addr, size_t size, PX_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED));
	void						getStats(const PxCudaBufferType& type, PxCudaMemoryManagerStats& outStats);
	bool						reserve(const PxCudaBufferType& type, size_t size);
	bool						setPageSize(const PxCudaBufferType& type, size_t size);
	bool						setMaxMemorySize(const PxCudaBufferType& type, size_t size);
	size_t						getBaseSize(const PxCudaBufferType& type);
	size_t						getPageSize(const PxCudaBufferType& type);
	size_t						getMaxMemorySize(const PxCudaBufferType& type);
	void						debugDisableAllocs()
	{
		mDebugDisableAllocs = true;
	}
	PxCudaBufferPtr				getMappedPinnedPtr(PxCudaBufferPtr hostPtr);

	// internals
	bool						free(CudaBuffer& buffer);
	bool						realloc(CudaBuffer& buffer, size_t size, PX_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED));
	bool						setBaseSize(const PxCudaBufferType& type, size_t size);

private:
	PX_INLINE	bool						initialize();
	physx::PxErrorCallback&				mErrorCallback;
	HeapManagerInterface*					mHeap[PxCudaBufferMemorySpace::COUNT];
	HeapManagerInterface::Allocator*		mMemoryAllocator[PxCudaBufferMemorySpace::COUNT];
	size_t									mMemoryBaseSize[PxCudaBufferMemorySpace::COUNT];
	size_t									mMemoryPageSize[PxCudaBufferMemorySpace::COUNT];
	size_t									mMemoryMaxSize[PxCudaBufferMemorySpace::COUNT];
	shdfnd::Pool<CudaBuffer>				mBufferPool;
	bool									mInitialized;
	PxCudaContextManager&					mMgr;
	shdfnd::Mutex							mMutex;
	bool									mDebugDisableAllocs;	
};

// TODO, give MemoryAllocator prefix or namespace
class DeviceMemAllocator: public HeapManagerInterface::Allocator, public shdfnd::UserAllocated
{
	PX_NOCOPY(DeviceMemAllocator)
public:
	DeviceMemAllocator(PxCudaContextManager& mgr, size_t maxSize);
	virtual ~DeviceMemAllocator();

	virtual void*	alloc(const size_t size);
	virtual void	free(void* addr, const size_t size);

	void	setMaxSize(size_t maxSize)
	{
		mMaxSize = maxSize;
	}
	size_t	getMaxSize()
	{
		return mMaxSize;
	}

private:
	PxCudaContextManager& mMgr;
	size_t			mMaxSize;
	size_t			mAllocSize;
};


class PinnedMemAllocator: public HeapManagerInterface::Allocator, public shdfnd::UserAllocated
{
	PX_NOCOPY(PinnedMemAllocator)
public:
	PinnedMemAllocator(PxCudaContextManager& mMgr, size_t maxSize);
	virtual ~PinnedMemAllocator();

	virtual void*	alloc(const size_t size);
	virtual void	free(void* addr, const size_t size);

	void	setMaxSize(size_t maxSize)
	{
		mMaxSize = maxSize;
	}
	size_t	getMaxSize()
	{
		return mMaxSize;
	}

	size_t getMappedPinnedOffset(void* base)
	{

		PX_ASSERT(base);
		const shdfnd::HashMap<void*, size_t>::Entry* entry = mMappedPinnedPtrs.find(base);
		PX_ASSERT(entry);
		return entry->second - size_t(base);
	}

private:
	PxCudaContextManager& mMgr;
	size_t			mMaxSize;
	size_t			mAllocSize;
	shdfnd::HashMap<void*, size_t> mMappedPinnedPtrs;
};


class HostMemAllocator: public HeapManagerInterface::Allocator, public shdfnd::UserAllocated
{
	PX_NOCOPY(HostMemAllocator)
public:
	HostMemAllocator(size_t maxSize);
	virtual ~HostMemAllocator();

	virtual void*	alloc(const size_t size);
	virtual void	free(void* addr, const size_t size);

	void	setMaxSize(size_t maxSize)
	{
		mMaxSize = maxSize;
	}
	size_t	getMaxSize()
	{
		return mMaxSize;
	}

private:
	size_t			mMaxSize;
	size_t			mAllocSize;
};


class WriteCombinedMemAllocator: public HeapManagerInterface::Allocator, public shdfnd::UserAllocated
{
	PX_NOCOPY(WriteCombinedMemAllocator)
public:
	WriteCombinedMemAllocator(PxCudaContextManager& mgr, size_t maxSize);
	virtual ~WriteCombinedMemAllocator();

	virtual void*	alloc(const size_t size);
	virtual void	free(void* addr, const size_t size);

	void	setMaxSize(size_t maxSize)
	{
		mMaxSize = maxSize;
	}
	size_t	getMaxSize()
	{
		return mMaxSize;
	}

private:
	struct WcMem
	{
		enum Enum
		{
			NOT_CHECKED,
			SUPPORTED,
			NOT_SUPPORTED
		};
	};

	bool	isWcMemSupported();

private:
	int					mCudaOrdinal;
	PxCudaContextManager& mMgr;
	WcMem::Enum			mWcMemSupport;
	size_t				mMaxSize;
	size_t				mAllocSize;
};

} // end physx namespace

#endif // PXCUDACONTEXTMANAGER_CUDAMEMORYMANAGER_H
