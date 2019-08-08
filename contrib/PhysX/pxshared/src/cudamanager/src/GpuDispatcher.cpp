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

#include "task/PxTaskDefine.h"

#if PX_SUPPORT_GPU_PHYSX

#include "task/PxGpuDispatcher.h"
#include "task/PxCpuDispatcher.h"
#include "cudamanager/PxCudaContextManager.h"
#include "cudamanager/PxGpuCopyDesc.h"

#include "CudaKernelWrangler.h"
#include "GpuDispatcher.h"

#if PX_SUPPORT_PXTASK_PROFILING
#include "foundation/PxProfiler.h"
#endif

#include "PsArray.h"
#include "PsSync.h"
#include "PsInlineArray.h"
#include "PsUserAllocated.h"
#include "PsAtomic.h"

#if PX_VC
#pragma warning(disable: 4062)	//enumerator 'identifier' in switch of enum 'enumeration' is not handled
#endif

using namespace physx;

// hack to run CUDA calls in a single thread
// used to capture CUDA APIC traces
// #define SINGLE_CUDA_THREAD 1

/* Kernels in UtilsKernels.cu */
const char* UtilKernelNames[] =
{
	"Saturate",
	"MemCopyAsync",
	"MemCopyBatchedAsync",
};
enum
{
	KERNEL_SATURATE,
	KERNEL_MEMCOPY,
	KERNEL_MEMCOPY_BATCHED
};

/* ==================== LaunchTask =========================== */

/**
\brief A task that maintains a list of dependent tasks.

This task maintains a list of dependent tasks that have their reference counts 
reduced on completion of the task.

The refcount is incremented every time a dependent task is added.
*/
class physx::FanoutTask : public PxBaseTask, public shdfnd::UserAllocated
{
	PX_NOCOPY(FanoutTask)
public:
	FanoutTask(const char* name) : mRefCount(0), mName(name), mNotifySubmission(false) {}

	virtual void run() {}
	virtual const char* getName(void) const { return mName; }

	virtual void removeReference()
	{
		shdfnd::Mutex::ScopedLock lock(mMutex);
		if (!shdfnd::atomicDecrement(&mRefCount))
		{
			// prevents access to mReferencesToRemove until release
			shdfnd::atomicIncrement(&mRefCount);
			mNotifySubmission = false;
			PX_ASSERT(mReferencesToRemove.empty());
			// save continuation and dependents
			for (uint32_t i = 0; i < mDependents.size(); i++)
				mReferencesToRemove.pushBack(mDependents[i]);
			mDependents.clear();
			mTm->getCpuDispatcher()->submitTask(*this);
		}
	}

	/** 
	\brief Increases reference count
	*/
	virtual void addReference()
	{
		shdfnd::Mutex::ScopedLock lock(mMutex);
		shdfnd::atomicIncrement(&mRefCount);
		mNotifySubmission = true;
	}

	/**
	\brief Return the ref-count for this task 
	*/
	PX_INLINE int32_t getReference() const
	{
		return mRefCount;
	}

	/**
	Adds a dependent task. It also sets the task manager querying it from the dependent task.  
	The refcount is incremented every time a dependent task is added.
	*/
	void addDependent(PxBaseTask& dependent)
	{
		shdfnd::Mutex::ScopedLock lock(mMutex);
		shdfnd::atomicIncrement(&mRefCount);
		mTm = dependent.getTaskManager();
		mDependents.pushBack(&dependent);
		dependent.addReference();
		mNotifySubmission = true;
	}

	/**
	Reduces reference counts of the continuation task and the dependent tasks, also 
	clearing the copy of continuation and dependents task list.
	*/
	virtual void release()
	{
		shdfnd::InlineArray<PxBaseTask*, 10> referencesToRemove;

		{
			shdfnd::Mutex::ScopedLock lock(mMutex);

			const uint32_t contCount = mReferencesToRemove.size(); 
			referencesToRemove.reserve(contCount);
			for (uint32_t i=0; i < contCount; ++i)
				referencesToRemove.pushBack(mReferencesToRemove[i]);
				
			mReferencesToRemove.clear();
			// allow access to mReferencesToRemove again
			if (mNotifySubmission)
			{
				removeReference();
			}
			else
			{
				physx::shdfnd::atomicDecrement(&mRefCount);
			}

			// the scoped lock needs to get freed before the continuation tasks get (potentially) submitted because
			// those continuation tasks might trigger events that delete this task and corrupt the memory of the
			// mutex (for example, assume this task is a member of the scene then the submitted tasks cause the simulation 
			// to finish and then the scene gets released which in turn will delete this task. When this task then finally
			// continues the heap memory will be corrupted.
		}

		for (uint32_t i=0; i < referencesToRemove.size(); ++i)
			referencesToRemove[i]->removeReference();
	}

private:
	friend class LaunchTask;

	volatile int32_t mRefCount;
	const char* mName;
	shdfnd::InlineArray<PxBaseTask*, 4> mDependents;
	shdfnd::InlineArray<PxBaseTask*, 4> mReferencesToRemove;
	bool mNotifySubmission;
	shdfnd::Mutex mMutex; // guarding mDependents and mNotifySubmission
};

class physx::LaunchTask : public physx::FanoutTask
{
public:
	LaunchTask() : FanoutTask("GpuDispatcher.launch"), mIsBatchStarted(false)
	{
	}

	virtual void run() 
	{
		if (mReferencesToRemove.size() >= 1)
			mIsBatchStarted = true;
	}

	bool mIsBatchStarted;
};

class physx::BlockTask : public PxLightCpuTask, public shdfnd::UserAllocated
{
	PX_NOCOPY(BlockTask)
public:
	BlockTask(PxGpuWorkerThread* dispatcher, physx::LaunchTask* launchTask) 
		: mDispatcher(dispatcher), mLaunchTask(launchTask), mSyncTask(NULL)
	{}

	virtual const char* getName(void) const 
	{ 
		return "GpuDispatcher.block"; 
	}

	virtual void removeReference()
	{
		shdfnd::Mutex::ScopedLock lock(mMutex);
		if (!physx::shdfnd::atomicDecrement(&mRefCount))
		{
			// prevents access to mSyncTask until release
			physx::shdfnd::atomicIncrement(&mRefCount);
			PX_ASSERT(!mSyncTask);
			shdfnd::swap(mSyncTask, mCont);
			mTm->getCpuDispatcher()->submitTask(*this);
		}
	}

	virtual void run() 
	{
		if (mLaunchTask->mIsBatchStarted)
		{
			mDispatcher->mCtxMgr->acquireContext();
			CUevent stopEv = mDispatcher->mCachedBlockingEvents.get();
			CUstream stream = (CUstream)0;
			mSyncTask->addReference();
			mDispatcher->flushBatch(stopEv, stream, mSyncTask);
			mDispatcher->mCtxMgr->releaseContext();
			mLaunchTask->mIsBatchStarted = false;
		}
	}

	virtual void setContinuation(PxBaseTask* continuation) 
	{
		// this function is called multiple times, skip after first call
		shdfnd::Mutex::ScopedLock lock(mMutex);
		physx::shdfnd::atomicIncrement(&mRefCount);
		if (!mCont)
		{
			mCont = continuation;
			mTm = mCont->getTaskManager();
			mCont->addReference();
		}
	}

	virtual void release()
	{
		shdfnd::Mutex::ScopedLock lock(mMutex);
		mSyncTask->removeReference();
		mSyncTask = NULL;
		// allow access to mSyncTask again
		if (mCont)
		{
			removeReference();
		}
		else
		{
			physx::shdfnd::atomicDecrement(&mRefCount);
		}
	}

	PxGpuWorkerThread* mDispatcher;
	physx::LaunchTask* mLaunchTask;
	PxBaseTask* mSyncTask;
	shdfnd::Mutex mMutex; // guarding mCont
};

/* ==================== API functions =========================== */

void physx::releaseGpuDispatcher(PxGpuDispatcher& gd)
{
	GpuDispatcherImpl* impl = (GpuDispatcherImpl*) &gd;
	delete impl;
}

PxCudaContextManager* GpuDispatcherImpl::getCudaContextManager()
{
	return mDispatcher->mCtxMgr;
}

GpuDispatcherImpl::GpuDispatcherImpl(PxErrorCallback& errorCallback, PxCudaContextManager& ctx) :
	mDispatcher(NULL),
	mBlockingThread(NULL),
	mLaunchTask(NULL),
	mBlockTask(NULL),
	mSyncTask(NULL)
{
	mDispatcher = PX_NEW(PxGpuWorkerThread);
	if (!mDispatcher)
	{
		forceFailureMode();
		return;
	}

	mDispatcher->setCudaContext(ctx);

	mDispatcher->mUtilKernelWrapper = PX_NEW(KernelWrangler)(*this, errorCallback, UtilKernelNames, sizeof(UtilKernelNames) / sizeof(char*));
	if (!mDispatcher->mUtilKernelWrapper || mDispatcher->mUtilKernelWrapper->hadError())
	{
		forceFailureMode();
		return;
	}

	mLaunchTask = PX_NEW(LaunchTask);
	mBlockTask = PX_NEW(BlockTask)(mDispatcher, mLaunchTask);
	mSyncTask = PX_NEW(FanoutTask)("GpuDispatcher.sync");
}

GpuDispatcherImpl::~GpuDispatcherImpl()
{
	if (mBlockingThread)
	{
		mBlockingThread->signalQuit();
		PX_ASSERT(mDispatcher);
		mDispatcher->mRecordEventQueued.set();
		mBlockingThread->waitForQuit();
		delete mBlockingThread;
	}

	if (mDispatcher)
	{
		mDispatcher->signalQuit();
		mDispatcher->mInputReady.set();
		mDispatcher->waitForQuit();
		delete mDispatcher;
	}

	if (mLaunchTask)
		PX_DELETE(mLaunchTask);

	if (mBlockTask)
		PX_DELETE(mBlockTask);

	if (mSyncTask)
		PX_DELETE(mSyncTask);
}

void GpuDispatcherImpl::start()
{
#ifndef SINGLE_CUDA_THREAD
	mDispatcher->start(shdfnd::Thread::getDefaultStackSize());
#else
	mDispatcher->execute();
#endif

	mBlockingThread = PX_NEW(BlockingWaitThread)(*mDispatcher);
#ifndef SINGLE_CUDA_THREAD
	mBlockingThread->start(shdfnd::Thread::getDefaultStackSize());
#endif
}

void GpuDispatcherImpl::startSimulation()
{
	mDispatcher->startSimulation();
}

void GpuDispatcherImpl::stopSimulation()
{
	mDispatcher->stopSimulation();
}

void GpuDispatcherImpl::startGroup()
{
	shdfnd::atomicIncrement(&mDispatcher->mActiveGroups);
}

void GpuDispatcherImpl::submitTask(PxTask& task)
{
	mDispatcher->mSubmittedTaskList.push(&task);
}

void GpuDispatcherImpl::finishGroup()
{
	if (!shdfnd::atomicDecrement(&mDispatcher->mActiveGroups))
	{
#ifdef SINGLE_CUDA_THREAD
		mDispatcher->mCtxMgr->acquireContext();
		mDispatcher->processActiveTasks();
		mDispatcher->mCtxMgr->releaseContext();
		mDispatcher->blockingWaitFunc();
#endif
		mDispatcher->mInputReady.set();
	}
}

void GpuDispatcherImpl::addCompletionPrereq(PxBaseTask& task)
{
	mDispatcher->addCompletionPrereq(task);
}

bool GpuDispatcherImpl::failureDetected() const
{
	return mDispatcher->mFailureDetected;
}

void GpuDispatcherImpl::forceFailureMode()
{
	mDispatcher->mFailureDetected = true;
}

void GpuDispatcherImpl::launchCopyKernel(PxGpuCopyDesc* desc, uint32_t count, CUstream stream)
{
	mDispatcher->launchCopyKernel(desc, count, stream);
}

PxBaseTask& GpuDispatcherImpl::getPreLaunchTask()
{
	PX_ASSERT(mLaunchTask);
	return *mLaunchTask;
}

void GpuDispatcherImpl::addPreLaunchDependent(PxBaseTask& dependent)
{
	PX_ASSERT(mLaunchTask);
	mLaunchTask->addDependent(dependent);
}

PxBaseTask& GpuDispatcherImpl::getPostLaunchTask()
{
	PX_ASSERT(mBlockTask);
	return *mBlockTask;
}

void GpuDispatcherImpl::addPostLaunchDependent(PxBaseTask& dependent)
{
	PX_ASSERT(mSyncTask && mBlockTask);
	mSyncTask->addDependent(dependent);
	mBlockTask->setContinuation(mSyncTask);
	mSyncTask->removeReference();
}

/* ==================== Worker Thread =========================== */

PxGpuWorkerThread::PxGpuWorkerThread()
	: mActiveGroups(0)
	, mCtxMgr(NULL)
	, mFailureDetected(false)
	, mCompletionRingPush(0)
	, mCompletionRingPop(0)
	, mCachedBlockingEvents(CU_EVENT_BLOCKING_SYNC)
	, mCachedNonBlockingEvents(0)
	, mCountActiveScenes(0)
	, mSmStartTimes(0)
	, mUtilKernelWrapper(0)
{
}

void PxGpuWorkerThread::setCudaContext(PxCudaContextManager& ctx)
{
	mCtxMgr = &ctx;
}

PxGpuWorkerThread::~PxGpuWorkerThread()
{
	mCtxMgr->acquireContext();
	mCachedBlockingEvents.clear();
	mCachedNonBlockingEvents.clear();
	mCachedStreams.reset();
	while (!mCachedStreams.empty())
	{
		GD_CHECK_CALL(cuStreamDestroy(mCachedStreams.get(mCachedStreams.popBack())));
	}
	mCtxMgr->releaseContext();

	if (mSmStartTimes)
	{
		PX_FREE(mSmStartTimes);
	}
	if (mUtilKernelWrapper)
	{
		// will acquire the context for itself
		PX_DELETE(mUtilKernelWrapper);
	}
}

void PxGpuWorkerThread::emitStartEvent(const char *id)
{
	PX_UNUSED(id);
#if PX_SUPPORT_PXTASK_PROFILING
	PX_PROFILE_START_CROSSTHREAD(id,0);
#endif
}

void PxGpuWorkerThread::emitStopEvent(const char *id)
{
	PX_UNUSED(id);
#if PX_SUPPORT_PXTASK_PROFILING
	PX_PROFILE_STOP_CROSSTHREAD(id,0);
#endif
}

/* A TaskManager is informing us that its simulation is being stepped */
void PxGpuWorkerThread::startSimulation()
{
	mUsingConcurrentStreams = mCtxMgr->getUsingConcurrentStreams();

	shdfnd::atomicIncrement(&mCountActiveScenes);
}


/* A TaskManager is informing us that its simulation has ended */
void PxGpuWorkerThread::stopSimulation()
{
	if (shdfnd::atomicDecrement(&mCountActiveScenes) == 0)
		mCachedStreams.reset();
}


/* PxGpuDispatcher worker thread main loop */
void PxGpuWorkerThread::execute()
{
	setName("GpuDispatcher.Worker");

#ifndef SINGLE_CUDA_THREAD
	bool running = true;
	while (running)
	{
		mInputReady.wait();

		if (quitIsSignalled())
		{
			break;
		}

		if (!mSubmittedTaskList.empty())
		{
			mCtxMgr->acquireContext();
			processActiveTasks();
			mCtxMgr->releaseContext();
		}
	}

	quit();
#endif
}

/* Expected to be called by a GPU task, or a function called by a GPU
 * task.  The context is important because I think it does not require
 * any locking since only one GPU task's launchInstance() function will
 * be running at a time, per GpuDispatcherImpl (per CUDA context).
 */
void PxGpuWorkerThread::addCompletionPrereq(PxBaseTask& task)
{
	if (mFailureDetected)
	{
		return;
	}

	emitStartEvent("GpuDispatcher.AddCompletionEvent");
	task.addReference();
	mCompletionTasks.pushBack(&task);
	emitStopEvent("GpuDispatcher.AddCompletionEvent");
}

namespace
{
	template <typename T0>
	PX_NOINLINE CUresult launchKernelGrid(CUfunction func, unsigned int gridWidth, unsigned int gridHeight, unsigned int numThreads, unsigned int sharedMem, CUstream stream, T0 v0)
	{
		void* kernelParams[] =
		{
			&v0,
		};
		return cuLaunchKernel(func, gridWidth, gridHeight, 1, numThreads, 1, 1, sharedMem, stream, kernelParams, NULL);
	}
}

void PxGpuWorkerThread::launchCopyKernel(PxGpuCopyDesc* desc, uint32_t count, CUstream stream)
{
	if (!mCtxMgr->canMapHostMemory())
	{
		for (uint32_t i = 0 ; i < count ; i++)
		{
			PX_ASSERT(desc->isValid());
			switch (desc->type)
			{
			case PxGpuCopyDesc::DeviceMemset32:
				GD_CHECK_CALL(cuMemsetD32Async(desc->dest, (uint32_t) desc->source, desc->bytes >> 2, stream));
				break;
			case PxGpuCopyDesc::DeviceToDevice:
				GD_CHECK_CALL(cuMemcpyDtoDAsync(desc->dest, desc->source, desc->bytes, stream));
				break;
			case PxGpuCopyDesc::DeviceToHost:
				GD_CHECK_CALL(cuMemcpyDtoHAsync((void*) desc->dest, desc->source, desc->bytes, stream));
				break;
			case PxGpuCopyDesc::HostToDevice:
				GD_CHECK_CALL(cuMemcpyHtoDAsync(desc->dest, (void*) desc->source, desc->bytes, stream));
				break;
			}

			desc++;
		}
	}
	else if (count == 1)
	{
		CUfunction func = mUtilKernelWrapper->getCuFunction(KERNEL_MEMCOPY);
		uint32_t smCount = (uint32_t)mCtxMgr->getMultiprocessorCount();

		PX_ASSERT(desc->isValid());

		CUdeviceptr dptr;
		switch (desc->type)
		{
		case PxGpuCopyDesc::DeviceToHost:
			dptr = mCtxMgr->getMemoryManager()->getMappedPinnedPtr(PxCudaBufferPtr(desc->dest));
			desc->dest = dptr;
			break;
		case PxGpuCopyDesc::HostToDevice:
			dptr = mCtxMgr->getMemoryManager()->getMappedPinnedPtr(PxCudaBufferPtr(desc->source));
			desc->source = dptr;
			break;
		case PxGpuCopyDesc::DeviceMemset32:
		case PxGpuCopyDesc::DeviceToDevice:
			//do nothing, cases are here for GCCs warning system
			break;
		}

		uint32_t numThreads;
		if (mCtxMgr->supportsArchSM20())
		{
			numThreads = 256;
		}
		else
		{
			numThreads = 128;
		}
		uint32_t blocks = uint32_t(desc->bytes / (numThreads * 4 * 6));
		if (blocks == 0)
		{
			blocks = 1;
		}
		if (blocks > smCount)
		{
			blocks = smCount;
		}

		GD_CHECK_CALL(
			launchKernel(func, blocks, numThreads, 0, stream, *desc)
			);
	}
	else
	{
		CUfunction func = mUtilKernelWrapper->getCuFunction(KERNEL_MEMCOPY_BATCHED);
		CUdeviceptr dptr;

		for (uint32_t i = 0 ; i < count ; i++)
		{
			PX_ASSERT(desc[i].isValid());

			switch (desc[i].type)
			{
			case PxGpuCopyDesc::DeviceToHost:
				dptr = mCtxMgr->getMemoryManager()->getMappedPinnedPtr(PxCudaBufferPtr(desc[i].dest));
				desc[i].dest = dptr;
				break;
			case PxGpuCopyDesc::HostToDevice:
				dptr = mCtxMgr->getMemoryManager()->getMappedPinnedPtr(PxCudaBufferPtr(desc[i].source));
				desc[i].source = dptr;
				break;
			case PxGpuCopyDesc::DeviceMemset32:
			case PxGpuCopyDesc::DeviceToDevice:
				//do nothing, cases are here for GCCs warning system
				break;
			}
		}

		uint32_t numThreads, numBlocksX;
		if (mCtxMgr->supportsArchSM20())
		{
			numThreads = 256;
			numBlocksX = 1;
		}
		else
		{
			numThreads = 128;
			numBlocksX = 2;
		}

		dptr = mCtxMgr->getMemoryManager()->getMappedPinnedPtr(PxCudaBufferPtr(desc));

		GD_CHECK_CALL(
			launchKernelGrid(func, numBlocksX, count, numThreads, 0, stream, 
				dptr)
			);
	}
}

void PxGpuWorkerThread::flushBatch(CUevent endEvent, CUstream syncStream, PxBaseTask* task)
{
	shdfnd::Mutex::ScopedLock lock(mMutex);

	if (mFailureDetected)
	{
		return;
	}

	if (endEvent && (1 & intptr_t(syncStream)))
	{
		// blocking record event on stream 0, flushes current push buffer
		GD_CHECK_CALL(cuEventRecord(endEvent, 0));
	} else {
		CUresult ret = cuStreamQuery(0); // flushes current push buffer
		PX_ASSERT(ret == CUDA_SUCCESS || ret == CUDA_ERROR_NOT_READY);
		PX_UNUSED(ret);
	}
	
	int next = (mCompletionRingPush + 1) % SIZE_COMPLETION_RING;
	while (next == mCompletionRingPop)
	{
		// lordy, I hope we never get here.
		PX_ALWAYS_ASSERT();
		mCtxMgr->releaseContext();
		sleep(1);
		mCtxMgr->acquireContext();
	}

	CudaBatch& b = mCompletionRing[ mCompletionRingPush ];
	b.blockingEvent = endEvent;
	b.blockingStream = syncStream;
	b.continuationTask = task;

	mCompletionRingPush = next;
	mRecordEventQueued.set();
}

// Take any submitted tasks into its appropriate read list
void PxGpuWorkerThread::pollSubmitted(shdfnd::Array<ReadyTask>* ready)
{
	mInputReady.reset();
	PxGpuTask* gt;
	while ((gt = (PxGpuTask*) mSubmittedTaskList.popBack()) != 0)
	{
		PxGpuTaskHint::Enum type = gt->getTaskHint();
		ReadyTask* r = &ready[ type ].insert();
		r->task = gt;
		r->iteration = 0;
		if (mUsingConcurrentStreams && gt->mStreamIndex == 0)
		{
			gt->mStreamIndex = mCachedStreams.popBack();
		}
	}
}

void PxGpuWorkerThread::processActiveTasks()
{
	emitStartEvent("GpuDispatcher.ProcessTasksEvent");
	
	if (mFailureDetected)
	{
		while (!mSubmittedTaskList.empty())
		{
			mInputReady.reset();
			mSubmittedTaskList.popBack()->release();
		}
		emitStopEvent("GpuDispatcher.ProcessTasksEvent");
		return;
	}

	for (uint32_t i = 0; i < PxGpuTaskHint::NUM_GPU_TASK_HINTS; i++)
	{
		mReady[i].clear();
	}

	//OutputDebugString("batch beginning\n");

	const CUevent nonBlockEv = mCachedNonBlockingEvents.get();
	bool workToFlush = false;
	bool tasksRemain = false;
	PxGpuTaskHint::Enum curMode = PxGpuTaskHint::HostToDevice;

	pollSubmitted(mReady);

	do
	{
		// cycle current run mode when necessary
		if (mReady[ curMode ].size() == 0)
		{
			if (curMode == PxGpuTaskHint::HostToDevice)
			{
				curMode = PxGpuTaskHint::Kernel;
			}
			else if (curMode == PxGpuTaskHint::Kernel)
			{
				curMode = PxGpuTaskHint::DeviceToHost;
			}
			else
			{
				curMode = PxGpuTaskHint::HostToDevice;
			}
		}

		uint32_t singleStream = mReady[curMode].empty() ? 0 : mReady[curMode].front().task->mStreamIndex;

		while (mReady[ curMode ].size())
		{
			bool needwfi = false;
			for (uint32_t i = 0 ; i < mReady[ curMode ].size() ; i++)
			{
				ReadyTask& r = mReady[ curMode ][ i ];

				if (r.task->mPreSyncRequired)
				{
					// If mPreSyncRequired is set *before* the task is run, it implies
					// a WFI must be inserted before this task issues any work.  Multiple
					// ready tasks may have this flag, so to avoid inserting multiple WFI
					// requests, we skip marked tasks in this pass and note a WFI is needed.
					needwfi = true;
					r.task->mPreSyncRequired = false;
				}
				else
				{
					const CUstream s = (r.task->mStreamIndex > 0) ? mCachedStreams.get(r.task->mStreamIndex) : 0;
#if PX_PROFILE
					r.task->mTm->emitStartEvent(*r.task);
#endif
					bool active = r.task->launchInstance(s, int(r.iteration++));
#if PX_PROFILE
					r.task->mTm->emitStopEvent(*r.task);
#endif
					if(singleStream != r.task->mStreamIndex)
						singleStream = 0;

					// If the launchInstance() call reported a non-recoverable error, gracefully
					// release all scheduled tasks
					if (mFailureDetected)
					{
						// Release all ready tasks
						for (uint32_t h = 0; h < PxGpuTaskHint::NUM_GPU_TASK_HINTS; h++)
						{
							for (uint32_t j = 0 ; j < mReady[ h ].size() ; j++)
							{
								mReady[ h ][ j ].task->release();
							}
							mReady[ h ].clear();
						}

						// Release all submitted tasks, until idle
						while (!mSubmittedTaskList.empty())
						{
							mInputReady.reset();
							mSubmittedTaskList.popBack()->release();
						}
						return;
					}

					workToFlush = true;
					if (r.task->mPreSyncRequired)
					{
						// This task has asked for a sync point, meaning it has launched a copy
						// or a kernel that must be completed before any later tasks are allowed
						// to start.  Insert a WFI and clear the needwfi flag
						GD_CHECK_CALL(cuEventRecord(nonBlockEv, 0));
						needwfi = false;
						r.task->mPreSyncRequired = false;
					}

					if (!active)
					{
						r.task->release();
						mReady[ curMode ].replaceWithLast(i);
						pollSubmitted(mReady);
						i -= 1;
					}
				}
			}

			if (needwfi)
			{
				GD_CHECK_CALL(cuEventRecord(nonBlockEv, 0));
			}
		}

		/* We have completed one of the three phases */

		tasksRemain = false;
		for (int e = (int) PxGpuTaskHint::HostToDevice ; e != (int) PxGpuTaskHint::NUM_GPU_TASK_HINTS ; e++)
		{
			tasksRemain |= (mReady[ e ].size() != 0);
		}

		if (!mCompletionTasks.empty())
		{
			workToFlush = true;
		}

		if (workToFlush && (tasksRemain == false || curMode == PxGpuTaskHint::DeviceToHost))
		{
			//OutputDebugString("batch ending\n");

			while (mCompletionTasks.size())
			{
				PxBaseTask* t = mCompletionTasks.popBack();
				if (workToFlush)
				{
					CUevent stopEv = mCachedBlockingEvents.get();
					CUstream stream = singleStream ? mCachedStreams.get(singleStream) : (CUstream)1;
					flushBatch(stopEv, stream, t);
					workToFlush = false;
				}
				else
				{
					flushBatch(0, 0, t);
				}
			}
			if (workToFlush)
			{
				/* Getting here is probably an indication of a bug in your task graph,
				 * but it is possible to get this warning if you have CPU tasks that
				 * can delay GpuTasks.  So, consider this warning "training wheels" and
				 * disable it if you know your graph is correct.
				 */
				// SJB - Disabling this warning, APEX does this every frame because
				// of how BasicIOS and IOFX interact.
				//shdfnd::getFoundation().error(PX_WARN,
				//	"CUDA work generated without a completion dependency!");
				CUevent stopEv = mCachedBlockingEvents.get();
				flushBatch(stopEv, (CUstream)1, NULL);
			}
		}
	}
	while (tasksRemain);

	mCachedNonBlockingEvents.add(nonBlockEv);

	emitStopEvent("GpuDispatcher.ProcessTasksEvent");
}

#endif
