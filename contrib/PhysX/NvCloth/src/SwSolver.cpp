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

#include <foundation/PxProfiler.h>
#include "SwSolver.h"
#include "SwCloth.h"
#include "ClothImpl.h"
#include "SwFabric.h"
#include "SwFactory.h"
#include "SwClothData.h"
#include "SwSolverKernel.h"
#include "SwInterCollision.h"
#include "ps/PsFPU.h"
#include "ps/PsSort.h"

using namespace physx;

namespace nv
{
namespace cloth
{
bool neonSolverKernel(SwCloth const&, SwClothData&, SwKernelAllocator&, IterationStateFactory&);
}
}

using namespace nv;
using namespace cloth;
#if NV_SIMD_SIMD
typedef Simd4f Simd4fType;
#else
typedef Scalar4f Simd4fType;
#endif

cloth::SwSolver::SwSolver()
: mInterCollisionDistance(0.0f)
, mInterCollisionStiffness(1.0f)
, mInterCollisionIterations(1)
, mInterCollisionFilter(nullptr)
, mInterCollisionScratchMem(nullptr)
, mInterCollisionScratchMemSize(0)
, mSimulateProfileEventData(nullptr)
{
}

cloth::SwSolver::~SwSolver()
{
	if (mInterCollisionScratchMem)
		NV_CLOTH_FREE(mInterCollisionScratchMem);

	NV_CLOTH_ASSERT(mSimulatedCloths.empty());
}

namespace
{
template <typename T>
bool clothSizeGreater(const T& t0, const T& t1)
{
	return t0.mCloth->mCurParticles.size() > t1.mCloth->mCurParticles.size();
}

template <typename T>
void sortTasks(shdfnd::Array<T, cloth::NonTrackingAllocator>& tasks)
{
	shdfnd::sort(tasks.begin(), tasks.size(), &clothSizeGreater<T>, nv::cloth::NonTrackingAllocator());
}
}

void cloth::SwSolver::addCloth(Cloth* cloth)
{
	addClothAppend(cloth);
	sortTasks(mSimulatedCloths);
}

void cloth::SwSolver::addCloths(Range<Cloth*> cloths)
{
	for (uint32_t i = 0; i < cloths.size(); ++i)
	{
		addClothAppend(*(cloths.begin() + i));
	}
	sortTasks(mSimulatedCloths);
}

void cloth::SwSolver::removeCloth(Cloth* cloth)
{
	SwCloth& swCloth = *static_cast<SwCloth*>(cloth);

	//Remove from mSimulatedCloths
	{
		Vector<SimulatedCloth>::Type::Iterator tIt = mSimulatedCloths.begin();
		Vector<SimulatedCloth>::Type::Iterator tEnd = mSimulatedCloths.end();
		while(tIt != tEnd && tIt->mCloth != &swCloth)
			++tIt;

		if(tIt != tEnd)
		{
			NV_CLOTH_FREE(tIt->mScratchMemory);
			mSimulatedCloths.replaceWithLast(tIt);
			sortTasks(mSimulatedCloths);
		}
	}

	//Remove from mCloths
	{
		ClothVector::Iterator tEnd = mCloths.end();
		ClothVector::Iterator it = mCloths.find(&swCloth);

		if(it != tEnd)
		{
			mCloths.replaceWithLast(it);
		}
	}
}

int cloth::SwSolver::getNumCloths() const
{
	return mCloths.size();
}
cloth::Cloth * const * cloth::SwSolver::getClothList() const
{
	if(getNumCloths())
		return reinterpret_cast<Cloth* const*>(&mCloths[0]);
	else
		return nullptr;
}

bool cloth::SwSolver::beginSimulation(float dt)
{
	if (mSimulatedCloths.empty())
		return false;

	mCurrentDt = dt;
	beginFrame();

	return true;
}
void cloth::SwSolver::simulateChunk(int idx)
{
	NV_CLOTH_ASSERT(!mSimulatedCloths.empty());
	mSimulatedCloths[idx].Simulate();
	mSimulatedCloths[idx].Destroy();
}
void cloth::SwSolver::endSimulation()
{
	NV_CLOTH_ASSERT(!mSimulatedCloths.empty());
	interCollision();
	endFrame();
}

int cloth::SwSolver::getSimulationChunkCount() const
{
	return static_cast<int>(mSimulatedCloths.size());
}

void cloth::SwSolver::interCollision()
{
	if (!mInterCollisionIterations || mInterCollisionDistance == 0.0f)
		return;
	if (mInterCollisionFilter == nullptr)
	{
		NV_CLOTH_LOG_WARNING("Inter collision will not work unless an inter collision filter is set using Solver::setInterCollisionFilter.");
		return;
	}

	float elasticity = 1.0f;

	// rebuild cloth instance array
	mInterCollisionInstances.resize(0);
	for (uint32_t i = 0; i < mSimulatedCloths.size(); ++i)
	{
		SwCloth* c = mSimulatedCloths[i].mCloth;
		float invNumIterations = mSimulatedCloths[i].mInvNumIterations;

		mInterCollisionInstances.pushBack(SwInterCollisionData(
		    c->mCurParticles.begin(), c->mPrevParticles.begin(),
		    c->mSelfCollisionIndices.empty() ? c->mCurParticles.size() : c->mSelfCollisionIndices.size(),
		    c->mSelfCollisionIndices.empty() ? NULL : &c->mSelfCollisionIndices[0], c->mTargetMotion,
		    c->mParticleBoundsCenter, c->mParticleBoundsHalfExtent, elasticity * invNumIterations, c->mUserData));
	}

	const uint32_t requiredTempMemorySize = uint32_t(SwInterCollision<Simd4fType>::estimateTemporaryMemory(
	    &mInterCollisionInstances[0], mInterCollisionInstances.size()));

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
	SwInterCollision<Simd4fType> collider(mInterCollisionInstances.begin(), mInterCollisionInstances.size(),
	                                      mInterCollisionDistance, mInterCollisionStiffness, mInterCollisionIterations,
	                                      mInterCollisionFilter, allocator);

	collider();
}

void cloth::SwSolver::addClothAppend(Cloth* cloth)
{
	SwCloth& swCloth = *static_cast<SwCloth*>(cloth);
	NV_CLOTH_ASSERT(mCloths.find(&swCloth) == mCloths.end());

	mSimulatedCloths.pushBack(SimulatedCloth(swCloth, this));

	mCloths.pushBack(&swCloth);
}

void cloth::SwSolver::beginFrame() const
{
	mSimulateProfileEventData = NV_CLOTH_PROFILE_START_CROSSTHREAD("cloth::SwSolver::simulate", 0);
}

void cloth::SwSolver::endFrame() const
{
	NV_CLOTH_PROFILE_STOP_CROSSTHREAD(mSimulateProfileEventData,"cloth::SwSolver::simulate", 0);
}

cloth::SwSolver::SimulatedCloth::SimulatedCloth(SwCloth& cloth, SwSolver* parent)
	: mCloth(&cloth), mScratchMemorySize(0), mScratchMemory(0), mInvNumIterations(0.0f), mParent(parent)
{

}

void cloth::SwSolver::SimulatedCloth::Destroy()
{
	mCloth->mMotionConstraints.pop();
	mCloth->mSeparationConstraints.pop();

	if (!mCloth->mTargetCollisionSpheres.empty())
	{
		swap(mCloth->mStartCollisionSpheres, mCloth->mTargetCollisionSpheres);
		mCloth->mTargetCollisionSpheres.resize(0);
	}

	if (!mCloth->mTargetCollisionPlanes.empty())
	{
		swap(mCloth->mStartCollisionPlanes, mCloth->mTargetCollisionPlanes);
		mCloth->mTargetCollisionPlanes.resize(0);
	}

	if (!mCloth->mTargetCollisionTriangles.empty())
	{
		swap(mCloth->mStartCollisionTriangles, mCloth->mTargetCollisionTriangles);
		mCloth->mTargetCollisionTriangles.resize(0);
	}
}
void cloth::SwSolver::SimulatedCloth::Simulate()
{
	// check if we need to reallocate the temp memory buffer
	// (number of shapes may have changed)
	uint32_t requiredTempMemorySize = uint32_t(SwSolverKernel<Simd4fType>::estimateTemporaryMemory(*mCloth));

	if (mScratchMemorySize < requiredTempMemorySize)
	{
		NV_CLOTH_FREE(mScratchMemory);

		mScratchMemory = NV_CLOTH_ALLOC(requiredTempMemorySize, "cloth::SwSolver::mScratchMemory");
		mScratchMemorySize = requiredTempMemorySize;
	}

	if (mParent->mCurrentDt == 0.0f)
		return;

	IterationStateFactory factory(*mCloth, mParent->mCurrentDt);
	mInvNumIterations = factory.mInvNumIterations;

	shdfnd::SIMDGuard simdGuard;

	SwClothData data(*mCloth, mCloth->mFabric);
	SwKernelAllocator allocator(mScratchMemory, uint32_t(mScratchMemorySize));

	// construct kernel functor and execute
#if NV_ANDROID
	if (!neonSolverKernel(*mCloth, data, allocator, factory))
	{
		//NV_CLOTH_LOG_WARNING("No NEON CPU support detected. Falling back to scalar types.");
		SwSolverKernel<Scalar4f>(*mCloth, data, allocator, factory)();
	}
#else
	SwSolverKernel<Simd4fType>(*mCloth, data, allocator, factory)();
#endif

	data.reconcile(*mCloth); // update cloth
}
