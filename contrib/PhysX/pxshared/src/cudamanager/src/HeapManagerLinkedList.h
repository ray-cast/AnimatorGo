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
// HeapManagerLinkedList.h
//----------------------------------------------------------------------------//

#ifndef PXCUDACONTEXTMANAGER_HEAPMANAGERLINKEDLIST_H
#define PXCUDACONTEXTMANAGER_HEAPMANAGERLINKEDLIST_H

#include "foundation/PxAssert.h"

namespace physx
{

template <typename T>
class LinkedList
{
public:

	PX_INLINE LinkedList()
		: mHead(NULL)
		, mNumElements(0)
	{
	}

	PX_INLINE void insert(T*& elt)
	{
		if (mHead)
		{
			elt->next = mHead->next;
		}
		mHead = elt;
		mNumElements++;
	}

	PX_INLINE void insertSorted(T*& elt)
	{
		if (!mHead)
		{
			mHead = elt;
			mHead->next = NULL;
		}
		else if (!mHead->next || (mHead->addr > elt->addr))
		{
			if (mHead->addr > elt->addr)
			{
				elt->next = mHead;
				mHead = elt;
			}
			else
			{
				mHead->next = elt;
				elt->next = NULL;
			}
		}
		else
		{
			T* cur = mHead;
			while (cur->next && (elt->addr > cur->next->addr))
			{
				cur = cur->next;
			}
			elt->next = cur->next;
			cur->next = elt;
		}
		mNumElements++;
	}

	PX_INLINE T* pop()
	{
		if (mHead)
		{
			T* ret = mHead;
			mHead = mHead->next;
			mNumElements--;
			return ret;
		}
		return NULL;
	}

	PX_INLINE bool remove(const T* elt)
	{
		PX_ASSERT(elt);
		if (mHead && mHead == elt)
		{
			mHead = mHead->next;
			mNumElements--;
			return true;
		}
		else
		{
			T* cur = mHead;
			while (cur && cur->next != elt)
			{
				PX_ASSERT(cur->addr < elt->addr); // assert for sorted list property.
				cur = cur->next;
			}
			if (cur && elt)
			{
				cur->next = elt->next;
				mNumElements--;
				return true;
			}
		}
		return false;
	}

	PX_INLINE T* find(const size_t addr)
	{
		T* cur = mHead;
		while (cur && cur->addr < addr)
		{
			cur = cur->next;
		}

		return cur && (cur->addr == addr) ? cur : NULL;
	}

	PX_INLINE T* findAndPop(const size_t addr)
	{
		if (mHead == NULL)
		{
			return NULL;
		}

		if (mHead->addr == addr)
		{
			return pop();
		}

		T* cur = mHead;
		T* last = mHead;
		while (cur)
		{
			if (cur->addr == addr)
			{
				last->next = cur->next;
				mNumElements--;
				return cur;
			}
			else if (cur->addr > addr)
			{
				return NULL; // because list is sorted.
			}
			else
			{
				last = cur;
				cur = cur->next;
			}
		}
		return NULL;
	}

	PX_INLINE	size_t	getSize()
	{
		return mNumElements;
	}
	PX_INLINE	T*		getHead()
	{
		return mHead;
	}

	// hacky
	PX_INLINE	void	setSize(size_t s)
	{
		mNumElements = s;
	}
	PX_INLINE	void	setHead(T* h)
	{
		mHead = h;
	}
private:
	T*		mHead;
	size_t	mNumElements;
};

}  // end physx namespace

#endif // PXCUDACONTEXTMANAGER_HEAPMANAGERLINKEDLIST_H
