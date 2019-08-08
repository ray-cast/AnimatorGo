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


#include "nn/os/os_ConditionVariable.h"
#include "nn/os/os_Tick.h"
#include "nn/nn_TimeSpan.h"
#include "nn/os/os_Mutex.h"
#include "foundation/PxAssert.h"
#include "PsSync.h"


namespace physx
{
namespace shdfnd
{
	namespace
	{
		class _SyncImpl
		{
		public:
			nn::os::ConditionVariableType syncCondVar;  // note: nn::os::EventType is not used because nn::os::ClearEvent() is not atomic
			nn::os::MutexType syncMutex;
			volatile PxI32 setCounter;
			volatile bool is_set;
		};
		
		_SyncImpl* getSync(SyncImpl* impl)
		{
			return reinterpret_cast<_SyncImpl*>(impl);
		}
	}
	
	static const PxU32 gSize = sizeof(_SyncImpl);
	const PxU32& SyncImpl::getSize()  { return gSize; }

	struct PxNXScopedLock
	{
	private:
		PX_NOCOPY(PxNXScopedLock)

	public:
		PxNXScopedLock(nn::os::MutexType& lock) : mLock(lock)	{ nn::os::LockMutex(&mLock); }
		~PxNXScopedLock()										{ nn::os::UnlockMutex(&mLock); }

	private:
		nn::os::MutexType& mLock;
	};

	SyncImpl::SyncImpl()
	{
		_SyncImpl* syncImpl = getSync(this);
		nn::os::InitializeMutex(&syncImpl->syncMutex, false, 0);  // non-recursive is correct even if it might be slightly confusing the way it is used but nn::os::WaitConditionVariable() unlocks and locks again
		nn::os::InitializeConditionVariable(&syncImpl->syncCondVar);
		syncImpl->setCounter = 0;
		syncImpl->is_set = false;
	}

	SyncImpl::~SyncImpl()
	{
		_SyncImpl* syncImpl = getSync(this);
		nn::os::FinalizeConditionVariable(&syncImpl->syncCondVar);
		nn::os::FinalizeMutex(&syncImpl->syncMutex);
	}

	void SyncImpl::reset()
	{
		_SyncImpl* syncImpl = getSync(this);
		PxNXScopedLock lock(syncImpl->syncMutex);
		syncImpl->is_set = false;
	}

	void SyncImpl::set()
	{
		_SyncImpl* syncImpl = getSync(this);
		PxNXScopedLock lock(syncImpl->syncMutex);
		if(!syncImpl->is_set)
		{
			syncImpl->setCounter++;
			syncImpl->is_set = true;
			nn::os::BroadcastConditionVariable(&syncImpl->syncCondVar);
		}
	}

	bool SyncImpl::wait(PxU32 milliseconds)
	{
		_SyncImpl* syncImpl = getSync(this);
		PxNXScopedLock lock(syncImpl->syncMutex);
		PxI32 lastSetCounter = syncImpl->setCounter;
		if(!getSync(this)->is_set)
		{
			if(milliseconds == static_cast<PxU32>(-1))
			{
				// have to loop here and check is_set since WaitConditionVariable can return even 
				// if it was not signaled by BroadcastConditionVariable
				while((!syncImpl->is_set) && (lastSetCounter == syncImpl->setCounter))
					nn::os::WaitConditionVariable(&syncImpl->syncCondVar, &syncImpl->syncMutex);
				PX_ASSERT(syncImpl->is_set || (lastSetCounter != syncImpl->setCounter));
			}
			else
			{
				const int64_t ticksToWait = (static_cast<int64_t>(milliseconds) * nn::os::GetSystemTickFrequency()) / 1000;
				const int64_t targetCounter = nn::os::GetSystemTick().GetInt64Value() + ticksToWait;
				const int64_t targetCounterWithMargin = targetCounter - (targetCounter >> 4) + 1;  // allow for a bit of error in the wait time (around 6%)
				
				// have to loop here and check is_set since TimedWaitConditionVariable can return even 
				// if it was not signaled by BroadcastConditionVariable. Note: to keep it simple, the elapsed
				// time is not taken into account in such a case and the original wait time will be used again.
				bool timeLimitOk = true;
				while((!syncImpl->is_set) && (lastSetCounter == syncImpl->setCounter) && timeLimitOk)
				{
					nn::os::TimedWaitConditionVariable(&syncImpl->syncCondVar, &syncImpl->syncMutex, nn::os::ConvertToTimeSpan(nn::os::Tick(ticksToWait)));
					timeLimitOk = nn::os::GetSystemTick().GetInt64Value() <= targetCounterWithMargin;
				}
				PX_ASSERT(syncImpl->is_set || (lastSetCounter != syncImpl->setCounter) || (!timeLimitOk));
			}
		}

		return syncImpl->is_set || (lastSetCounter != syncImpl->setCounter);
	}

} // namespace shdfnd
} // namespace physx
