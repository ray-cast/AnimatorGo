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

#include "CuCloth.h"
#include "CuFabric.h"
#include "CuFactory.h"
#include "CuContextLock.h"
#include "CuCheckSuccess.h"
#include "CuClothData.h"
#include "CuSolver.h"
#include "../TripletScheduler.h"
#include "../ClothBase.h"
#include <foundation/PxMat44.h>

using namespace physx;

#if PX_VC
#pragma warning(disable : 4365) // 'action' : conversion from 'type_1' to 'type_2', signed/unsigned mismatch
#endif

namespace nv
{
namespace cloth
{
PhaseConfig transform(const PhaseConfig&); // from PhaseConfig.cpp
}
}

using namespace nv;

namespace
{
bool isSelfCollisionEnabled(const cloth::CuCloth& cloth)
{
	return std::min(cloth.mSelfCollisionDistance, -cloth.mSelfCollisionLogStiffness) > 0.0f;
}
}

cloth::CuCloth::CuCloth(CuFactory& factory, CuFabric& fabric, Range<const PxVec4> particles)
: CuContextLock(factory)
, mFactory(factory)
, mFabric(fabric)
, mClothDataDirty(false)
, mNumParticles(uint32_t(particles.size()))
, mParticles(mFactory.mContext)
, mParticlesHostCopy(mFactory.mContext)
, mDeviceParticlesDirty(false)
, mHostParticlesDirty(true)
, mPhaseConfigs(mFactory.mContext)
, mMotionConstraints(mFactory.mContext)
, mSeparationConstraints(mFactory.mContext)
, mParticleAccelerations(mFactory.mContext)
, mParticleAccelerationsHostCopy(mFactory.mContext)
, mCapsuleIndices(mFactory.mContext)
, mStartCollisionSpheres(mFactory.mContext)
, mTargetCollisionSpheres(mFactory.mContext)
, mConvexMasks(mFactory.mContext)
, mStartCollisionPlanes(mFactory.mContext)
, mTargetCollisionPlanes(mFactory.mContext)
, mStartCollisionTriangles(mFactory.mContext)
, mTargetCollisionTriangles(mFactory.mContext)
, mVirtualParticleSetSizes(mFactory.mContext)
, mVirtualParticleIndices(mFactory.mContext)
, mVirtualParticleWeights(mFactory.mContext)
, mRestPositions(mFactory.mContext)
, mSelfCollisionIndices(mFactory.mContext)
, mSelfCollisionData(mFactory.mContext)
, mSharedMemorySize(0)
, mUserData(0)
{
	NV_CLOTH_ASSERT(!particles.empty());
	NV_CLOTH_ASSERT(particles.size() == fabric.getNumParticles());

	initialize(*this, particles.begin(), particles.end());

	mParticles.reserve(2 * mNumParticles);
	mParticles.push_back(particles.begin(), particles.end());
	mParticles.push_back(particles.begin(), particles.end());
	mParticlesHostCopy.resizeUninitialized(2 * mNumParticles);

	mFabric.incRefCount();

	CuContextLock::release();
}

cloth::CuCloth::CuCloth(CuFactory& factory, const CuCloth& cloth)
: CuContextLock(factory)
, mFactory(factory)
, mFabric(cloth.mFabric)
, mNumParticles(cloth.mNumParticles)
, mParticles(cloth.mParticles)
, mParticlesHostCopy(cloth.mParticlesHostCopy)
, mDeviceParticlesDirty(cloth.mDeviceParticlesDirty)
, mHostParticlesDirty(cloth.mHostParticlesDirty)
, mPhaseConfigs(cloth.mPhaseConfigs)
, mHostPhaseConfigs(cloth.mHostPhaseConfigs)
, mMotionConstraints(cloth.mMotionConstraints)
, mSeparationConstraints(cloth.mSeparationConstraints)
, mParticleAccelerations(cloth.mParticleAccelerations)
, mParticleAccelerationsHostCopy(cloth.mParticleAccelerationsHostCopy)
, mCapsuleIndices(cloth.mCapsuleIndices)
, mStartCollisionSpheres(cloth.mStartCollisionSpheres)
, mTargetCollisionSpheres(cloth.mTargetCollisionSpheres)
, mStartCollisionPlanes(cloth.mStartCollisionPlanes)
, mTargetCollisionPlanes(cloth.mTargetCollisionPlanes)
, mStartCollisionTriangles(cloth.mStartCollisionTriangles)
, mTargetCollisionTriangles(cloth.mTargetCollisionTriangles)
, mVirtualParticleSetSizes(cloth.mVirtualParticleSetSizes)
, mVirtualParticleIndices(cloth.mVirtualParticleIndices)
, mVirtualParticleWeights(cloth.mVirtualParticleWeights)
, mRestPositions(cloth.mRestPositions)
, mSelfCollisionIndices(cloth.mSelfCollisionIndices)
, mSelfCollisionData(mFactory.mContext)
, mSharedMemorySize(cloth.mSharedMemorySize)
, mUserData(cloth.mUserData)
{
	copy(*this, cloth);

	mFabric.incRefCount();

	CuContextLock::release();
}

cloth::CuCloth::~CuCloth()
{
	CuContextLock::acquire();

	mFabric.decRefCount();
}

void cloth::CuCloth::notifyChanged()
{
	mClothDataDirty = true;
}

bool cloth::CuCloth::updateClothData(CuClothData& clothData)
{
	// test particle pointer to detect when cloth data array has been reordered
	if (!mClothDataDirty && clothData.mParticles == array(*mParticles.begin().get()))
	{
		NV_CLOTH_ASSERT(mSharedMemorySize == getSharedMemorySize());
		return false;
	}

	mSharedMemorySize = getSharedMemorySize();

	if (mSelfCollisionData.empty() && isSelfCollisionEnabled(*this))
	{
		uint32_t numSelfCollisionIndices =
		    mSelfCollisionIndices.empty() ? mNumParticles : uint32_t(mSelfCollisionIndices.size());

		uint32_t particleSize = 4 * mNumParticles;
		uint32_t keySize = 2 * numSelfCollisionIndices;           // 2x for radix buffer
		uint32_t cellStartSize = (129 + 128 * 128 + 130) / 2 + 1; // half because type is int16_t

		// use 16bit indices for cellStart array (128x128 grid)
		mSelfCollisionData.resize(particleSize + keySize + cellStartSize);
		checkSuccess(cuMemsetD32((mSelfCollisionData.begin() + particleSize + keySize).dev(), 0xffffffff, cellStartSize));
	}

	clothData = CuClothData(*this);
	mClothDataDirty = false;

	return true;
}

uint32_t cloth::CuCloth::getSharedMemorySize() const
{
	uint32_t numPhases = uint32_t(mPhaseConfigs.size());
	uint32_t numSpheres = uint32_t(mStartCollisionSpheres.size());
	uint32_t numCones = uint32_t(mCapsuleIndices.size());
	uint32_t numPlanes = uint32_t(mStartCollisionPlanes.size());
	uint32_t numConvexes = uint32_t(mConvexMasks.size());
	uint32_t numTriangles = uint32_t(mStartCollisionTriangles.size() / 3);

	uint32_t phaseConfigSize = numPhases * sizeof(CuPhaseConfig);

	bool storePrevCollisionData = mEnableContinuousCollision || mFriction > 0.0f;
	uint32_t continuousCollisionSize = storePrevCollisionData ? 4 * numSpheres + 10 * numCones : 0;
	continuousCollisionSize += 4 * numCones + numConvexes; // capsule and convex masks
	uint32_t discreteCollisionSize = 4 * numSpheres + std::max(10 * numCones + 96, 208u);
	discreteCollisionSize = std::max(discreteCollisionSize, std::max(4 * numPlanes, 19 * numTriangles));

	// scratch memory for prefix sum and histogram
	uint32_t selfCollisionSize = isSelfCollisionEnabled(*this) ? 571u : 0; //Note: different sizes between PhysX and APEX, took APEX size as it was larger

	// see CuSolverKenel.cu::gSharedMemory comment for details
	return phaseConfigSize + sizeof(float) * (continuousCollisionSize + std::max(selfCollisionSize, discreteCollisionSize));
}

void cloth::CuCloth::setPhaseConfigInternal(Range<const PhaseConfig> configs)
{
	mHostPhaseConfigs.assign(configs.begin(), configs.end());

	Vector<CuPhaseConfig>::Type deviceConfigs;
	deviceConfigs.reserve(configs.size());
	const PhaseConfig* cEnd = configs.end();
	for (const PhaseConfig* cIt = configs.begin(); cIt != cEnd; ++cIt)
	{
		CuPhaseConfig config;

		config.mStiffness = cIt->mStiffness;
		config.mStiffnessMultiplier = cIt->mStiffnessMultiplier;
		config.mCompressionLimit = cIt->mCompressionLimit;
		config.mStretchLimit = cIt->mStretchLimit;

		uint16_t phaseIndex = cIt->mPhaseIndex;
		config.mNumConstraints = mFabric.mNumConstraintsInPhase[phaseIndex];
		config.mRestvalues = mFabric.mRestvaluesInPhase[phaseIndex].get();
		config.mStiffnessValues = mFabric.mStiffnessValuesInPhase[phaseIndex].get();
		config.mIndices = mFabric.mIndicesInPhase[phaseIndex].get();

		deviceConfigs.pushBack(config);
	}

	CuContextLock contextLock(mFactory);
	mPhaseConfigs.assign(deviceConfigs.begin(), deviceConfigs.end());
}

cloth::Range<PxVec4> cloth::CuCloth::push(cloth::CuConstraints& constraints)
{
	if (!constraints.mTarget.capacity())
	{
		CuContextLock contextLock(mFactory);
		constraints.mTarget.reserve(mNumParticles);
	}
	if (constraints.mHostCopy.empty())
		constraints.mTarget.resize(mNumParticles);

	if (constraints.mStart.empty()) // initialize start first
		constraints.mStart.swap(constraints.mTarget);

	if (!constraints.mHostCopy.capacity())
	{
		CuContextLock contextLock(mFactory);
		constraints.mHostCopy.reserve(mNumParticles);
	}
	constraints.mHostCopy.resizeUninitialized(mNumParticles);

	PxVec4* data = &constraints.mHostCopy.front();
	return Range<PxVec4>(data, data + constraints.mHostCopy.size());
}

void cloth::CuCloth::clear(cloth::CuConstraints& constraints)
{
	CuContextLock contextLock(mFactory);
	CuDeviceVector<PxVec4>(mFactory.mContext).swap(constraints.mStart);
	CuDeviceVector<PxVec4>(mFactory.mContext).swap(constraints.mTarget);
}

void cloth::CuCloth::syncDeviceParticles()
{
	if (mDeviceParticlesDirty)
	{
		CuContextLock contextLock(mFactory);
		checkSuccess(
		    cuMemcpyHtoD(mParticles.begin().dev(), mParticlesHostCopy.begin(), 2 * mNumParticles * sizeof(PxVec4)));
		mDeviceParticlesDirty = false;
	}
}

void cloth::CuCloth::syncHostParticles()
{
	if (mHostParticlesDirty)
	{
		CuContextLock contextLock(mFactory);
		checkSuccess(cuMemcpyDtoH(mParticlesHostCopy.begin(), mParticles.begin().dev(), 2 * mNumParticles * sizeof(PxVec4)));
		mHostParticlesDirty = false;
	}
}

cloth::Range<const PxVec3> cloth::CuCloth::clampTriangleCount(Range<const PxVec3> range, uint32_t replaceSize)
{
	// clamp to 500 triangles (1500 vertices) to prevent running out of shared memory
	uint32_t removedSize = mStartCollisionTriangles.size() - replaceSize;
	const PxVec3* clamp = range.begin() + 1500 - removedSize;

	if (range.end() > clamp)
	{
		NV_CLOTH_LOG_WARNING("Too many collision triangles specified for cloth, dropping all but first 500.\n");
	}

	return Range<const PxVec3>(range.begin(), std::min(range.end(), clamp));
}

#include "../ClothImpl.h"

namespace nv
{
namespace cloth
{

// CuCloth::clone() implemented in CuClothClone.cpp

uint32_t CuCloth::getNumParticles() const
{
	return mNumParticles;
}

void CuCloth::lockParticles() const
{
	const_cast<CuCloth&>(*this).syncHostParticles();
}

void CuCloth::unlockParticles() const
{
}

MappedRange<PxVec4> CuCloth::getCurrentParticles()
{
	wakeUp();
	lockParticles();
	mDeviceParticlesDirty = true;
	return getMappedParticles(mParticlesHostCopy.begin());
}

MappedRange<const PxVec4> CuCloth::getCurrentParticles() const
{
	lockParticles();
	return getMappedParticles(mParticlesHostCopy.begin());
}

MappedRange<PxVec4> CuCloth::getPreviousParticles()
{
	wakeUp();
	lockParticles();
	mDeviceParticlesDirty = true;
	return getMappedParticles(mParticlesHostCopy.begin() + mNumParticles);
}

MappedRange<const PxVec4> CuCloth::getPreviousParticles() const
{
	lockParticles();
	return getMappedParticles(mParticlesHostCopy.begin() + mNumParticles);
}

GpuParticles CuCloth::getGpuParticles()
{
	syncDeviceParticles();
	mHostParticlesDirty = true;
	PxVec4* particles = mParticles.begin().get();
	GpuParticles result = { particles, particles + mNumParticles, 0 };
	return result;
}

void CuCloth::setPhaseConfig(Range<const PhaseConfig> configs)
{
	Vector<PhaseConfig>::Type transformedConfigs;
	transformedConfigs.reserve(configs.size());

	// transform phase config to use in solver
	for (; !configs.empty(); configs.popFront())
		if (configs.front().mStiffness > 0.0f)
			transformedConfigs.pushBack(transform(configs.front()));

	setPhaseConfigInternal(Range<const PhaseConfig>(transformedConfigs.begin(), transformedConfigs.end()));
	notifyChanged();
	wakeUp();
}

void CuCloth::setSelfCollisionIndices(Range<const uint32_t> indices)
{
	ContextLockType lock(mFactory);
	mSelfCollisionIndices.assign(indices.begin(), indices.end());
	mSelfCollisionIndicesHost.assign(indices.begin(), indices.end());
	notifyChanged();
	wakeUp();
}

uint32_t CuCloth::getNumVirtualParticles() const
{
	return uint32_t(mVirtualParticleIndices.size());
}

Range<PxVec4> CuCloth::getParticleAccelerations()
{
	if (mParticleAccelerations.empty())
	{
		CuContextLock contextLock(mFactory);
		mParticleAccelerations.resize(mNumParticles);
	}

	if (!mParticleAccelerationsHostCopy.capacity())
	{
		CuContextLock contextLock(mFactory);
		mParticleAccelerationsHostCopy.reserve(mNumParticles);
	}
	mParticleAccelerationsHostCopy.resizeUninitialized(mNumParticles);

	wakeUp();

	PxVec4* data = mParticleAccelerationsHostCopy.begin();
	return Range<PxVec4>(data, mParticleAccelerationsHostCopy.end());
}

void CuCloth::clearParticleAccelerations()
{
	CuContextLock contextLock(mFactory);
	CuDeviceVector<PxVec4>(mFactory.mContext).swap(mParticleAccelerations);
	CuHostVector<PxVec4, CU_MEMHOSTALLOC_DEVICEMAP>::Type(mFactory.mContext).swap(mParticleAccelerationsHostCopy);
	wakeUp();
}

namespace
{
	uint32_t calculateNumReplays(const nv::cloth::Vector<cloth::Vec4u>::Type& triplets, const nv::cloth::Vector<uint32_t>::Type setSizes)
	{
		uint32_t result = 0;

		auto tIt = triplets.begin();
		uint32_t index = 0;
		for (auto sIt = setSizes.begin(), sEnd = setSizes.end(); sIt != sEnd; ++sIt, ++index)
		{
			auto tEnd = tIt + *sIt, tLast = tIt;
			while (tLast != tEnd)
			{
				uint8_t numConflicts[3][32] = {};
				uint8_t numReplays[3] = {};

				for (tLast += std::min(ptrdiff_t(32), tEnd - tLast); tIt != tLast; ++tIt)
					for (int i = 0; i < 3; ++i)
						numReplays[i] = std::max(numReplays[i], ++numConflicts[i][(*tIt)[i] & 31]);

				result += numReplays[0] + numReplays[1] + numReplays[2];
			}
		}

		return result;
	}
}

void CuCloth::setVirtualParticles(Range<const uint32_t[4]> indices, Range<const PxVec3> weights)
{
	// shuffle indices to form independent SIMD sets
	TripletScheduler scheduler(indices);
	scheduler.warp(mNumParticles, 32);

	// convert to 16bit indices
	Vector<Vec4us>::Type hostIndices;
	hostIndices.reserve(indices.size());
	TripletScheduler::ConstTripletIter tIt = scheduler.mTriplets.begin();
	TripletScheduler::ConstTripletIter tEnd = scheduler.mTriplets.end();
	for (; tIt != tEnd; ++tIt)
		hostIndices.pushBack(Vec4us(*tIt));

	// printf("num sets = %u, num replays = %u\n", scheduler.mSetSizes.size(),
	//	calculateNumReplays(scheduler.mTriplets, scheduler.mSetSizes));

	// add normalization weight
	Vector<PxVec4>::Type hostWeights;
	hostWeights.reserve(weights.size());
	for (; !weights.empty(); weights.popFront())
	{
		PxVec3 w = reinterpret_cast<const PxVec3&>(weights.front());
		float scale = 1 / w.magnitudeSquared();
		hostWeights.pushBack(PxVec4( w.x, w.y, w.z, scale ));
	}

	CuContextLock contextLock(mFactory);

	// todo: 'swap' these to force reallocation?
	mVirtualParticleIndices = hostIndices;
	mVirtualParticleSetSizes = scheduler.mSetSizes;
	mVirtualParticleWeights = hostWeights;

	notifyChanged();
	wakeUp();
}

} // namespace cloth
} // namespace nv
