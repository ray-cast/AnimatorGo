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

#include "CudaMemoryManager.h"
#include "HeapManagerRef.h"

#include "PsSort.h"
#include "PsArray.h"
#include "PsAllocator.h"
#include "PsString.h"

#include "foundation/PxMath.h"
#include "foundation/PxErrorCallback.h"
#include "foundation/PxMemory.h"


#if DUMP_HEAP_USAGE_TO_FILE
#include "PsWindowsInclude.h"
#endif

using namespace physx::shdfnd;
using namespace physx;


#define CMM_DELETE_SINGLE(x)	{ if(x) delete x; x = NULL; }
#define CMM_DELETE_ARRAY(x)		{ if(x) delete [] x; x = NULL; }

HeapManagerRef::HeapManagerRef(physx::PxErrorCallback& errorCallback, bool enableMutex)
	: mHeaps(PX_DEBUG_EXP("HeapManagerRef:mHeaps"))
	, mBuddyPool("mBuddyPool", 1024)
	, mPageSize(0)
	, mMinBlockSize(0)
	, mMaxIntFrag(size_t(-1))
	, mNewEmptyPage(false)
	, mMemAllocator(NULL)
	, mGlobalAllocMem(0)
	, mGlobalMaxAllocMem(0)
	, mGlobalInternalFragmentation(0)
	, mGlobalMaxInternalFragmentation(0)
	, mErrorCallback(errorCallback)

{
	PX_UNUSED(enableMutex);  // SJB: heap alloc of shdfnd::Mutex not working for me
}

HeapManagerRef::~HeapManagerRef()
{
	for (uint32_t i = 0; i < mHeaps.size(); i++)
	{
		if (mMemAllocator && mHeaps[i].baseAddr)
		{
			mMemAllocator->free(reinterpret_cast<void*>(mHeaps[i].baseAddr), mHeaps[i].heap->getTotalMemorySize());
		}
		CMM_DELETE_SINGLE(mHeaps[i].heap);
	}
#if DUMP_HEAP_USAGE_TO_FILE
	fclose(mLogFile);
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE METHODS
bool HeapManagerRef::init(Allocator* memAllocator, const size_t baseSize, const size_t pageSize, const size_t minBlockSize, const size_t maxIntFrag)
{
	shdfnd::Mutex::ScopedLock lock(mMutex);
	// init Heap and do some basic checks.

	// init only once
	if (mHeaps.size())
	{
		return false;
	}

	if (baseSize && (minBlockSize > baseSize))
	{
		return false;
	}

	if (minBlockSize > pageSize)
	{
		return false;
	}

	if (baseSize && (baseSize % minBlockSize))
	{
		return false;
	}

	uint8_t minBlockSizeLog2;
	if (minBlockSize != findNextPow2(minBlockSizeLog2, minBlockSize, 0, BITSPERWORD))
	{
		return false;
	}

	if (pageSize != findNextPow2(pageSize, minBlockSizeLog2, BITSPERWORD))
	{
		return false;
	}

	if (!memAllocator)
	{
		return false;
	}

	mMemAllocator = memAllocator;
	mPageSize = pageSize;
	mMinBlockSize = minBlockSize;
	mMaxIntFrag = maxIntFrag;

	memset(&mGlobalAllocIdStats, 0, sizeof(PxAllocIdStats)*PxAllocId::NUM_IDS);

#if DUMP_HEAP_USAGE_TO_FILE
	char fileName[1024];
	sprintf_s(fileName, 1024, "HeapLog_%p.txt", this);
	fopen_s(&mLogFile, fileName, "w");
	fprintf(mLogFile, "HeapSize: %d, BlockSize: %d Addr: 0x0\n", baseSize, minBlockSize);
	QueryPerformanceCounter((LARGE_INTEGER*)&m_qpc);
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_qpf);
#endif

	// init heap
	if (baseSize)
	{
		return allocateNewHeap(baseSize, true) != NULL;
	}
	else
	{
		return true;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool HeapManagerRef::setPageSize(size_t pageSize)
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

	if (pageSize == 0)
	{
		return false;
	}

	if (mMinBlockSize > pageSize)
	{
		return false;
	}

	if (pageSize != findNextPow2(pageSize, 0, BITSPERWORD))
	{
		return false;
	}

	mPageSize = pageSize;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* HeapManagerRef::alloc(const size_t size, PX_ALLOC_INFO_PARAMS_DEF())
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

#if DUMP_HEAP_USAGE_TO_FILE
	unsigned __int64 qpc;
	QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
	float dtime = (float)((double)(qpc - m_qpc) / (double)m_qpf);
	fprintf(mLogFile, "alloc t: %f s: %d", dtime, size);
#endif

	void* ret = NULL;
	// try to allocate it in one of the heaps/pages
	for (uint32_t i = 0; !ret && i < mHeaps.size(); i++)
	{
		ret = mHeaps[i].heap->alloc(size, PX_ALLOC_INFO_PARAMS_INPUT());
	}

	// create a new page
	if (!ret)
	{
		Heap* heap = allocateNewPages(size);
		if (heap)
		{
			ret = heap->alloc(size, PX_ALLOC_INFO_PARAMS_INPUT());
		}
	}

#if DUMP_HEAP_USAGE_TO_FILE
	fprintf(mLogFile, " a: 0x%p\n", ret);
#endif

	return ret;
}

//(10/20/2009 feodorb) TODO: decide whether we move the binary search
//somewhere away from here. Stands here for std::lower_bound replacement
template<typename T>
static uint32_t findUpperBound(const physx::shdfnd::Array<T>& refArray, const T& refValue)
{
	uint32_t start = 0, end = refArray.size();
	while (end - start > 0)
	{
		uint32_t midPoint = start + ((end - start) >> 1);

		if (!(refValue < refArray[midPoint]))
		{
			start = midPoint + 1;
		}
		else
		{
			end = midPoint;
		}
	}
	return start;
}

Heap* HeapManagerRef::findHeap(void* addr) const
{
	HeapManagerPage searchPage;
	searchPage.baseAddr = reinterpret_cast<size_t>(addr);

	uint32_t upperBound = findUpperBound(mHeaps, searchPage);
	PX_ASSERT(upperBound == 0 ||
	          (searchPage.baseAddr >= mHeaps[upperBound - 1].baseAddr &&
	           searchPage.baseAddr < mHeaps[upperBound - 1].baseAddr + mHeaps[upperBound - 1].heap->getTotalMemorySize())
	         );

	return (upperBound > 0) ? mHeaps[upperBound - 1].heap : 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool HeapManagerRef::realloc(void* addr, const size_t size, PX_ALLOC_INFO_PARAMS_DEF())
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

#if DUMP_HEAP_USAGE_TO_FILE
	unsigned __int64 qpc;
	QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
	float dtime = (float)((double)(qpc - m_qpc) / (double)m_qpf);
	fprintf(mLogFile, "realloc t: %f s: %d, a: 0x%p\n", dtime, size, addr);
#endif

	Heap* heap = findHeap(addr);

	if (heap != 0)
	{
		bool ret = heap->realloc(addr, size, PX_ALLOC_INFO_PARAMS_INPUT());
		if (ret && size > 0 && mNewEmptyPage)
		{
			shrinkMemory();
		}
		return ret;
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool HeapManagerRef::free(void* addr)
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

#if DUMP_HEAP_USAGE_TO_FILE
	unsigned __int64 qpc;
	QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
	float dtime = (float)((double)(qpc - m_qpc) / (double)m_qpf);
	fprintf(mLogFile, "free t: %f a: 0x%p\n", dtime, addr);
#endif

	if (addr == NULL)
	{
		return false;
	}

	Heap* heap = findHeap(addr);
	if (heap != 0)
	{
		bool ret = heap->free(addr);
		if (ret && mNewEmptyPage)
		{
			shrinkMemory();
		}
		return ret;
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void addStats(ApexHeapStats& dst, const ApexHeapStats& src, uint32_t flags)
{
	if (flags & HeapStatsFlags::F_BASIC_STATS)
	{
		dst.heapSize += src.heapSize;
	}
	if (flags & HeapStatsFlags::F_BIGGEST_FREE_BLOCK)
	{
		dst.biggestFreeBlock = PxMax(dst.biggestFreeBlock, src.biggestFreeBlock);
	}
	if (flags & HeapStatsFlags::F_HISTOGRAM)
	{
		dst.numEntries = PxMax(dst.numEntries, src.numEntries);
		for (uint32_t i = 0; i < BITSPERWORD; i++)
		{
			dst.freeBuddyHistogram[i] += src.freeBuddyHistogram[i];
			dst.allocatedBuddyHistogram[i] += src.allocatedBuddyHistogram[i];
		}
	}
}

PX_INLINE void HeapManagerRef::addToStats(PxAllocId::Enum id, const size_t size, const size_t fragmentation)
{
	PxAllocIdStats& idStats = mGlobalAllocIdStats[id];
	idStats.elements++;
	idStats.size += size;
	idStats.maxElements = PxMax(idStats.maxElements, idStats.elements);
	idStats.maxSize = PxMax(idStats.maxSize, idStats.size);
	mGlobalAllocMem += size;
	mGlobalMaxAllocMem = PxMax(mGlobalMaxAllocMem, mGlobalAllocMem);
	mGlobalInternalFragmentation += fragmentation;
	mGlobalMaxInternalFragmentation = PxMax(mGlobalMaxInternalFragmentation, mGlobalInternalFragmentation);
}


PX_INLINE void HeapManagerRef::removeFromStats(PxAllocId::Enum id, const size_t size, const size_t fragmentation)
{
	PxAllocIdStats& idStats = mGlobalAllocIdStats[id];
	PX_ASSERT(idStats.elements);
	PX_ASSERT(idStats.size >= size);

	idStats.elements--;
	idStats.size -= size;
	mGlobalAllocMem -= size;
	mGlobalInternalFragmentation -= fragmentation;
}

PX_INLINE void HeapManagerRef::incStats(PxAllocId::Enum id, const size_t change, const size_t fragmentation)
{
	PxAllocIdStats& idStats = mGlobalAllocIdStats[id];
	idStats.size += change;
	idStats.maxSize = PxMax(idStats.maxSize, idStats.size);
	mGlobalAllocMem += change;
	mGlobalMaxAllocMem = PxMax(mGlobalMaxAllocMem, mGlobalAllocMem);
	mGlobalInternalFragmentation += fragmentation;
	mGlobalMaxInternalFragmentation = PxMax(mGlobalMaxInternalFragmentation, mGlobalInternalFragmentation);
}

PX_INLINE void HeapManagerRef::decStats(PxAllocId::Enum id, const size_t change, const size_t fragmentation)
{
	PxAllocIdStats& idStats = mGlobalAllocIdStats[id];
	PX_ASSERT(idStats.size >= change);
	idStats.size -= change;
	mGlobalAllocMem += change;
	mGlobalInternalFragmentation += fragmentation;
}

bool HeapManagerRef::getStats(ApexHeapStats& stats, const uint32_t flags)
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

	ApexHeapStats tmpStats;
	stats.reset();
	for (uint32_t i = 0; i < mHeaps.size(); i++)
	{
		mHeaps[i].heap->getStats(tmpStats, flags);
		addStats(stats, tmpStats, flags);
	}
	if (flags & HeapStatsFlags::F_BASIC_STATS)
	{
		stats.totalAllocated = mGlobalAllocMem;
		stats.maxAllocated = mGlobalMaxAllocMem;

	}
	if (flags & HeapStatsFlags::F_INTERNAL_FRAGMENTATION)
	{
		stats.internalFragmentation = mGlobalInternalFragmentation;
		stats.maxInternalFragmentation = mGlobalMaxInternalFragmentation;
	}
	if (flags & HeapStatsFlags::F_ALLOC_ID_STATS)
	{
		// stats per allocation ID
		PxMemCopy(stats.allocIdStats, mGlobalAllocIdStats, sizeof(PxAllocIdStats)*PxAllocId::NUM_IDS);
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool HeapManagerRef::visualizeMemory(uint8_t* array, const size_t arraySize)
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

	ApexHeapStats tmpStats;
	getStats(tmpStats, HeapStatsFlags::F_BASIC_STATS);
	float scale = float(arraySize) / float(tmpStats.heapSize);
	uint8_t* start = array;
	for (uint32_t i = 0; i < mHeaps.size(); i++)
	{
		size_t heapSize = mHeaps[i].heap->getTotalMemorySize();
		size_t numVis = size_t(float(heapSize) * scale);
		PX_ASSERT(start + numVis <= array + arraySize);
		mHeaps[i].heap->visualizeMemory(start, numVis);
		start += numVis;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* HeapManagerRef::findBaseAddress(void* addr)
{
	Heap* heap = findHeap(addr);
	if (heap)
	{
		return heap->getBaseAddress();
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Heap* HeapManagerRef::allocateNewHeap(size_t heapSize, bool isPersistent)
{
	if (!mMemAllocator)
	{
		return NULL;
	}

	void* newPage = mMemAllocator->alloc(heapSize);
	if (newPage)
	{
		HeapManagerPage page;
		page.baseAddr = reinterpret_cast<size_t>(newPage);
		page.heap = PX_NEW(Heap)(*this, mErrorCallback);
		page.isPersistent = isPersistent;
		if (page.heap && page.heap->init(page.baseAddr, heapSize, mMinBlockSize, mMaxIntFrag))
		{
			mHeaps.pushBack(page);
			shdfnd::sort(mHeaps.begin(), (uint32_t) mHeaps.size());
			return page.heap;
		}
		else
		{
			mMemAllocator->free(newPage, page.heap->getTotalMemorySize());
			CMM_DELETE_SINGLE(page.heap);
		}
	}
	return NULL;
}

Heap* HeapManagerRef::allocateNewPages(size_t requestedSize)
{
	uint8_t pageSizeLog2;
	uint8_t minBlockSizeLog2;
	findNextPow2(minBlockSizeLog2, mMinBlockSize, 0, BITSPERWORD);
	findNextPow2(pageSizeLog2, mPageSize, minBlockSizeLog2, BITSPERWORD);
	const size_t allocSize = findNextPow2(requestedSize, pageSizeLog2, BITSPERWORD);
	return allocateNewHeap(allocSize);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HeapManagerRef::removeDeletedHeapsFromList(uint32_t numDeletes)
{
	// remove pages from list, keeping it sorted.
	if (numDeletes)
	{
		const uint32_t numEntries = (uint32_t) mHeaps.size();

		//seek
		uint32_t w = 0;
		while (w < (numEntries) && mHeaps[w].heap != NULL)
		{
			w++;
		}

		// remove holes
		uint32_t r = w + 1;
		while (r < numEntries)
		{
			if (mHeaps[r].heap == NULL)
			{
				r++;
			}
			else
			{
				mHeaps[w++] = mHeaps[r++];
			}
		}

		mHeaps.resize(numEntries - numDeletes);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HeapManagerRef::resetHeap(HeapManagerPage& page)
{
	PX_ASSERT(page.heap->getAllocatedMemorySize() == 0);
	Heap* newHeap = PX_NEW(Heap)(*this, mErrorCallback);
	if (newHeap)
	{
		if (newHeap->init(page.baseAddr, page.heap->getTotalMemorySize(), mMinBlockSize, mMaxIntFrag))
		{
			CMM_DELETE_SINGLE(page.heap);
			page.heap = newHeap;
		}
		else
		{
			CMM_DELETE_SINGLE(newHeap);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HeapManagerRef::freeEmptyPages()
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

	// release heaps
	uint32_t numDeletes = 0;
	const uint32_t numEntries = (uint32_t) mHeaps.size();
	for (uint32_t i = 0; i < numEntries; i++)
	{
		HeapManagerPage& page = mHeaps[i];
		PX_ASSERT(page.heap);
		if (page.isPersistent)
		{
			// for persistent pages: reset without release.
			if (page.heap->getAllocatedMemorySize() == 0)
			{
				resetHeap(page);
			}
		}
		else if (page.heap->getAllocatedMemorySize() == 0)
		{
			mMemAllocator->free(reinterpret_cast<void*>(page.baseAddr), page.heap->getTotalMemorySize());
			CMM_DELETE_SINGLE(page.heap);
			numDeletes++;
		}
	}

	if (numDeletes)
	{
		removeDeletedHeapsFromList(numDeletes);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HeapManagerRef::shrinkMemory()
{
	mNewEmptyPage = false;

	// collect stats
	size_t totalAllocated = 0;
	size_t totalPageSize = 0;
	const uint32_t numEntries = (uint32_t) mHeaps.size();
	for (uint32_t i = 0; i < numEntries; i++)
	{
		HeapManagerPage& page = mHeaps[i];
		totalAllocated += page.heap->getAllocatedMemorySize();
		totalPageSize += page.heap->getTotalMemorySize();
		PX_ASSERT(totalAllocated <= totalPageSize);
	}

	// shrink memory if free non-persistent space is half or more of the allocated pages.
	// releasing from the back of address sorted list, other strategies like LRU, best fit are also possible.
	if (totalPageSize)
	{
		float allocScale = float(totalAllocated) / float(totalPageSize);
		if (allocScale <= 0.5f)
		{
			size_t sizeToRelease = totalAllocated ? (totalPageSize - totalAllocated) >> 1 : totalPageSize;
			uint32_t numDeletes = 0;
			for (uint32_t i = 0; i < numEntries; i++)
			{
				HeapManagerPage& page = mHeaps[numEntries - i - 1];
				PX_ASSERT(page.heap);
				if (page.heap->getAllocatedMemorySize() == 0)
				{
					if (!page.isPersistent && page.heap->getTotalMemorySize() <= sizeToRelease)
					{
						mMemAllocator->free(reinterpret_cast<void*>(page.baseAddr), page.heap->getTotalMemorySize());
						sizeToRelease -= page.heap->getTotalMemorySize();
						CMM_DELETE_SINGLE(page.heap);
						numDeletes++;
					}
					else
					{
						resetHeap(page);
					}
				}
			}

			if (numDeletes)
			{
				removeDeletedHeapsFromList(numDeletes);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool HeapManagerRef::reserve(size_t size)
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

	size_t freeSize = 0;
	for (uint32_t i = 0; i < mHeaps.size(); i++)
	{
		freeSize += mHeaps[i].heap->getTotalMemorySize() - mHeaps[i].heap->getAllocatedMemorySize();
	}

	if (freeSize < size)
	{
		return allocateNewPages(size - freeSize) != NULL;
	}
	else
	{
		return true;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PX_INLINE size_t HeapManagerRef::findNextPow2(const size_t val, const uint8_t min, const uint8_t max)
{
	size_t ret = val;
	for (uint8_t i = min; i <= max; i++)
	{
		ret = size_t(1) << i;
		if (ret >= val)
		{
			break;
		}
	}
	return ret;
}


PX_INLINE size_t HeapManagerRef::findNextPow2(uint8_t& pow, const size_t val, const uint8_t min, const uint8_t max)
{
	size_t ret = val;
	for (pow = min; pow <= max; pow++)
	{
		ret = size_t(1) << pow;
		if (ret >= val)
		{
			break;
		}
	}
	return ret;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BuddyList::clear()
{
	Buddy* cur = pop();
	while (cur)
	{
		heap->getBuddyPool().destroy(cur);
		cur = pop();
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Heap::~Heap()
{
	for (uint32_t i = 0; i <= mMaxLevel; i++)
	{
		mFreeBuddiesAtLevel[i].clear();
	}
	CMM_DELETE_ARRAY(mFreeBuddiesAtLevel);

	for (Buddy* buddy = mAllocatedBuddies.getHead(); buddy != NULL; buddy = buddy->next)
	{
		void* address = reinterpret_cast<void*>((buddy->addr << mMinBlockLog2) + mBaseAddr);
#if KEEP_DEBUG_INFO
		char buffer[256];
		physx::shdfnd::snprintf(buffer, 256, "Memory leak!\naddress %p file %s, line %d, name %s",  address, buddy->file, buddy->line, buddy->allocName);
		mErrorCallback.reportError(PxErrorCode::eDEBUG_WARNING, buffer, __FILE__, __LINE__);
#else
		char buffer[256];
		physx::shdfnd::snprintf(buffer, 256, "Memory leak at address %p", address);
		mErrorCallback.reportError(PxErrorCode::eDEBUG_WARNING, buffer, __FILE__, __LINE__);
#endif
	}

	//clear it anyway
	mAllocatedBuddies.clear();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Heap::init(size_t baseAddr, const size_t baseSize, const size_t minBlockSize, const size_t maxIntFrag)
{
	if (minBlockSize != HeapManagerRef::findNextPow2(mMinBlockLog2, minBlockSize, 0, BITSPERWORD))
	{
		return false;
	}

	if ((maxIntFrag != size_t(-1)) && (maxIntFrag != HeapManagerRef::findNextPow2(maxIntFrag, 0, BITSPERWORD)))
	{
		return false;
	}

	mMaxIntFrag = maxIntFrag;

	mMinBlockSize = minBlockSize;
	mTotalSize = baseSize;
	mBaseAddr = baseAddr;

	if (mBaseAddr == 0)
	{
		return false;
	}

	size_t numBlocks = baseSize >> mMinBlockLog2;
	// allow only memory blocks which have a power of 2 in size. and numblocks must be at least 1.
	if (numBlocks != HeapManagerRef::findNextPow2(mMaxLevel, numBlocks, 0, sizeof(size_t) * 4))
	{
		return false;
	}

	mFreeBuddiesAtLevel = PX_NEW(BuddyList)[(unsigned int)(mMaxLevel + 1)];
	if (!mFreeBuddiesAtLevel)
	{
		return false;
	}

	// init size of buddy arrays
	for (uint32_t i = 0; i <= mMaxLevel; i++)
	{
		mFreeBuddiesAtLevel[i].buddySize = size_t(1) << i;
		mFreeBuddiesAtLevel[i].heap = this;
	}
	mAllocatedBuddies.heap = this;

	Buddy* b = mManager.getBuddyPool().construct();
	if (!b)
	{
		CMM_DELETE_ARRAY(mFreeBuddiesAtLevel);
		return false;
	}
	b->level = mMaxLevel;

	// add buddy to its array
	mFreeBuddiesAtLevel[mMaxLevel].insert(b);

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* Heap::alloc(const size_t size, PX_ALLOC_INFO_PARAMS_DEF())
{
	/*
	compute needed buddysize -> level
	if(mFreeBuddiesAtLevel[level].array.size() > 0)
	{
		ret =  array.popBack()
		allocList.pushBack(ret)
		return ret.adr+basePtr;
	}
	else
	{
		if(nonemptylist at higher levels)
			ret = recursive split
		else if(nonemptylist at lower levels )
			ret = recursive merge
		else
			ret = NULL; // heap full or to fragmented
	}
	*/

	PX_UNUSED(allocId);
	PX_UNUSED(allocName);
	PX_UNUSED(line);
	PX_UNUSED(file);

	if (size == 0 || size > mTotalSize)
	{
		return NULL;
	}

	PX_ASSERT(allocId < PxAllocId::NUM_IDS);
	//PX_ASSERT(allocId != PxAllocId::UNASSIGNED);	// enable to track unassigned memory

	// compute needed buddysize -> level
	uint8_t level = 0;
	HeapManagerRef::findNextPow2(level, size, mMinBlockLog2, BITSPERWORD);
	level = uint8_t(level - mMinBlockLog2);

	Buddy* ret = NULL;
	if (mFreeBuddiesAtLevel[level].getSize() > 0)
	{
		ret = mFreeBuddiesAtLevel[level].pop();
	}
	else
	{
		// prefer splitting
		if (level != mMaxLevel)
		{
			ret = findBySplitting(level);
		}
		// else try merging
		if (!ret && level != 0)
		{
			ret = findByMerging(level);
		}
	}

	if (ret)
	{
		ret->occupiedSize = size;
		size_t addr = ret->addr;
		ret->allocId = uint16_t(allocId);
#if KEEP_DEBUG_INFO
		ret->file = file;
		ret->line = (uint32_t)line;
		ret->allocName = allocName;
#endif

		size_t allocSize;
		if (mMaxIntFrag != size_t(-1))
		{
			allocSize = reduceIntFragment(*ret, mMaxIntFrag); // ret can be changed in here, that's why we store the address
		}
		else
		{
			allocSize = size_t(1) << (level + mMinBlockLog2);
			mAllocatedBuddies.insertSorted(ret);
		}
		mAllocMem += allocSize;
		mInternalFragmentation += allocSize - size;
		mMaxAllocMem = PxMax(mAllocMem, mMaxAllocMem);
		mMaxInternalFragmentation = PxMax(mInternalFragmentation, mMaxInternalFragmentation);
		mManager.addToStats(allocId, allocSize, allocSize - size);

		PX_ASSERT(sanityTest());
		return reinterpret_cast<void*>((addr << mMinBlockLog2) + mBaseAddr);
	}
	else
	{
		PX_ASSERT(sanityTest());
		return 	NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Heap::realloc(void* addr, const size_t size, PX_ALLOC_INFO_PARAMS_DEF())
{
	PX_UNUSED(allocId);
	PX_UNUSED(allocName);
	PX_UNUSED(line);
	PX_UNUSED(file);

	if (addr == NULL || size > mTotalSize)
	{
		return false;
	}

	if (size == 0)
	{
		//realloc to 0 bytes can't keep the pointer as it was.
		return false;
		//return free(addr);
	}

	size_t inernal_addr = reinterpret_cast<size_t>(addr) - mBaseAddr;
	inernal_addr >>= mMinBlockLog2;

	// collect all buddies which are associated with this addr
	shdfnd::Array<Buddy*, shdfnd::TempAllocator> budyList;
	size_t totalAllocated = 0;
	size_t buddyAllocated = 0;
	Buddy* found = NULL;
	do
	{
		found = mAllocatedBuddies.find(inernal_addr);
		if (!found)
		{
			return false;
		}
		budyList.pushBack(found);
		inernal_addr += size_t(1) << found->level;
		totalAllocated += found->occupiedSize;
		buddyAllocated += size_t(1) << (found->level + mMinBlockLog2);
	}
	while (found && !found->isLastBuddy);

	Buddy* cur = budyList.popBack();

	// increase size
	if (totalAllocated < size)
	{
		size_t leftSpace = (size_t(1) << (cur->level + mMinBlockLog2)) - cur->occupiedSize;
		size_t neededSpace = size - totalAllocated;
		if (neededSpace <= leftSpace)
		{
			cur->occupiedSize += neededSpace;
#if KEEP_DEBUG_INFO
			cur->file = file;
			cur->line = (uint32_t)line;
			cur->allocName = allocName;
#endif

			mInternalFragmentation -= neededSpace;
			mManager.decStats(PxAllocId::Enum(cur->allocId), 0, neededSpace);

			// replace
			mAllocatedBuddies.remove(cur);
			mAllocatedBuddies.insertSorted(cur);
			PX_ASSERT(sanityTest());
			return true;
		}
		else
		{
			return false;
#ifdef UNREACHABLE
			// TODO:try merge free buddies until big enough,
			// then add buddy and do internal fragmentation reduction.

			// search for free blocks next to this one.
			size_t addr = cur->addr + (size_t(1) << cur->level);
			if (!mAllocatedBuddies.find(addr))
			{
				return false;
			}

			// if not found, return null, let user reallocate
			PX_ASSERT(sanityTest());
			return false;
#endif
		}
	}
	// reduce size
	else
	{
		// succededly remove buddies until the requested size is reached.
		// if internal fragmentation reduction is turned on, then an allocation can consist of multiple buddies.
		mInternalFragmentation -= (size_t(1) << (cur->level + mMinBlockLog2)) - cur->occupiedSize;
		mManager.decStats(PxAllocId::Enum(cur->allocId), 0, (size_t(1) << (cur->level + mMinBlockLog2)) - cur->occupiedSize);
		size_t diff = totalAllocated - size;
		while (diff >= cur->occupiedSize)
		{
			diff -= cur->occupiedSize;
			cur->occupiedSize = 0;
			bool succ = mAllocatedBuddies.remove(cur);
			PX_UNUSED(succ);
			PX_ASSERT(succ);
			mFreeBuddiesAtLevel[cur->level].insertSorted(cur);
			size_t allocSize = size_t(1) << (cur->level + mMinBlockLog2);
			mAllocMem -= allocSize;
			mManager.decStats(PxAllocId::Enum(cur->allocId), allocSize, 0);
			cur = budyList.popBack();
		}
		cur->isLastBuddy = true;
		cur->occupiedSize -= diff;

#if KEEP_DEBUG_INFO
		cur->file = file;
		cur->line =(uint32_t)line;
		cur->allocName = allocName;
#endif

		// replace
		bool succ = mAllocatedBuddies.remove(cur);
		PX_UNUSED(succ);
		PX_ASSERT(succ);
		mAllocatedBuddies.insertSorted(cur);
		mInternalFragmentation += (size_t(1) << (cur->level + mMinBlockLog2)) - cur->occupiedSize;
		mManager.incStats(PxAllocId::Enum(cur->allocId), 0, (size_t(1) << (cur->level + mMinBlockLog2)) - cur->occupiedSize);
		PX_ASSERT(sanityTest());
		return true;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Heap::free(void* addr)
{
	if (addr == NULL)
	{
		return false;
	}

	size_t internalAddr = reinterpret_cast<size_t>(addr) - mBaseAddr;
	internalAddr >>= mMinBlockLog2;

	bool ret = true;

	bool dummy = true;
	while (dummy)
	{
		Buddy* b = mAllocatedBuddies.findAndPop(internalAddr);

		if (!b)
		{
			return false;
		}

		size_t allocSize = size_t(1) << (b->level + mMinBlockLog2);
		mAllocMem -= allocSize;
		mInternalFragmentation -= allocSize - b->occupiedSize;
		mManager.removeFromStats(PxAllocId::Enum(b->allocId), allocSize, allocSize - b->occupiedSize);
		b->occupiedSize = 0;

		mFreeBuddiesAtLevel[b->level].insertSorted(b);

		// check if this memory block occupied another buddy
		if (b->isLastBuddy)
		{
			break;
		}
		else
		{
			internalAddr += size_t(1) << b->level;
		}
	}

	if (mAllocMem == 0)
	{
		mManager.notifyEmptyPage();
	}

	PX_ASSERT(sanityTest());
	return ret;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Heap::getStats(ApexHeapStats& stats, const uint32_t flags)
{
	if (flags & HeapStatsFlags::F_BASIC_STATS)
	{
		stats.heapSize = mTotalSize;
		stats.totalAllocated = mAllocMem;
		stats.maxAllocated = mMaxAllocMem;
	}
	if (flags & HeapStatsFlags::F_INTERNAL_FRAGMENTATION)
	{
		// internal fragmentation
		stats.internalFragmentation = mInternalFragmentation;
		stats.maxInternalFragmentation = mMaxInternalFragmentation;
	}
	if (flags & HeapStatsFlags::F_BIGGEST_FREE_BLOCK)
	{
		// bigggest free block
		stats.biggestFreeBlock = 0;
		uint8_t curLevel = mMaxLevel;
		do
		{
			if (mFreeBuddiesAtLevel[curLevel].getSize())
			{
				stats.biggestFreeBlock = mFreeBuddiesAtLevel[curLevel].buddySize << mMinBlockLog2;
				break;
			}
			curLevel--;
		}
		while (curLevel != 0);
	}
	if (flags & HeapStatsFlags::F_HISTOGRAM)
	{
		// histograms
		for (uint8_t i = 0; i <= mMaxLevel; i++)
		{
			stats.freeBuddyHistogram[i] = mFreeBuddiesAtLevel[i].getSize();
			stats.allocatedBuddyHistogram[i] = 0;
		}
		Buddy* b = mAllocatedBuddies.getHead();
		while (b)
		{
			stats.allocatedBuddyHistogram[b->level]++;
			b = b->next;
		}
		stats.numEntries = size_t(mMaxLevel + 1);
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Heap::sanityTest()
{
	size_t sum = 0;

	for (uint8_t i = 0; i <= mMaxLevel; i++)
	{
		size_t buddiesAtLevel = 0;
		for (Buddy* cur = mFreeBuddiesAtLevel[i].getHead(); cur; cur = cur->next)
		{
			if ((size_t(1) << cur->level != mFreeBuddiesAtLevel[i].buddySize) ||
			        (cur->occupiedSize > size_t(1) << (cur->level + mMinBlockLog2)))
			{
				return false;
			}
			sum += mFreeBuddiesAtLevel[i].buddySize << mMinBlockLog2;
			buddiesAtLevel++;
		}
		if (mFreeBuddiesAtLevel[i].getSize() != buddiesAtLevel ||
		        (buddiesAtLevel > (size_t(1) << (mMaxLevel - i))))
		{
			return false;
		}
	}

	size_t numAllocated = 0;
	for (Buddy* cur = mAllocatedBuddies.getHead(); cur; cur = cur->next)
	{
		sum += size_t(1) << (cur->level + mMinBlockLog2);
		numAllocated++;
	}

	if (numAllocated != mAllocatedBuddies.getSize())
	{
		return false;
	}

	ptrdiff_t diff = ptrdiff_t(sum - (size_t(1) << (mMaxLevel + mMinBlockLog2)));
	if (diff != 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Heap::visualizeMemory(uint8_t* array, const size_t size)
{
	float scale = (float)size / (size_t(1) << mMaxLevel);

	for (size_t i = 0; i < size; i++)
	{
		array[i] = 0;
	}
	for (Buddy* cur = mAllocatedBuddies.getHead(); cur; cur = cur->next)
	{
		size_t start = (size_t)((float)(cur->addr) * scale);
		size_t end = (size_t)((float)(cur->addr + (size_t(1) << size_t(cur->level))) * scale);
		PX_ASSERT(start <= size);
		PX_ASSERT(end <= size);
		for (size_t i = start; i < end; i++)
		{
			PX_ASSERT(i < size);
			array[i] = uint8_t(cur->level + 1);
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Heap::splitBuddy(Buddy* b)
{
	if (b->level == 0)
	{
		return false;
	}

	b->level--;
	size_t newSize = size_t(1) << b->level;

	Buddy* b0 = b;
	Buddy* b1 = mManager.getBuddyPool().construct(*b);
	PX_ASSERT(b0 && b1);

	b1->addr = b1->addr + newSize;

	mFreeBuddiesAtLevel[b0->level].insertSorted(b0);
	mFreeBuddiesAtLevel[b1->level].insertSorted(b1);
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Heap::mergeBuddies(Buddy* b0, Buddy* b1)
{
	if (b0->level != b1->level || b0->level >= mMaxLevel || (b1->addr - b0->addr) != size_t(1) << size_t(b0->level))
	{
		return false;
	}

	Buddy* b = b0;
	b->occupiedSize = 0;
	b->isLastBuddy = true;
	b->level++;
	b->next = NULL;
	mFreeBuddiesAtLevel[b->level].insertSorted(b);

	mManager.getBuddyPool().destroy(b1);
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Buddy* Heap::findBySplitting(uint8_t searchLevel)
{
	Buddy* ret = NULL;

	uint8_t curLevel = searchLevel;

	// walk through array of buddy lists and search for a free buddy which is at level >= searchLevel
	for (; !mFreeBuddiesAtLevel[curLevel].getSize() && (curLevel < mMaxLevel); curLevel++)
	{
		;
	}

	// pop buddy at highest level and split until it has the correct level
	ret = mFreeBuddiesAtLevel[curLevel].pop();
	for (; ret && (curLevel != searchLevel) && curLevel > 0; curLevel--)
	{
		splitBuddy(ret);
		ret = mFreeBuddiesAtLevel[curLevel - 1].pop();
	}
	return ret;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Buddy* Heap::findByMerging(uint8_t searchLevel)
{
	/*
		while there is no pair to merge on this level, ask lower level to merge
		may ask lower level to merge more than 1 pair after each failure
		or just merge all pairs of lower levels
	*/
	if (searchLevel == 0)
	{
		return NULL;
	}

	uint8_t curLevel = uint8_t(searchLevel - 1);
	bool dummy = true;
	while (dummy)
	{
		int32_t shift = (mMaxLevel - (1 << (curLevel + 1)));
		shift = shift >= 0 ? shift : 0;
		size_t numToFind = size_t(1) << shift;
		size_t found = findPairAndMerge(mFreeBuddiesAtLevel[curLevel], numToFind);
		if (found)
		{
			if (curLevel == searchLevel - 1)
			{
				break;
			}
			curLevel++;
		}
		else
		{
			if (curLevel > 0)
			{
				curLevel--;
			}
			else
			{
				return NULL;
			}
		}
	}
	return mFreeBuddiesAtLevel[searchLevel].pop();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t Heap::findPairAndMerge(BuddyList& list, size_t numToFind)
{
	size_t found = 0;
	Buddy* cur = list.getHead();
	Buddy* last = list.getHead();
	size_t diff = list.buddySize;
	while ((found != numToFind) && cur && cur->next)
	{
		// find buddy pair b0 and b1, b0 must be at an even address, and b0 and b1 must be neighbours in address space.
		// since the list is sorted, we do only compare neighbours in the list.
		if (((cur->addr & (size_t(1) << size_t(cur->level))) == 0) && (cur->next->addr - cur->addr == diff))
		{
			Buddy* b0 = cur;
			Buddy* b1 = cur->next;

			if (cur == list.getHead())
			{
				list.setHead(cur->next->next);
				cur = list.getHead();
				last = cur;
			}
			else
			{
				cur = cur->next->next;
				last->next = cur;
			}
			list.setSize(list.getSize() - 2);
			if (mergeBuddies(b0, b1))
			{
				found++;
			}
		}
		else
		{
			last = cur;
			cur = cur->next;
		}
	}
	return found;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t Heap::reduceIntFragment(Buddy& b, size_t threshold)
{
	/*
	while internalFragmentation > threshold
		cut buddy in the middle
		if cut goes through occupied space
			left buddy is full, can be pushed to alloc list clear last buddy flag
		else
			right buddy is free, push it to free list
	*/
	size_t allocSize = 0;
	Buddy* curB = &b;
	curB->isLastBuddy = false;
	while (curB->level && ((size_t(1) << (curB->level + mMinBlockLog2)) - curB->occupiedSize) > threshold)
	{
		//split
		Buddy* b0 = mManager.getBuddyPool().construct(*curB);
		Buddy* b1 = curB;
		b0->level--;
		b1->level--;
		b1->addr += size_t(1) << size_t(b1->level);
		if ((size_t(1) << (b0->level + mMinBlockLog2)) < b0->occupiedSize)
		{
			b0->occupiedSize = size_t(1) << (b0->level + mMinBlockLog2);
			b1->occupiedSize -= b0->occupiedSize;
			mAllocatedBuddies.insertSorted(b0);
			allocSize += size_t(1) << b1->level;
			curB = b1;
		}
		else
		{
			b1->occupiedSize = 0;
			mFreeBuddiesAtLevel[b1->level].insertSorted(b1);
			curB = b0;
		}
	}
	curB->isLastBuddy = true;
	allocSize += size_t(1) << curB->level;
	mAllocatedBuddies.insertSorted(curB);
	return (allocSize << mMinBlockLog2);
}


