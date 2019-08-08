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

#include "task/PxTask.h"
#include "task/PxGpuTask.h"
#include "cudamanager/PxCudaContextManager.h"

#include "PsString.h"

#if PX_SUPPORT_PXTASK_PROFILING
#include "foundation/PxFoundation.h"
#include "foundation/PxProfiler.h"
#endif

#include "GpuDispatcher.h"

using namespace physx;

/* Blocking thread / GPU Profiling Event Code */

void PxGpuWorkerThread::blockingWaitFunc()
{
	mCtxMgr->acquireContext();

	while (mCompletionRingPop != mCompletionRingPush)
	{
		CudaBatch& b = mCompletionRing[ mCompletionRingPop ];
		PxBaseTask* t = b.continuationTask;

		if (!b.blockingEvent)
		{
			PX_ASSERT(b.continuationTask != 0);

			/* No blocking necessary, just allow continuation task to run */
		}
		else if (!mFailureDetected)
		{
			emitStartEvent("GpuDispatcher.BlockingWaitEvent");

			if (1 & ~intptr_t(b.blockingStream))
			{
				GD_CHECK_CALL(cuStreamSynchronize(b.blockingStream));
			}
			else
			{
				GD_CHECK_CALL(cuEventSynchronize(b.blockingEvent));
			}

			emitStopEvent("GpuDispatcher.BlockingWaitEvent");
		}

		if (b.blockingEvent)
		{
			mCachedBlockingEvents.add(b.blockingEvent);
		}
		if (t)
		{
			t->removeReference();
		}
		mCompletionRingPop = (mCompletionRingPop + 1) % SIZE_COMPLETION_RING;
	}

	mCtxMgr->releaseContext();
}


/* Blocking wait thread

   All this thread does is block waiting for CUDA Record Events to
   be signaled.
 */

void BlockingWaitThread::execute()
{
	setName("GpuDispatcher.BlockingWait");
	bool running = true;
	while (running)
	{
		mWorker.mRecordEventQueued.wait();
		if (quitIsSignalled())
		{
			running = false;
		}

		mWorker.mRecordEventQueued.reset();
		mWorker.blockingWaitFunc();
	}
	quit();
}

#endif

