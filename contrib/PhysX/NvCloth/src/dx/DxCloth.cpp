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

#include "DxCloth.h"
#include "DxFabric.h"
#include "DxFactory.h"
#include "DxContextLock.h"
#include "DxClothData.h"
#include "DxSolver.h"
#include "../TripletScheduler.h"
#include "../ClothBase.h"
#include <foundation/PxMat44.h>
#include <PsFoundation.h>

#if NV_CLOTH_ENABLE_DX11

using namespace physx;

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
bool isSelfCollisionEnabled(const cloth::DxCloth& cloth)
{
	return std::min(cloth.mSelfCollisionDistance, -cloth.mSelfCollisionLogStiffness) > 0.0f;
}
}

cloth::DxCloth::DxCloth(DxFactory& factory, DxFabric& fabric, Range<const PxVec4> particles)
: DxContextLock(factory)
, mFactory(factory)
, mFabric(fabric)
, mClothDataDirty(false)
, mClothCostDirty(true)
, mNumParticles(uint32_t(particles.size()))
, mDeviceParticlesDirty(true)
, mHostParticlesDirty(false)
, mMotionConstraints(mFactory.mMotionConstraints)
, mSeparationConstraints(mFactory.mSeparationConstraints)
, mParticles(mFactory.mParticles)
, mParticlesHostCopy(mFactory.mParticlesHostCopy)
, mParticlesMapPointer(0)
, mParticlesMapRefCount(0)
, mPhaseConfigs(mFactory.mPhaseConfigs)
, mParticleAccelerations(mFactory.mParticleAccelerations)
, mCapsuleIndices(mFactory.mCapsuleIndices)
, mStartCollisionSpheres(mFactory.mCollisionSpheres)
, mTargetCollisionSpheres(mFactory.mCollisionSpheres)
, mConvexMasks(mFactory.mConvexMasks)
, mStartCollisionPlanes(mFactory.mCollisionPlanes)
, mTargetCollisionPlanes(mFactory.mCollisionPlanes)
, mStartCollisionTriangles(mFactory.mCollisionTriangles)
, mTargetCollisionTriangles(mFactory.mCollisionTriangles)
, mVirtualParticleSetSizes(mFactory.mContextManager)
, mVirtualParticleIndices(mFactory.mContextManager)
, mVirtualParticleWeights(mFactory.mContextManager)
, mRestPositions(mFactory.mRestPositions)
, mSelfCollisionIndices(mFactory.mSelfCollisionIndices)
, mSelfCollisionParticles(mFactory.mSelfCollisionParticles)
, mSelfCollisionData(mFactory.mSelfCollisionData)
, mInitSelfCollisionData(false)
, mSharedMemorySize(0)
, mUserData(0)
{
	NV_CLOTH_ASSERT(!particles.empty());
	NV_CLOTH_ASSERT(particles.size() == fabric.getNumParticles());

	initialize(*this, particles.begin(), particles.end());

	mParticlesHostCopy.resize(2 * mNumParticles);
	PxVec4* pIt = mParticlesHostCopy.map(D3D11_MAP_WRITE);
	memcpy(pIt, particles.begin(), mNumParticles * sizeof(PxVec4));
	memcpy(pIt + mNumParticles, particles.begin(), mNumParticles * sizeof(PxVec4));
	mParticlesHostCopy.unmap();

	mParticles.resize(2 * mNumParticles);
	mFabric.incRefCount();

	DxContextLock::release();
}

cloth::DxCloth::DxCloth(DxFactory& factory, const DxCloth& cloth)
: DxContextLock(factory)
, mFactory(factory)
, mFabric(cloth.mFabric)
, mNumParticles(cloth.mNumParticles)
, mParticles(cloth.mParticles)
, mParticlesHostCopy(cloth.mParticlesHostCopy)
, mParticlesMapPointer(0)
, mParticlesMapRefCount(0)
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
, mConvexMasks(cloth.mConvexMasks)
, mStartCollisionPlanes(cloth.mStartCollisionPlanes)
, mTargetCollisionPlanes(cloth.mTargetCollisionPlanes)
, mStartCollisionTriangles(cloth.mStartCollisionTriangles)
, mTargetCollisionTriangles(cloth.mTargetCollisionTriangles)
, mVirtualParticleSetSizes(cloth.mVirtualParticleSetSizes)
, mVirtualParticleIndices(cloth.mVirtualParticleIndices)
, mVirtualParticleWeights(cloth.mVirtualParticleWeights)
, mRestPositions(cloth.mRestPositions)
, mSelfCollisionIndices(cloth.mSelfCollisionIndices)
, mSelfCollisionParticles(cloth.mSelfCollisionParticles)
, mSelfCollisionData(cloth.mSelfCollisionData)
, mInitSelfCollisionData(cloth.mInitSelfCollisionData)
, mSharedMemorySize(cloth.mSharedMemorySize)
, mUserData(cloth.mUserData)
{
	copy(*this, cloth);

	mFabric.incRefCount();

	DxContextLock::release();
}

cloth::DxCloth::~DxCloth()
{
	DxContextLock::acquire();

	mFabric.decRefCount();
}

void cloth::DxCloth::notifyChanged()
{
	mClothDataDirty = true;
}

bool cloth::DxCloth::updateClothData(DxClothData& clothData)
{
	if (!mClothDataDirty)
	{
		NV_CLOTH_ASSERT(mSharedMemorySize == getSharedMemorySize());
		return false;
	}

	mSharedMemorySize = getSharedMemorySize();

	if (mSelfCollisionData.empty() && isSelfCollisionEnabled(*this))
	{
		uint32_t numSelfCollisionIndices =
		    mSelfCollisionIndices.empty() ? mNumParticles : uint32_t(mSelfCollisionIndices.size());

		uint32_t keySize = 2 * numSelfCollisionIndices;           // 2x for radix buffer
		uint32_t cellStartSize = (129 + 128 * 128 + 130); 

		mSelfCollisionParticles.resize(mNumParticles);
		mSelfCollisionData.resize(keySize + cellStartSize);
		// checkSuccess( cuMemsetD32((mSelfCollisionData.begin()
		//	+ particleSize + keySize).dev(), 0xffffffff, cellStartSize) );

		mInitSelfCollisionData = true;
	}

	clothData = DxClothData(*this);
	mClothDataDirty = false;

	return true;
}

uint32_t cloth::DxCloth::getSharedMemorySize() const
{
	uint32_t numPhases = uint32_t(mPhaseConfigs.size());
	uint32_t numSpheres = uint32_t(mStartCollisionSpheres.size());
	uint32_t numCones = uint32_t(mCapsuleIndices.size());
	uint32_t numPlanes = uint32_t(mStartCollisionPlanes.size());
	uint32_t numConvexes = uint32_t(mConvexMasks.size());
	uint32_t numTriangles = uint32_t(mStartCollisionTriangles.size() / 3);

	uint32_t phaseConfigSize = numPhases * sizeof(DxPhaseConfig);

	bool storePrevCollisionData = mEnableContinuousCollision || mFriction > 0.0f;
	uint32_t continuousCollisionSize = storePrevCollisionData ? 4 * numSpheres + 10 * numCones : 0;
	continuousCollisionSize += 4 * numCones + numConvexes; // capsule and convex masks
	uint32_t discreteCollisionSize = 4 * numSpheres + std::max(10 * numCones + 96, 208u);
	discreteCollisionSize = std::max(discreteCollisionSize, std::max(4 * numPlanes, 19 * numTriangles));

	// scratch memory for prefix sum and histogram
	uint32_t selfCollisionSize = isSelfCollisionEnabled(*this) ? 544 : 0;

	// see CuSolverKenel.cu::gSharedMemory comment for details
	return phaseConfigSize + sizeof(float) * (continuousCollisionSize + std::max(selfCollisionSize, discreteCollisionSize));
}

void cloth::DxCloth::setPhaseConfig(Range<const PhaseConfig> configs)
{
	mHostPhaseConfigs.assign(configs.begin(), configs.end());

	Vector<DxPhaseConfig>::Type deviceConfigs;
	deviceConfigs.reserve(configs.size());
	const PhaseConfig* cEnd = configs.end();
	for (const PhaseConfig* cIt = configs.begin(); cIt != cEnd; ++cIt)
	{
		DxPhaseConfig config;

		config.mStiffness = cIt->mStiffness;
		config.mStiffnessMultiplier = cIt->mStiffnessMultiplier;
		config.mCompressionLimit = cIt->mCompressionLimit;
		config.mStretchLimit = cIt->mStretchLimit;

		uint16_t phaseIndex = cIt->mPhaseIndex;
		config.mFirstConstraint = mFabric.mFirstConstraintInPhase[phaseIndex];
		config.mNumConstraints = mFabric.mNumConstraintsInPhase[phaseIndex];

		deviceConfigs.pushBack(config);
	}

	DxContextLock contextLock(mFactory);

	mPhaseConfigs.assign(deviceConfigs.begin(), deviceConfigs.begin() + deviceConfigs.size());
}

cloth::Range<PxVec4> cloth::DxCloth::push(cloth::DxConstraints& constraints)
{
	if (!constraints.mTarget.capacity())
	{
		DxContextLock contextLock(mFactory);
		constraints.mTarget.reserve(mNumParticles);
	}
	if (constraints.mHostCopy.empty())
		constraints.mTarget.resize(mNumParticles);

	if (constraints.mStart.empty()) // initialize start first
	{
		DxContextLock contextLock(mFactory);
		constraints.mStart.swap(constraints.mTarget);
	}

	if (!constraints.mHostCopy.capacity())
	{
		DxContextLock contextLock(mFactory);
		constraints.mHostCopy.reserve(mNumParticles);
	}
	constraints.mHostCopy.resizeUninitialized(mNumParticles);

	PxVec4* data = &constraints.mHostCopy.front();
	return Range<PxVec4>(data, data + constraints.mHostCopy.size());
}

void cloth::DxCloth::clear(cloth::DxConstraints& constraints)
{
	DxContextLock contextLock(mFactory);
	constraints.mStart.clear();
	constraints.mTarget.clear();
}

void cloth::DxCloth::mapParticles()
{
	if (mHostParticlesDirty)
	{
		DxContextLock contextLock(mFactory);
		mParticlesHostCopy = mParticles;
		mHostParticlesDirty = false;
	}
	if (0 == mParticlesMapRefCount++)
	{
		DxContextLock contextLock(mFactory);
		mParticlesMapPointer = mParticlesHostCopy.map();
	}
}

void cloth::DxCloth::unmapParticles()
{
	if (0 == --mParticlesMapRefCount)
	{
		DxContextLock contextLock(mFactory);
		mParticlesHostCopy.unmap();
		mParticlesMapPointer = 0;
	}
}

cloth::Range<const PxVec3> cloth::DxCloth::clampTriangleCount(Range<const PxVec3> range, uint32_t replaceSize)
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

// ClothImpl<DxCloth>::clone() implemented in DxClothClone.cpp

template <>
uint32_t ClothImpl<DxCloth>::getNumParticles() const
{
	return mCloth.mNumParticles;
}

template <>
void ClothImpl<DxCloth>::lockParticles() const
{
	const_cast<DxCloth&>(mCloth).mapParticles();
}

template <>
void ClothImpl<DxCloth>::unlockParticles() const
{
	const_cast<DxCloth&>(mCloth).unmapParticles();
}

template <>
MappedRange<PxVec4> ClothImpl<DxCloth>::getCurrentParticles()
{
	mCloth.wakeUp();
	lockParticles();
	mCloth.mDeviceParticlesDirty = true;
	return getMappedParticles(mCloth.mParticlesMapPointer);
}

template <>
MappedRange<const PxVec4> ClothImpl<DxCloth>::getCurrentParticles() const
{
	lockParticles();
	const PxVec4* data = mCloth.mParticlesMapPointer;
	return getMappedParticles(data);
}

template <>
MappedRange<PxVec4> ClothImpl<DxCloth>::getPreviousParticles()
{
	mCloth.wakeUp();
	lockParticles();
	mCloth.mDeviceParticlesDirty = true;
	return getMappedParticles(mCloth.mParticlesMapPointer + mCloth.mNumParticles);
}

template <>
MappedRange<const PxVec4> ClothImpl<DxCloth>::getPreviousParticles() const
{
	lockParticles();
	const PxVec4* data = (const PxVec4*)mCloth.mParticlesMapPointer;
	return getMappedParticles(data + mCloth.mNumParticles);
}

template <>
GpuParticles ClothImpl<DxCloth>::getGpuParticles()
{
	ID3D11Buffer* buffer = mCloth.mParticles.buffer();
	PxVec4* offset = (PxVec4*)nullptr + mCloth.mParticles.mOffset;
	GpuParticles result = { offset, offset + mCloth.mNumParticles, buffer };
	return result;
}

template <>
void ClothImpl<DxCloth>::setPhaseConfig(Range<const PhaseConfig> configs)
{
	Vector<PhaseConfig>::Type transformedConfigs;
	transformedConfigs.reserve(configs.size());

	// transform phase config to use in solver
	for (; !configs.empty(); configs.popFront())
		if (configs.front().mStiffness > 0.0f)
			transformedConfigs.pushBack(transform(configs.front()));

	mCloth.setPhaseConfig(Range<const PhaseConfig>(transformedConfigs.begin(), 
		transformedConfigs.begin() + transformedConfigs.size()));
	mCloth.notifyChanged();
	mCloth.wakeUp();
}

template <>
void ClothImpl<DxCloth>::setSelfCollisionIndices(Range<const uint32_t> indices)
{
	ContextLockType lock(mCloth.mFactory);
	mCloth.mSelfCollisionIndices.assign(indices.begin(), indices.end());
	mCloth.mSelfCollisionIndicesHost.assign(indices.begin(), indices.end());
	mCloth.notifyChanged();
	mCloth.wakeUp();
}

template <>
uint32_t ClothImpl<DxCloth>::getNumVirtualParticles() const
{
	return uint32_t(mCloth.mVirtualParticleIndices.size());
}

template <>
Range<PxVec4> ClothImpl<DxCloth>::getParticleAccelerations()
{
	if (mCloth.mParticleAccelerations.empty())
	{
		DxContextLock contextLock(mCloth.mFactory);
		mCloth.mParticleAccelerations.resize(mCloth.mNumParticles);
	}

	if (!mCloth.mParticleAccelerationsHostCopy.capacity())
	{
		DxContextLock contextLock(mCloth.mFactory);
		mCloth.mParticleAccelerationsHostCopy.reserve(mCloth.mNumParticles);
	}
	mCloth.mParticleAccelerationsHostCopy.resizeUninitialized(mCloth.mNumParticles);

	mCloth.wakeUp();

	PxVec4* data = mCloth.mParticleAccelerationsHostCopy.begin();
	return Range<PxVec4>(data, mCloth.mParticleAccelerationsHostCopy.end());
}

template <>
void ClothImpl<DxCloth>::clearParticleAccelerations()
{
	DxContextLock contextLock(mCloth.mFactory);
	mCloth.mParticleAccelerations.clear();
	Vector<PxVec4>::Type().swap(mCloth.mParticleAccelerationsHostCopy);
	mCloth.wakeUp();
}

namespace
{
uint32_t calculateNumReplays(const Vector<Vec4u>::Type& triplets, const Vector<uint32_t>::Type setSizes)
{
	uint32_t result = 0;

	Vector<Vec4u>::Type::ConstIterator tIt = triplets.begin();
	Vector<uint32_t>::Type::ConstIterator sIt, sEnd = setSizes.end();
	uint32_t index = 0;
	for (sIt = setSizes.begin(); sIt != sEnd; ++sIt, ++index)
	{
		Vector<Vec4u>::Type::ConstIterator tEnd = tIt + *sIt, tLast = tIt;
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

template <>
void ClothImpl<DxCloth>::setVirtualParticles(Range<const uint32_t[4]> indices, Range<const PxVec3> weights)
{
	// shuffle indices to form independent SIMD sets
	TripletScheduler scheduler(indices);
	scheduler.warp(mCloth.mNumParticles, 32);

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
		float scale = 1.f / w.magnitudeSquared();
		hostWeights.pushBack(PxVec4(w.x, w.y, w.z, scale));
	}

	DxContextLock contextLock(mCloth.mFactory);

	// todo: 'swap' these to force reallocation?
	mCloth.mVirtualParticleIndices = hostIndices;
	mCloth.mVirtualParticleSetSizes = scheduler.mSetSizes;
	mCloth.mVirtualParticleWeights = hostWeights;

	mCloth.notifyChanged();
	mCloth.wakeUp();
}

} // namespace cloth
} // namespace nv

#endif // NV_CLOTH_ENABLE_DX11
