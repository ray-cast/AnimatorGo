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

//----------------------------------------------------------------------------//
// HeapManagerRef.h
//----------------------------------------------------------------------------//

#ifndef PXCUDACONTEXTMANAGER_HEAPMANAGERREF_H
#define PXCUDACONTEXTMANAGER_HEAPMANAGERREF_H

#include "task/PxTaskDefine.h"

#include "HeapManagerInterface.h"
#include "HeapManagerLinkedList.h"
#include "PsPool.h"
#include "PsMutex.h"
#include "PsArray.h"
#include "PsUserAllocated.h"

#define DUMP_HEAP_USAGE_TO_FILE 0

#ifdef _DEBUG
#define KEEP_DEBUG_INFO 1
#else
#define KEEP_DEBUG_INFO 0
#endif

#if DUMP_HEAP_USAGE_TO_FILE
#include "stdio.h"
#endif

namespace physx
{

class Heap;
struct Buddy
{
	PX_INLINE Buddy()
		: addr(0)
		, next(0)
		, occupiedSize(0)
		, allocId(PxAllocId::UNASSIGNED)
		, isLastBuddy(true)
		, level(0)
#if KEEP_DEBUG_INFO
		, file(NULL)
		, allocName(NULL)
		, line(0)
#endif
	{}

	PX_INLINE Buddy(Buddy& b)
		: addr(b.addr)
		, next(b.next)
		, occupiedSize(b.occupiedSize)
		, allocId(b.allocId)
		, isLastBuddy(b.isLastBuddy)
		, level(b.level)
#if KEEP_DEBUG_INFO
		, file(b.file)
		, allocName(b.allocName)
		, line(b.line)
#endif
	{}

	size_t		addr;
	Buddy*		next;
	size_t		occupiedSize;
	uint16_t	allocId;
	uint8_t 	isLastBuddy;
	uint8_t		level;
#if KEEP_DEBUG_INFO
	const char* file;
	const char* allocName;
	uint32_t	line;
#endif
};

struct BuddyList: public LinkedList<Buddy>, public shdfnd::UserAllocated
{
	BuddyList()
		: buddySize(0)
		, heap(NULL)
	{}
	void clear();

	size_t		buddySize;  // = 2^level
	Heap*		heap;
};

struct HeapManagerPage
{
	PX_INLINE bool operator < (const HeapManagerPage& p) const
	{
		return baseAddr < p.baseAddr;
	}

	PX_INLINE bool operator > (const HeapManagerPage& p) const
	{
		return baseAddr > p.baseAddr;
	}

	size_t	baseAddr;
	Heap*	heap;
	bool	isPersistent;
};


class HeapManagerRef: public HeapManagerInterface, public shdfnd::UserAllocated
{
	PX_NOCOPY(HeapManagerRef)
public:
	HeapManagerRef(physx::PxErrorCallback& errorCallback, bool enableMutex = true);
	virtual ~HeapManagerRef();

	// INTERFACE METHODS
	virtual		bool		init(Allocator* memAllocator, const size_t baseSize, const size_t pageSize, const size_t minBlockSize, const size_t maxIntFrag);
	virtual		bool		setPageSize(size_t pageSize);
	virtual		void*		alloc(const size_t size, PX_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED));
	virtual		bool		realloc(void* addr, const size_t size, PX_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED));
	virtual		bool		free(void* addr);
	virtual		void		freeEmptyPages();
	virtual		bool		reserve(size_t size);
	virtual		bool		getStats(ApexHeapStats& stats, const uint32_t flags);
	virtual		bool		visualizeMemory(uint8_t* array, const size_t size);
	virtual		void*		findBaseAddress(void* addr);

	// INTERNALS
	// searches 2^pow >= val,  searches pow in [min, max]
	static PX_INLINE size_t	findNextPow2(const size_t val, const uint8_t min = 0, const uint8_t max = BITSPERWORD);
	static PX_INLINE size_t	findNextPow2(uint8_t& pow, const size_t val, const uint8_t min = 0, const uint8_t max = BITSPERWORD);

	PX_INLINE	void		addToStats(PxAllocId::Enum id, const size_t size, const size_t fragmentation);
	PX_INLINE	void		removeFromStats(PxAllocId::Enum id, const size_t size, const size_t fragmentation);
	PX_INLINE	void		incStats(PxAllocId::Enum id, const size_t change, const size_t fragmentation);
	PX_INLINE	void		decStats(PxAllocId::Enum id, const size_t change, const size_t fragmentation);

	PX_INLINE	void					notifyEmptyPage()
	{
		mNewEmptyPage = true;
	}
	PX_INLINE	shdfnd::Pool<Buddy>&	getBuddyPool()
	{
		return mBuddyPool;
	}

private:
	Heap*	allocateNewHeap(size_t heapSize, bool isPersistent = false);
	Heap*	allocateNewPages(size_t requestedSize);
	void	resetHeap(HeapManagerPage& page);
	void	removeDeletedHeapsFromList(uint32_t numDeletes);
	void	shrinkMemory();

	Heap*	findHeap(void* addr) const;

private:
	// heaps
	shdfnd::Array<HeapManagerPage> mHeaps;
	shdfnd::Pool<Buddy>		    mBuddyPool;
	size_t						mPageSize;
	size_t						mMinBlockSize;
	size_t						mMaxIntFrag;
	bool						mNewEmptyPage;
	// lock
	shdfnd::Mutex				mMutex;
	// page allocator
	Allocator*					mMemAllocator;
	// overall stats
	size_t						mGlobalAllocMem;
	size_t						mGlobalMaxAllocMem;
	size_t						mGlobalInternalFragmentation;
	size_t						mGlobalMaxInternalFragmentation;
	// stats per allocation ID
	PxAllocIdStats				mGlobalAllocIdStats[PxAllocId::NUM_IDS];
	// error callback
	physx::PxErrorCallback&	mErrorCallback;

#if DUMP_HEAP_USAGE_TO_FILE
	FILE*				mLogFile;
	unsigned __int64    m_qpc;
	unsigned __int64    m_qpf;
#endif
};


class Heap : public shdfnd::UserAllocated
{
public:
	PX_INLINE Heap(HeapManagerRef& manager, physx::PxErrorCallback& errorCallback)
		: mManager(manager)
		, mErrorCallback(errorCallback)
		, mBaseAddr(0)
		, mMinBlockSize(0)
		, mFreeBuddiesAtLevel(NULL)
		, mMaxIntFrag(0)
		, mTotalSize(0)
		, mMaxLevel(0)
		, mMinBlockLog2(0)
		, mAllocMem(0)
		, mMaxAllocMem(0)
		, mInternalFragmentation(0)
		, mMaxInternalFragmentation(0)
	{}

	PX_INLINE ~Heap();

	bool	init(size_t baseAddr, const size_t baseSize, const size_t minBlockSize, const size_t maxIntFrag);
	void*	alloc(const size_t size, PX_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED));
	bool	realloc(void* addr, const size_t size, PX_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED));
	bool	free(void* addr);
	bool	getStats(ApexHeapStats& stats, const uint32_t flags);
	bool	visualizeMemory(uint8_t* array, const size_t size);

	PX_INLINE	size_t					getTotalMemorySize()
	{
		return mTotalSize;
	}
	PX_INLINE	size_t					getAllocatedMemorySize()
	{
		return mAllocMem;
	}
	PX_INLINE	shdfnd::Pool<Buddy>&	getBuddyPool()
	{
		return mManager.getBuddyPool();
	}
	PX_INLINE	void*					getBaseAddress() 
	{ 
		return (void*)mBaseAddr; 
	}

private:
	// split buddy b with size 2^level into two buddies with level 2^(level-1) and append those to the free list. deletes b, assumes that b was removed from the list before.
	bool	splitBuddy(Buddy* b);
	// merge 2 buddies to next bigger one. deletes b0 and b1, assumes that they are already removed from their array.
	bool	mergeBuddies(Buddy* b0, Buddy* b1);

	// split of right, free children of a buddy if the internal fragmentation of a buddy is bigger than a threshold
	// returns the size of all allocated buddies
	size_t	reduceIntFragment(Buddy& b, size_t threshold);

	// find a Buddy by splitting a Buddy at searchLevel
	Buddy*	findBySplitting(uint8_t searchLevel);
	Buddy*	findByMerging(uint8_t searchLevel);
	size_t	findPairAndMerge(BuddyList& list, size_t numToFind);

	bool	sanityTest();

	void operator=(const Heap&)
	{
		PX_ASSERT(0);
	}

private:
	HeapManagerRef&			mManager;
	physx::PxErrorCallback& mErrorCallback;
	size_t					mBaseAddr;
	size_t					mMinBlockSize;
	BuddyList*				mFreeBuddiesAtLevel;
	BuddyList				mAllocatedBuddies;
	size_t					mMaxIntFrag;
	size_t					mTotalSize;
	uint8_t					mMaxLevel;		// 2^maxLevel <= memorySize
	uint8_t					mMinBlockLog2;

	size_t					mAllocMem;		// fragmented
	size_t					mMaxAllocMem;
	size_t					mInternalFragmentation;
	size_t					mMaxInternalFragmentation;
};

} // end physx namespace

#endif // PXCUDACONTEXTMANAGER_HEAPMANAGERREF_H
