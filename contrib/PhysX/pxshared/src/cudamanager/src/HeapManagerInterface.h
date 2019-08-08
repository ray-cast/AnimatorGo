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
// HeapManagerInterface.h
//----------------------------------------------------------------------------//

#ifndef PXCUDACONTEXTMANAGER_HEAPMANAGERINTERFACE_H
#define PXCUDACONTEXTMANAGER_HEAPMANAGERINTERFACE_H

#include "task/PxTaskDefine.h"
#include "foundation/PxSimpleTypes.h"
#include "cudamanager/PxCudaMemoryManager.h"

#include <string.h>

namespace physx
{
struct HeapStatsFlags
{
	enum Enum
	{
		F_BASIC_STATS				= 1 << 0,
		F_INTERNAL_FRAGMENTATION	= 1 << 1,
		F_BIGGEST_FREE_BLOCK		= 1 << 2,
		F_HISTOGRAM					= 1 << 3,
		F_ALLOC_ID_STATS			= 1 << 4,
		F_ALL						= 0xFFFFFFFF,
	};
};

#define BITSPERWORD sizeof(size_t)*8

class ApexHeapStats
{
public:
	ApexHeapStats():
		heapSize(0),
		totalAllocated(0),
		maxAllocated(0),
		internalFragmentation(0),
		maxInternalFragmentation(0),
		biggestFreeBlock(0),
		numEntries(0)
	{}

	PX_INLINE void reset()
	{
		memset(this, 0, sizeof(ApexHeapStats));
	}

	// F_BASIC_STATS
	size_t	heapSize;
	size_t	totalAllocated;
	size_t	maxAllocated;

	// F_INTERNAL_FRAGMENTATION
	size_t	internalFragmentation;
	size_t	maxInternalFragmentation;

	// F_BIGGEST_FREE_BLOCK
	size_t	biggestFreeBlock;

	// F_HISTOGRAM
	size_t	freeBuddyHistogram[BITSPERWORD];
	size_t	allocatedBuddyHistogram[BITSPERWORD];
	size_t	numEntries;

	// F_ALLOC_ID_STATS
	PxAllocIdStats allocIdStats[PxAllocId::NUM_IDS];
};


class HeapManagerInterface
{
public:
	// simple allocator interface over which the heap manager does its base allocation and allocates further pages
	class Allocator
	{
	public:
		virtual ~Allocator() {};

		virtual		void*		alloc(const size_t size) = 0;
		virtual		void		free(void* addr, const size_t size) = 0;
	};

	virtual ~HeapManagerInterface() {};

	// INTERFACE METHODS
	// init the HeapManager by passing it a block of memory and the smallest size of a memory block.
	// returns true if init was successful
	virtual		bool		init(Allocator* memAllocator, const size_t baseSize, const size_t pageSize, const size_t minBlockSize, const size_t maxIntFrag = size_t(-1)) = 0;

	// Changes the page size. The size of allocations over the supplied Allocator are a multiple of the pageSize.
	// returns true if the page size was valid. (!0, >minBlockSize, pow2)
	virtual		bool		setPageSize(size_t pageSize) = 0;

	// returns the address of an allocated block for the requested size.
	// returns a NULL ptr if alloc failed.
	virtual		void*		alloc(const size_t size, PX_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED)) = 0;

	// returns true if the block at the given address could be resized to size
	// returns false if this failed. Manual free and alloc is still possible but needs a memcopy.
	virtual		bool		realloc(void* addr, const size_t size, PX_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED)) = 0;

	// frees a given block.
	// returns true if the operation was successful
	virtual		bool		free(void* addr) = 0;

	// deallocates all empty pages
	virtual		void		freeEmptyPages() = 0;

	// ensures that there there is free memory of at least the requested size
	// returns true if the operation was successful. Free memory was already big enough or new pages were allocated successfully.
	virtual		bool		reserve(size_t size) = 0;

	// returns stats into a ApexHeapStats object, stats can be selected with HeapManagerStatsFlags.
	// returns true if the operation was successful
	virtual		bool		getStats(ApexHeapStats& stats, const uint32_t flags) = 0;

	// discretisize memory into an array such that it can be visualized
	// returns true if the operation was successful
	virtual		bool		visualizeMemory(uint8_t* array, const size_t size) = 0;

	// returns the base address of the page containing the memory block at addr. 
	// returns NULL if addr doesn't correspond to a page
	virtual		void*		findBaseAddress(void* addr) = 0;
};

} // end physx namespace

#endif // PXCUDACONTEXTMANAGER_HEAPMANAGERINTERFACE_H
