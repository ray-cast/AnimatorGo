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
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.

#include "CuSolver.h"
#include "CuCloth.h"
#include "../ClothImpl.h"
#include "CuFabric.h"
#include "CuFactory.h"
#include "CuSolverKernel.h"
#include "CuContextLock.h"
#include "CuCheckSuccess.h"
#include "../IterationState.h"
#include "../Ps/PsSort.h"
#include <foundation/PxProfiler.h>

#if NV_NVTX
#include "nvToolsExt.h"
#endif

#define NV_CUPTI 0

#if NV_CUPTI
#pragma warning(disable : 4324)
#include "cupti_activity.h"
#include "cupti_metrics.h"
#include "cupti_driver_cbid.h"
#include <cstdio>

namespace
{
void CUPTIAPI bufferRequested(uint8_t** buffer, size_t* size, size_t* maxNumRecords)
{
	*buffer = (uint8_t*)PX_ALIGNED16_ALLOC(*size = 32 * 1024 * 1024);
	*maxNumRecords = 0;
}

void CUPTIAPI bufferCompleted(CUcontext context, uint32_t streamId, uint8_t* buffer, size_t /*size*/, size_t validSize)
{
	CUpti_Activity* record = NULL;
	uint64_t totalTime = 0, numRecords = 0;
	while (CUPTI_SUCCESS == cuptiActivityGetNextRecord(buffer, validSize, &record))
	{
		if (record->kind != CUPTI_ACTIVITY_KIND_KERNEL)
			continue;

		CUpti_ActivityKernel3* kernel = (CUpti_ActivityKernel3*)record;
		if (strcmp(kernel->name, cloth::getKernelFunctionName()))
			continue;

		totalTime += kernel->end - kernel->start;
		++numRecords;
	}

	if (numRecords)
	{
		printf("%u kernel records, average runtime is %u ns\n", unsigned(numRecords), unsigned(totalTime / numRecords));
	}

	size_t dropped;
	cuptiActivityGetNumDroppedRecords(context, streamId, &dropped);
	if (dropped)
	{
		printf("Dropped %u activity records\n", unsigned(dropped));
	}

	PX_ALIGNED16_FREE(buffer);
}

struct CuptiEventProfiler
{
	CuptiEventProfiler() : mActiveCycles(0), mNumEvents(0)
	{
		CUdevice device = 0;
		cuCtxGetDevice(&device);
		CUcontext context = 0;
		cuCtxGetCurrent(&context);
		cuptiEventGetIdFromName(device, "active_cycles", &mEventId);
		cuptiEventGroupCreate(context, &mEventGroup, 0);
		cuptiEventGroupAddEvent(mEventGroup, mEventId);
		cuptiSubscribe(&mSubscriber, eventCallback, this);
		cuptiEnableCallback(1, mSubscriber, CUPTI_CB_DOMAIN_DRIVER_API, CUPTI_DRIVER_TRACE_CBID_cuLaunchKernel);
	}

	~CuptiEventProfiler()
	{
		cuptiUnsubscribe(mSubscriber);
		cuptiEventGroupRemoveEvent(mEventGroup, mEventId);
		cuptiEventGroupDestroy(mEventGroup);
		if (mNumEvents)
		{
			printf("%u kernel events, average active cycles is %u\n", unsigned(mNumEvents),
			       unsigned(mActiveCycles / mNumEvents));
		}
	}

	static void CUPTIAPI
	eventCallback(void* profiler, CUpti_CallbackDomain domain, CUpti_CallbackId cbid, const void* cbInfo)
	{
		// This callback is enabled only for launch so we shouldn't see anything else.
		NV_CLOTH_ASSERT(domain == CUPTI_CB_DOMAIN_DRIVER_API);
		NV_CLOTH_ASSERT(cbid == CUPTI_DRIVER_TRACE_CBID_cuLaunchKernel);

		reinterpret_cast<CuptiEventProfiler*>(profiler)
		    ->eventCallback(reinterpret_cast<const CUpti_CallbackData*>(cbInfo));
	}

	void eventCallback(const CUpti_CallbackData* cbInfo)
	{
		// on entry, enable all the event groups being collected this pass,
		// for metrics we collect for all instances of the event
		if (cbInfo->callbackSite == CUPTI_API_ENTER)
		{
			cuCtxSynchronize();
			cuptiSetEventCollectionMode(cbInfo->context, CUPTI_EVENT_COLLECTION_MODE_KERNEL);
			cuptiEventGroupEnable(mEventGroup);
		}

		// on exit, read and record event values
		if (cbInfo->callbackSite == CUPTI_API_EXIT)
		{
			cuCtxSynchronize();
			uint64_t activeCycles = 0;
			size_t bytesRead = sizeof(activeCycles);
			cuptiEventGroupReadEvent(mEventGroup, CUPTI_EVENT_READ_FLAG_NONE, mEventId, &bytesRead, &activeCycles);
			cuptiEventGroupDisable(mEventGroup);
			mActiveCycles += activeCycles;
			++mNumEvents;
		}
	}

	CUpti_SubscriberHandle mSubscriber;
	CUpti_EventGroup mEventGroup;
	CUpti_EventID mEventId;
	uint64_t mActiveCycles;
	uint64_t mNumEvents;
};
}
#endif

using namespace nv;
using namespace physx;

const char* cloth::getKernelFunctionName()
{
	return "simulateCloths";
}

namespace
{
const char* gKernelNames[] = { cloth::getKernelFunctionName(), };

// Note: gCuProfileZoneNames has a corresponding enum list (CuProfileZoneIds) in CuSolverKernel.h.
// Additions/deletions to gCuProfileZoneNames requires a similar action to CuProfileZoneIds.
const char* gCuProfileZoneNames[] = {
	"cloth::CuSolverKernel::simulateKernel",         "cloth::CuSolverKernel::integrateParticles",
	"cloth::CuSolverKernel::accelerateParticles",    "cloth::CuSolverKernel::applyWind",
	"cloth::CuSolverKernel::constrainTether",        "cloth::CuSolverKernel::solveFabric",
	"cloth::CuSolverKernel::constrainMotion",        "cloth::CuSolverKernel::constrainSeparation",
	"cloth::CuSolverKernel::collideParticles",       "cloth::CuSolverKernel::selfCollideParticles",
	"cloth::CuSolverKernel::updateSleepState",       "cloth::CuSolverKernel::simulateShared",
	"cloth::CuSolverKernel::simulateStreamed",       "cloth::CuSolverKernel::simulateGlobal",
	"cloth::CuSolverKernel::solveConstraintSet",     "cloth::CuCollision::buildAccleration",
	"cloth::CuCollision::collideCapsules",           "cloth::CuCollision::collideVirtualCapsules",
	"cloth::CuCollision::collideContinuousCapsules", "cloth::CuCollision::collideConvexes",
	"cloth::CuCollision::collideTriangles",          "cloth::CuSelfCollision::buildAccleration",
	"cloth::CuSelfCollision::collideParticles",
};
}

namespace
{
template <typename T>
struct CuDeviceAllocator
{
	CuDeviceAllocator(CUcontext ctx) : mManager(ctx)
	{
	}

	T* allocate(size_t n)
	{
		CUdeviceptr result;
		checkSuccess(cuMemAlloc(&result, n * sizeof(T)));
		return reinterpret_cast<T*>(result);
	}

	void deallocate(T* ptr)
	{
		checkSuccess(cuMemFree(reinterpret_cast<CUdeviceptr>(ptr)));
	}

	CUcontext mManager;
};
}

cloth::CuSolver::CuSolver(CuFactory& factory)
: CuContextLock(factory)
, mFactory(factory)
, mClothData(mFactory.mContext)
, mClothDataHostCopy(mFactory.mContext)
, mClothDataDirty(false)
, mFrameData(mFactory.mContext)
, mIterationData(mFactory.mContext)
, mIterationDataBegin(0)
, mFrameDt(0.0f)
, mSharedMemorySize(0)
, mSharedMemoryLimit(0)
, mStream(0)
, mKernelFunction(0)
, mKernelSharedMemorySize(0)
, mClothIndex(CuDeviceAllocator<uint32_t>(mFactory.mContext).allocate(1))
, mInterCollisionDistance(0.0f)
, mInterCollisionStiffness(1.0f)
, mInterCollisionIterations(1)
, mInterCollisionFilter(nullptr)
, mInterCollisionScratchMem(NULL)
, mInterCollisionScratchMemSize(0)
, mSimulateNvtxRangeId(0)
, mProfileBuffer(0)
, mProfileBaseId(0)
, mCudaError(false)
{
	mFactory.mSolverCount++;

	NV_CLOTH_ASSERT(CuProfileZoneIds::NUMZONES == PX_ARRAY_SIZE(gCuProfileZoneNames));

	if (mCudaError)
	{
		CuContextLock::release();
		return;
	}

	checkSuccess(cuStreamCreate(&mStream, 0));
	checkSuccess(cuModuleGetFunction(&mKernelFunction, mFactory.mKernelModule, getKernelFunctionName()));

	// get amount of statically allocated shared memory
	checkSuccess(cuFuncGetAttribute(&mKernelSharedMemorySize, CU_FUNC_ATTRIBUTE_SHARED_SIZE_BYTES, mKernelFunction));

	// initialize cloth index
	checkSuccess(cuMemsetD32(mClothIndex.dev(), 0, 1));

#if PX_CUPTI
	// activity (measure kernel runtime in ns)
	CUcontext context = 0;
	cuCtxGetCurrent(&context);
	cuptiActivityEnableContext(context, CUPTI_ACTIVITY_KIND_KERNEL);
	cuptiActivityRegisterCallbacks(bufferRequested, bufferCompleted);
	// event (measure kernel active cycles)
	mCuptiEventProfiler = NV_CLOTH_NEW(CuptiEventProfiler);
#endif

	CuContextLock::release();

	mSimulateProfileEventData = nullptr;
}

cloth::CuSolver::~CuSolver()
{
	NV_CLOTH_ASSERT(mCloths.empty());

	CuContextLock::acquire();

#if PX_CUPTI
	cuptiActivityFlushAll(0);
	cuptiActivityDisable(CUPTI_ACTIVITY_KIND_KERNEL);
	NV_CLOTH_DELETE((CuptiEventProfiler*)mCuptiEventProfiler);
#endif

	CuDeviceAllocator<uint32_t>(mFactory.mContext).deallocate(mClothIndex.get());

	if (mStream)
		checkSuccess(cuStreamDestroy(mStream));

	if (mInterCollisionScratchMem)
		NV_CLOTH_FREE(mInterCollisionScratchMem);

	mFactory.mSolverCount--;
}

void cloth::CuSolver::addClothAppend(Cloth* cloth)
{
	CuCloth& cuCloth = *static_cast<CuCloth*>(cloth);

	NV_CLOTH_ASSERT(mCloths.find(&cuCloth) == mCloths.end());

	mCloths.pushBack(&cuCloth);
	// trigger update of mClothData array
	cuCloth.notifyChanged();
}

void cloth::CuSolver::addClothUpdateData()
{
	CuContextLock contextLock(mFactory);

	// resize containers and update kernel data
	mClothDataHostCopy.resize(mCloths.size());
	mClothData.resize(mCloths.size());
	mFrameData.resize(mCloths.size());
	updateKernelData();
}

void cloth::CuSolver::updateKernelData()
{
	mKernelDataHost.mClothIndex = mClothIndex.get();
	mKernelDataHost.mClothData = mClothData.begin().get();
	mKernelDataHost.mFrameData = getDevicePointer(mFrameData);

	mKernelDataHost.mProfileBuffer = mProfileBuffer;
	mKernelDataHost.mProfileBaseId = mProfileBaseId;

}

namespace
{
struct ClothSimCostGreater
{
	bool operator()(const cloth::CuCloth* left, const cloth::CuCloth* right) const
	{
		return left->mNumParticles * left->mSolverFrequency > right->mNumParticles * right->mSolverFrequency;
	}
};
}

void cloth::CuSolver::addCloth(Cloth* cloth)
{
	addClothAppend(cloth);
	addClothUpdateData();
}

void cloth::CuSolver::addCloths(Range<Cloth*> cloths)
{
	for (uint32_t i = 0; i < cloths.size(); ++i)
	{
		addClothAppend(*(cloths.begin() + i));
	}
	addClothUpdateData();
}

void cloth::CuSolver::removeCloth(Cloth* cloth)
{
	CuCloth& cuCloth = static_cast<CuCloth&>(*cloth);

	ClothVector::Iterator begin = mCloths.begin(), end = mCloths.end();
	ClothVector::Iterator it = mCloths.find(&cuCloth);

	if (it == end)
		return; // not found

	uint32_t index = uint32_t(it - begin);

	mCloths.remove(index);
	mClothDataHostCopy.remove(index);
	mClothData.resize(mCloths.size());
	mClothDataDirty = true;
}

int cloth::CuSolver::getNumCloths() const
{
	return mCloths.size();
}
cloth::Cloth * const * cloth::CuSolver::getClothList() const
{
	if(getNumCloths())
		return reinterpret_cast<Cloth* const*>(&mCloths[0]);
	else
		return nullptr;
}

bool cloth::CuSolver::beginSimulation(float dt)
{
	if (mCloths.empty())
		return false;
	mFrameDt = dt;
	beginFrame();
	return true;
}

void cloth::CuSolver::simulateChunk(int idx)
{
	PX_UNUSED(idx);
	NV_CLOTH_ASSERT(!mCloths.empty());
	NV_CLOTH_ASSERT(idx == 0);
	CuSolver::executeKernel();
}

void cloth::CuSolver::endSimulation()
{
	NV_CLOTH_ASSERT(!mCloths.empty());
	CuSolver::endFrame();
}

int cloth::CuSolver::getSimulationChunkCount() const
{
	// 0 chunks when no cloth present in the solver, 1 otherwise
	return getNumCloths() != 0;
}

void cloth::CuSolver::beginFrame()
{
	CuContextLock contextLock(mFactory);

	mSimulateProfileEventData = NV_CLOTH_PROFILE_START_CROSSTHREAD("cloth::CuSolver::simulate", 0);

	CuIterationData* iterationDataBegin = mIterationData.empty() ? 0 : &mIterationData.front();

	mFrameData.resize(0);
	mIterationData.resize(0);

	// update cloth data
	ClothVector::Iterator cIt, cEnd = mCloths.end();
	CuHostVector<CuClothData>::Type::Iterator dIt = mClothDataHostCopy.begin();
	for (cIt = mCloths.begin(); cIt != cEnd; ++cIt, ++dIt)
		mClothDataDirty |= (*cIt)->updateClothData(*dIt);

	if (mClothDataDirty)
	{
		/* find optimal number of cloths per SM */

		CUdevice device = 0;
		checkSuccess(cuCtxGetDevice(&device));
		int numSMs = 0;
		checkSuccess(cuDeviceGetAttribute(&numSMs, CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, device));
		int sharedMemoryPerBlock = 0;
		checkSuccess(cuDeviceGetAttribute(&sharedMemoryPerBlock, CU_DEVICE_ATTRIBUTE_SHARED_MEMORY_PER_BLOCK, device));

		// at least 192 threads per block (e.g. CuCollision::buildAcceleration)
		uint32_t maxClothsPerSM = std::min(mFactory.mMaxThreadsPerBlock / 192, uint32_t(mCloths.size() + numSMs - 1) / numSMs);

		// tuning parameters: relative performance per numSharedPositions
		float weights[3] = { 0.4f, 0.8f, 1.0f }; //TODO check if these are the newest weights (APEX has different values)

		// try all possible number of cloths per SM and estimate performance
		float maxWeightSum = 0.0f;
		uint32_t numClothsPerSM = 0;
		for (uint32_t i = 1; i <= maxClothsPerSM; ++i)
		{
			uint32_t sharedMemoryLimit = (sharedMemoryPerBlock / i) - mKernelSharedMemorySize;

			float weightSum = 0.0f;
			for (cIt = mCloths.begin(); cIt != cEnd; ++cIt)
			{
				uint32_t sharedMemorySize = (*cIt)->mSharedMemorySize;
				uint32_t positionsSize = (*cIt)->mNumParticles * sizeof(PxVec4);

				if (sharedMemorySize > sharedMemoryLimit)
					break;

				uint32_t numSharedPositions = std::min(2u, (sharedMemoryLimit - sharedMemorySize) / positionsSize);

				weightSum += weights[numSharedPositions] * positionsSize;
			}
			// tuning parameter: inverse performance for running i cloths per SM
			weightSum *= 2.0f + i;

			if (cIt == cEnd && weightSum > maxWeightSum)
			{
				maxWeightSum = weightSum;
				numClothsPerSM = i;
			}
		}
		NV_CLOTH_ASSERT(numClothsPerSM);

		// update block size
		uint32_t numThreadsPerBlock = mFactory.mMaxThreadsPerBlock / numClothsPerSM & ~31;
		if (mFactory.mNumThreadsPerBlock != numThreadsPerBlock)
		{
			mFactory.mNumThreadsPerBlock = numThreadsPerBlock;
			//checkSuccess(
			//    cuFuncSetBlockShape(mKernelFunction, int(mFactory.mNumThreadsPerBlock = numThreadsPerBlock), 1, 1));
		}

		// remember num cloths per SM in terms of max shared memory per block
		mSharedMemoryLimit = (sharedMemoryPerBlock / numClothsPerSM) - mKernelSharedMemorySize;
	}

	uint32_t maxSharedMemorySize = 0;
	for (cIt = mCloths.begin(); cIt != cEnd; ++cIt)
	{
		CuCloth& cloth = **cIt;

		uint32_t sharedMemorySize = cloth.mSharedMemorySize;
		uint32_t positionsSize = cloth.mNumParticles * sizeof(PxVec4);

		uint32_t numSharedPositions = std::min(2u, (mSharedMemoryLimit - sharedMemorySize) / positionsSize);

		maxSharedMemorySize = std::max(maxSharedMemorySize, sharedMemorySize + numSharedPositions * positionsSize);

		IterationStateFactory factory(cloth, mFrameDt);
		IterationState<Simd4f> state = factory.create<Simd4f>(cloth);

		mFrameData.pushBack(CuFrameData(cloth, numSharedPositions, state, mIterationDataBegin + mIterationData.size()));

		while (state.mRemainingIterations)
		{
			mIterationData.pushBack(CuIterationData(state));
			state.update();
		}
	}
	mSharedMemorySize = maxSharedMemorySize;

	// add dummy element because we read past the end
	mIterationData.pushBack(CuIterationData());

	if (&mIterationData.front() != iterationDataBegin)
	{
		// mIterationData grew, update pointers
		iterationDataBegin = getDevicePointer(mIterationData);

		ptrdiff_t diff = (char*)iterationDataBegin - (char*)mIterationDataBegin;
		CuHostVector<CuFrameData>::Type::Iterator fIt = mFrameData.begin(), fEnd;
		for (fEnd = mFrameData.end(); fIt != fEnd; ++fIt)
			reinterpret_cast<const char*&>(fIt->mIterationData) += diff;

		mIterationDataBegin = iterationDataBegin;
	}
}

void CUDA_CB cloth::CuSolver::KernelFinished(CUstream stream, CUresult status, void *userData)
{
	PX_UNUSED(stream);
	PX_UNUSED(status);
	//static_cast<CuSolver*>(userData)->mEndSimulationTask.removeReference();
	PX_UNUSED(userData);
}

void cloth::CuSolver::executeKernel()
{
	CuContextLock contextLock(mFactory);

/*#if PX_PROFILE //We don't have a gpu distapcher anymore
	// Note: The profile buffer is valid only within the cuda launch context
	void* profileBuffer = getDispatcher().getCurrentProfileBuffer();
	if (mProfileBuffer != profileBuffer && mProfileBaseId + 1)
	{
		mProfileBuffer = profileBuffer;
		updateKernelData();
	}
#endif*/

	if (mClothDataDirty)
	{
		NV_CLOTH_ASSERT(mClothDataHostCopy.size() == mClothData.size());
		size_t numBytes = mClothData.size() * sizeof(CuClothData);
		checkSuccess(cuMemcpyHtoDAsync(mClothData.begin().dev(), mClothDataHostCopy.begin(), numBytes, mStream));
		mClothDataDirty = false;
	}

#if 0
	static int frame = 0;
	if (++frame == 100)
		record(*this);
#endif

	void* Arguments[] = {(void*)&mKernelDataHost};

	// launch kernel
	CUresult result = cuLaunchKernel(mKernelFunction, uint32_t(mCloths.size()), 1, 1, 
		mFactory.mNumThreadsPerBlock, 1, 1, mSharedMemorySize, mStream, Arguments, 0);
	cuStreamAddCallback(mStream, &cloth::CuSolver::KernelFinished, this, 0);


#if PX_DEBUG
	// in debug builds check kernel result
	checkSuccess(result);
	checkSuccess(cuStreamSynchronize(mStream));
#endif

	// mark the solver as being in an error state
	// all cloth instances should be migrated to software
	if (result != CUDA_SUCCESS)
		mCudaError = true;
}

void cloth::CuSolver::endFrame()
{
	checkSuccess(cuStreamSynchronize(mStream));

	CuHostVector<CuFrameData>::Type::ConstIterator fIt = mFrameData.begin();
	ClothVector::Iterator cIt, cEnd = mCloths.end();
	for (cIt = mCloths.begin(); cIt != cEnd; ++cIt, ++fIt)
	{
		CuCloth& cloth = **cIt;

		cloth.mHostParticlesDirty = false;
		cloth.mDeviceParticlesDirty = false;

		cloth.mMotionConstraints.pop();
		cloth.mMotionConstraints.mHostCopy.resize(0);

		cloth.mSeparationConstraints.pop();
		cloth.mSeparationConstraints.mHostCopy.resize(0);

		if (!cloth.mTargetCollisionSpheres.empty())
		{
			shdfnd::swap(cloth.mStartCollisionSpheres, cloth.mTargetCollisionSpheres);
			cloth.mTargetCollisionSpheres.resize(0);
		}

		if (!cloth.mTargetCollisionPlanes.empty())
		{
			shdfnd::swap(cloth.mStartCollisionPlanes, cloth.mTargetCollisionPlanes);
			cloth.mTargetCollisionPlanes.resize(0);
		}

		if (!cloth.mTargetCollisionTriangles.empty())
		{
			shdfnd::swap(cloth.mStartCollisionTriangles, cloth.mTargetCollisionTriangles);
			cloth.mTargetCollisionTriangles.resize(0);
		}

		for (uint32_t i = 0; i < 3; ++i)
		{
			float upper = fIt->mParticleBounds[i * 2 + 0];
			float negativeLower = fIt->mParticleBounds[i * 2 + 1];
			cloth.mParticleBoundsCenter[i] = (upper - negativeLower) * 0.5f;
			cloth.mParticleBoundsHalfExtent[i] = (upper + negativeLower) * 0.5f;
		}

		cloth.mSleepPassCounter = fIt->mSleepPassCounter;
		cloth.mSleepTestCounter = fIt->mSleepTestCounter;
	}

	interCollision();

	NV_CLOTH_PROFILE_STOP_CROSSTHREAD(mSimulateProfileEventData, "cloth::CuSolver::simulate", 0);
}

void cloth::CuSolver::interCollision()
{
	if (!mInterCollisionIterations || mInterCollisionDistance == 0.0f)
		return;
	if (mInterCollisionFilter == nullptr)
	{
		NV_CLOTH_LOG_WARNING("Inter collision will not work unless an inter collision filter is set using Solver::setInterCollisionFilter.");
		return;
	}

	typedef SwInterCollision<Simd4f> SwInterCollision;

	// rebuild cloth instance array
	mInterCollisionInstances.resize(0);
	for (uint32_t i = 0, n = mCloths.size(); i < n; ++i)
	{
		CuCloth& cloth = *mCloths[i];

		float elasticity = 1.0f / mFrameData[i].mNumIterations;
		NV_CLOTH_ASSERT(!cloth.mHostParticlesDirty);
		PxVec4* particles = cloth.mParticlesHostCopy.begin();
		uint32_t* indices = NULL, numIndices = cloth.mNumParticles;
		if (!cloth.mSelfCollisionIndices.empty())
		{
			indices = cloth.mSelfCollisionIndicesHost.begin();
			numIndices = uint32_t(cloth.mSelfCollisionIndices.size());
		}

		mInterCollisionInstances.pushBack(SwInterCollisionData(
		    particles, particles + cloth.mNumParticles, numIndices, indices, cloth.mTargetMotion,
		    cloth.mParticleBoundsCenter, cloth.mParticleBoundsHalfExtent, elasticity, cloth.mUserData));

		cloth.mDeviceParticlesDirty = true;
	}

	uint32_t requiredTempMemorySize = uint32_t(SwInterCollision::estimateTemporaryMemory(
		&mInterCollisionInstances[0], uint32_t(mInterCollisionInstances.size())));

	// realloc temp memory if necessary
	if (mInterCollisionScratchMemSize < requiredTempMemorySize)
	{
		if (mInterCollisionScratchMem)
			NV_CLOTH_FREE(mInterCollisionScratchMem);

		mInterCollisionScratchMem = NV_CLOTH_ALLOC(requiredTempMemorySize, "cloth::SwSolver::mInterCollisionScratchMem");
		mInterCollisionScratchMemSize = requiredTempMemorySize;
	}

	SwKernelAllocator allocator(mInterCollisionScratchMem, mInterCollisionScratchMemSize);

	// run inter-collision
	SwInterCollision(mInterCollisionInstances.begin(), mInterCollisionInstances.size(), mInterCollisionDistance,
	                 mInterCollisionStiffness, mInterCollisionIterations, mInterCollisionFilter, allocator)();
}