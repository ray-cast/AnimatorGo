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
#include "PsFoundation.h"
#include "nn/os/os_Thread.h"
#include "nn/nn_TimeSpan.h"

#include "PsBitUtils.h"
#include "PsThread.h"
#include "foundation/PxAssert.h"


namespace physx
{
namespace shdfnd
{

namespace
{
	class _ThreadImpl
	{
		PX_NOCOPY(_ThreadImpl)

	public:
		enum State
		{
			NotStarted,
			Started,
			Stopped
		};

		nn::os::ThreadType	nativeThread;

		PxU8*				stackMemory;
		int					threadAffinity;

		std::atomic<int>	quitNow;
		State				state;

		ThreadImpl::ExecuteFn	fn;
		void*				arg;

		static const int sInvalidAffinityMask = 0xffffffff;
	};

	_ThreadImpl* getThread(ThreadImpl* impl)
	{
		return reinterpret_cast<_ThreadImpl*>(impl);
	}

	PX_FORCE_INLINE void initThreadImpl(_ThreadImpl* threadImpl)
	{
		threadImpl->nativeThread._basePriority = nn::os::DefaultThreadPriority;
		threadImpl->nativeThread._namePointer = NULL;
		threadImpl->state = _ThreadImpl::NotStarted;
		threadImpl->quitNow = 0;
		threadImpl->threadAffinity = _ThreadImpl::sInvalidAffinityMask;
		threadImpl->fn = NULL;
		threadImpl->arg = NULL;
		threadImpl->stackMemory = NULL;
	}

	void ThreadStart(void* ptrArg)
	{
		// then run either the passed in function or execute from the derived class (Runnable).
		_ThreadImpl* impl = getThread(reinterpret_cast<ThreadImpl*>(ptrArg));
		if(impl->fn)
			(*impl->fn)(impl->arg);
		else if(impl->arg)
			(static_cast<Runnable*>(impl->arg))->execute();
	}

	PX_FORCE_INLINE void* allocateStackMemory(size_t size)
	{
		size_t pad = (nn::os::StackRegionAlignment - 1) + sizeof(size_t); // store offset for delete.
		PxU8* base = reinterpret_cast<PxU8*>(::malloc(size + pad));
		if (!base)
			return NULL;

		PxU8* ptr = reinterpret_cast<PxU8*>(size_t(base + pad) & ~(nn::os::StackRegionAlignment - 1)); // aligned pointer
		(reinterpret_cast<size_t*>(ptr))[-1] = static_cast<size_t>(ptr - base); // store offset

		return ptr;
	}

	PX_FORCE_INLINE void freeStackMemory(void* ptr)
	{
		if (ptr == NULL)
			return;

		PxU8* base = reinterpret_cast<PxU8*>(ptr) - (reinterpret_cast<size_t*>(ptr))[-1];
		::free(base);
	}
}

static const PxU32 gSize = sizeof(_ThreadImpl);
const PxU32& ThreadImpl::getSize()  { return gSize; }


ThreadImpl::Id ThreadImpl::getId()
{
	return reinterpret_cast<Id>(nn::os::GetCurrentThread());
}

ThreadImpl::ThreadImpl()
{
	initThreadImpl(getThread(this));
}

ThreadImpl::ThreadImpl(ExecuteFn fn, void* arg)
{
	_ThreadImpl* tImpl = getThread(this);

	initThreadImpl(tImpl);

	tImpl->fn = fn;
	tImpl->arg = arg;

	start(0, NULL);
}

ThreadImpl::~ThreadImpl()
{
	_ThreadImpl* tImpl = getThread(this);

	if ((tImpl->state != _ThreadImpl::NotStarted) && (tImpl->stackMemory != NULL))
	{
		nn::os::DestroyThread(&tImpl->nativeThread);
		freeStackMemory(reinterpret_cast<void*>(tImpl->stackMemory));
	}
}

PxU32 ThreadImpl::getDefaultStackSize()
{
	const PxU32 defaultSize = 524288;
	PX_COMPILE_TIME_ASSERT((defaultSize % nn::os::StackRegionAlignment) == 0);
	return defaultSize;
}

void ThreadImpl::start(PxU32 stackSize, Runnable* runnable)
{
	_ThreadImpl* tImpl = getThread(this);
	if(tImpl->state != _ThreadImpl::NotStarted)
		return;
	tImpl->state = _ThreadImpl::Started;

	PxU32 newStackSize = getDefaultStackSize();
	if(stackSize != 0)
		newStackSize = stackSize;

	PX_ASSERT((newStackSize % nn::os::StackRegionAlignment) == 0);

	// need to provide stack memory as well
	// (for other platforms, the system allocates the stack memory, hence it seems ok to not use the user allocator for this)
	PxU8* mem = reinterpret_cast<PxU8*>(allocateStackMemory(newStackSize));

	if (mem)
	{
		tImpl->stackMemory = mem;
		
		if(runnable && !tImpl->arg && ! tImpl->fn)
			tImpl->arg = runnable;

		const int priority = tImpl->nativeThread._basePriority;
		PX_ASSERT((priority <= nn::os::LowestThreadPriority) && (priority >= nn::os::HighestThreadPriority));

		nn::Result result = nn::os::CreateThread(&tImpl->nativeThread, ThreadStart, this, mem, newStackSize, priority);

		if (result.IsSuccess())
		{
			if (tImpl->threadAffinity != _ThreadImpl::sInvalidAffinityMask)
				nn::os::SetThreadCoreMask(&tImpl->nativeThread, nn::os::IdealCoreDontCare, static_cast<nn::Bit64>(tImpl->threadAffinity));

			if (tImpl->nativeThread._namePointer)
				nn::os::SetThreadNamePointer(&tImpl->nativeThread, tImpl->nativeThread._namePointer);

			nn::os::StartThread(&tImpl->nativeThread);
		}
		else
		{
			freeStackMemory(reinterpret_cast<void*>(mem));
			tImpl->stackMemory = NULL;
			PX_ALWAYS_ASSERT();
		}
	}
}

void ThreadImpl::signalQuit()
{
	getThread(this)->quitNow.fetch_add(1);
}

bool ThreadImpl::waitForQuit()
{
	_ThreadImpl* tImpl = getThread(this);

	if(tImpl->state == _ThreadImpl::NotStarted)
		return false;

	nn::os::WaitThread(&tImpl->nativeThread);

	return true;
}

bool ThreadImpl::quitIsSignalled()
{
	_ThreadImpl* tImpl = getThread(this);
	int expected = 0;
	return !tImpl->quitNow.compare_exchange_strong(expected, 0);
}

void ThreadImpl::quit()
{
	getThread(this)->state = _ThreadImpl::Stopped;

	// nothing to call. The thread will return and that will trigger all waiting threads to be informed.
}

void ThreadImpl::kill()
{
	PX_ASSERT(!"kill() is not implemented for this platform");

	// nn::os::DestroyThread() waits for the thread to exit, which does seem the wrong behavior for kill()
}

void ThreadImpl::sleep(PxU32 ms)
{
	nn::os::SleepThread(nn::TimeSpan::FromMilliSeconds(ms));
}

void ThreadImpl::yield()
{
	nn::os::YieldThread();
}

PxU32 ThreadImpl::setAffinityMask(PxU32 mask)
{
	PX_ASSERT((mask & (~nn::os::GetThreadAvailableCoreMask())) == 0);

	_ThreadImpl* tImpl = getThread(this);

	if (tImpl->state == _ThreadImpl::NotStarted)
	{
		const int previousMask = tImpl->threadAffinity;
		tImpl->threadAffinity = static_cast<int>(mask);

		if (previousMask == _ThreadImpl::sInvalidAffinityMask)
			return 0;
		else
			return static_cast<PxU32>(previousMask);
	}
	else
	{
		nn::Bit64 affMask;
		nn::os::GetThreadCoreMask(NULL, &affMask, &tImpl->nativeThread);

		nn::os::SetThreadCoreMask(&tImpl->nativeThread, nn::os::IdealCoreDontCare, static_cast<nn::Bit64>(mask));

		return static_cast<PxU32>(affMask);
	}
}

void ThreadImpl::setName(const char* name)
{
	// important: The memory has to be allocated and managed by the caller

	_ThreadImpl* tImpl = getThread(this);
	if(tImpl->state == _ThreadImpl::Started)
		nn::os::SetThreadNamePointer(&tImpl->nativeThread, name);
	else
		tImpl->nativeThread._namePointer = name;
}

void ThreadImpl::setPriority(ThreadPriority::Enum prio)
{
	_ThreadImpl* tImpl = getThread(this);

	int convertedPriority = ThreadPriority::eNORMAL;  // compiler complains if no default is set (even though all values are covered in the switch statement)
	switch(prio)
	{
	case ThreadPriority::eHIGH:
		convertedPriority = nn::os::HighestThreadPriority;
		break;
	case ThreadPriority::eABOVE_NORMAL:
		convertedPriority = (nn::os::HighestThreadPriority + nn::os::DefaultThreadPriority) / 2;
		break;
	case ThreadPriority::eNORMAL:
		convertedPriority = nn::os::DefaultThreadPriority;
		break;
	case ThreadPriority::eBELOW_NORMAL:
		convertedPriority = (nn::os::LowestThreadPriority + nn::os::DefaultThreadPriority) / 2;
		break;
	case ThreadPriority::eLOW:
		convertedPriority = nn::os::LowestThreadPriority;
		break;
	case ThreadPriority::eFORCE_DWORD:
		PX_ALWAYS_ASSERT();
		convertedPriority = nn::os::DefaultThreadPriority;
		break;
	}

	if (tImpl->state == _ThreadImpl::Started)
		nn::os::ChangeThreadPriority(&tImpl->nativeThread, convertedPriority);
	else
		tImpl->nativeThread._basePriority = convertedPriority;
}

ThreadPriority::Enum ThreadImpl::getPriority(Id tID)
{
	const nn::os::ThreadType* thread = reinterpret_cast<const nn::os::ThreadType*>(tID);
	int prio = nn::os::GetThreadCurrentPriority(thread);
	ThreadPriority::Enum convertedPriority;

	switch(prio)
	{
	case nn::os::HighestThreadPriority:
		convertedPriority = ThreadPriority::eHIGH;
		break;
	case ((nn::os::HighestThreadPriority + nn::os::DefaultThreadPriority) / 2):
		convertedPriority = ThreadPriority::eABOVE_NORMAL;
		break;
	case nn::os::DefaultThreadPriority:
		convertedPriority = ThreadPriority::eNORMAL;
		break;
	case ((nn::os::LowestThreadPriority + nn::os::DefaultThreadPriority) / 2):
		convertedPriority = ThreadPriority::eBELOW_NORMAL;
		break;
	case nn::os::LowestThreadPriority:
		convertedPriority = ThreadPriority::eLOW;
		break;
	default:
		PX_ALWAYS_ASSERT();
		convertedPriority = ThreadPriority::eNORMAL;
		break;
	}

	return convertedPriority;
}

PxU32 ThreadImpl::getNbPhysicalCores()
{
	nn::Bit64 mask = nn::os::GetThreadAvailableCoreMask();
	PX_ASSERT(mask > 0);

	const uint32_t count = bitCount(static_cast<const uint32_t>(mask));
	return count;
}


PxU32 TlsAlloc()
{
	// note: only nn::os::TlsSlotCountMax Tls slots are available (which used to be 16)

	nn::os::TlsSlot tlsSlot;
	nn::Result result = nn::os::AllocateTlsSlot(&tlsSlot, NULL);

	if (result.IsSuccess())
		return static_cast<PxU32>(tlsSlot._innerValue);
	else
	{
		PX_ALWAYS_ASSERT();
		return 0xffffffff;
	}
}

void TlsFree(PxU32 index)
{
	nn::os::TlsSlot tlsSlot;
	tlsSlot._innerValue = static_cast<uint32_t>(index);

	nn::os::FreeTlsSlot(tlsSlot);
}

void* TlsGet(PxU32 index)
{
	nn::os::TlsSlot tlsSlot;
	tlsSlot._innerValue = static_cast<uint32_t>(index);

	return reinterpret_cast<void*>(nn::os::GetTlsValue(tlsSlot));
}

PxU32 TlsSet(PxU32 index, void* value)
{
	nn::os::TlsSlot tlsSlot;
	tlsSlot._innerValue = static_cast<uint32_t>(index);

	nn::os::SetTlsValue(tlsSlot, reinterpret_cast<uintptr_t>(value));

	return 1;
}


} // namespace shdfnd
} // namespace physx
