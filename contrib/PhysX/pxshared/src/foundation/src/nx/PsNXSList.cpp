/*
 * Copyright (c) 2008-2015, NVIDIA CORPORATION.  All rights reserved.
 *
 * NVIDIA CORPORATION and its licensors retain all intellectual property
 * and proprietary rights in and to this software, related documentation
 * and any modifications thereto.  Any use, reproduction, disclosure or
 * distribution of this software and related documentation without an express
 * license agreement from NVIDIA CORPORATION is strictly prohibited.
 */
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  


#include "PsThread.h"  // for PxSpinLockPause()
#include "PsSList.h"

namespace physx
{
namespace shdfnd
{
	namespace 
	{
		struct ScopedSpinLock
		{
		private:
			PX_NOCOPY(ScopedSpinLock)

		public:
			// !!!pthread version need to check
			PX_FORCE_INLINE ScopedSpinLock(volatile PxI32& lock): mLock(lock)	
			{  
				while (__sync_lock_test_and_set(&mLock, 1))
				{
					// spinning without atomics is usually
					// causing less bus traffic. -> only one
					// CPU is modifying the cache line.
					while(lock)
						PxSpinLockPause();
				} 
			}

			PX_FORCE_INLINE ~ScopedSpinLock()
			{  
				__sync_lock_release(&mLock);
			}
		private:
			volatile PxI32& mLock;
		};
		
		struct SListDetail
		{
			SListEntry* head;
			volatile PxI32 lock;
		};

		template <typename T>
		SListDetail* getDetail(T* impl)
		{
			return reinterpret_cast<SListDetail*>(impl);
		}
	}
		
	SListImpl::SListImpl()
	{
		getDetail(this)->head = NULL;
		getDetail(this)->lock = 0; // 0 == unlocked
	}
	
	SListImpl::~SListImpl()
	{
	}
	
	void SListImpl::push(SListEntry* entry)
	{
		ScopedSpinLock lock(getDetail(this)->lock);
		entry->mNext = getDetail(this)->head;
		getDetail(this)->head = entry;
	}
	
	SListEntry* SListImpl::pop()
	{
		ScopedSpinLock lock(getDetail(this)->lock);
		SListEntry* result = getDetail(this)->head;
		if( result != NULL )
			getDetail(this)->head = result->mNext;
		return result;
	}
	
	SListEntry* SListImpl::flush()
	{
		ScopedSpinLock lock(getDetail(this)->lock);
		SListEntry* result = getDetail(this)->head;
		getDetail(this)->head = NULL;
		return result;
	}
	
	static const PxU32 gSize = sizeof(SListDetail);

	const PxU32& SListImpl::getSize()
	{
		return gSize;
	}

} // namespace shdfnd
} // namespace physx
