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


#include <atomic>
#include "nn/os/os_Mutex.h"
#include "PsFoundation.h"
#include "PsAllocator.h"
#include "PsMutex.h"
#include "PsThread.h"

namespace physx
{
namespace shdfnd
{

namespace 
{
	struct MutexNXImpl
	{
		nn::os::MutexType lock;
		Thread::Id owner;
	};

	MutexNXImpl* getMutex(MutexImpl* impl)
	{
		return reinterpret_cast<MutexNXImpl*>(impl);
	}
}

MutexImpl::MutexImpl() 
{ 
	nn::os::InitializeMutex(&getMutex(this)->lock, true, 0);
}

MutexImpl::~MutexImpl() 
{ 
	nn::os::FinalizeMutex(&getMutex(this)->lock);
}

void MutexImpl::lock()
{
	nn::os::LockMutex(&getMutex(this)->lock);

#ifdef PX_DEBUG
	getMutex(this)->owner = Thread::getId();
#endif
}

bool MutexImpl::trylock()
{
	bool success = nn::os::TryLockMutex(&getMutex(this)->lock);
#ifdef PX_DEBUG
	if (success)
		getMutex(this)->owner = Thread::getId();
#endif
	return success;
}

void MutexImpl::unlock()
{
#ifdef PX_DEBUG
	// ensure we are already holding the lock
	if (getMutex(this)->owner != Thread::getId())
	{
		getFoundation().error(PxErrorCode::eINVALID_OPERATION, __FILE__, __LINE__, "Mutex must be unlocked only by thread that has already acquired lock");
		return;
	}
#endif

	nn::os::UnlockMutex(&getMutex(this)->lock);
}

static const PxU32 gSize = sizeof(MutexNXImpl);

const PxU32& MutexImpl::getSize()  { return gSize; }

class ReadWriteLockImpl
{
	PX_NOCOPY(ReadWriteLockImpl)
public:
	ReadWriteLockImpl() : readerCount(0) {}
	Mutex				mutex;
	std::atomic<int>	readerCount;  //handle recursive writer locking
};

ReadWriteLock::ReadWriteLock()
{
    mImpl = reinterpret_cast<ReadWriteLockImpl*>(PX_ALLOC(sizeof(ReadWriteLockImpl), PX_DEBUG_EXP("ReadWriteLockImpl")));
	PX_PLACEMENT_NEW(mImpl, ReadWriteLockImpl);
}

ReadWriteLock::~ReadWriteLock()
{
	mImpl->~ReadWriteLockImpl();
    PX_FREE( mImpl );
}

void ReadWriteLock::lockReader()
{
	mImpl->mutex.lock();

	mImpl->readerCount.fetch_add(1);

	mImpl->mutex.unlock();
}

void ReadWriteLock::lockWriter()
{
	mImpl->mutex.lock();

	// spin lock until no readers
	while (mImpl->readerCount);
}

void ReadWriteLock::unlockReader()
{
	mImpl->readerCount.fetch_sub(1);
}

void ReadWriteLock::unlockWriter()
{
	mImpl->mutex.unlock();
}

} // namespace shdfnd
} // namespace physx

