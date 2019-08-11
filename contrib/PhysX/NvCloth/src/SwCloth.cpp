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

#include "SwCloth.h"
#include "SwFabric.h"
#include "SwFactory.h"
#include "TripletScheduler.h"
#include "ClothBase.h"
#include <foundation/PxMat44.h>
#include "NvCloth/Allocator.h"

using namespace physx;

namespace nv
{
namespace cloth
{
PhaseConfig transform(const PhaseConfig&); // from PhaseConfig.cpp
}
}

using namespace nv;

cloth::SwCloth::SwCloth(SwFactory& factory, SwFabric& fabric, Range<const PxVec4> particles)
: mFactory(factory), mFabric(fabric), mNumVirtualParticles(0), mUserData(0)
{
	NV_CLOTH_ASSERT(!particles.empty());

	initialize(*this, particles.begin(), particles.end());

#if PX_WINDOWS_FAMILY
	const uint32_t kSimdWidth = 8; // avx
#else
	const uint32_t kSimdWidth = 4; // sse
#endif

	NV_CLOTH_ASSERT(particles.size() == fabric.getNumParticles());

	mCurParticles.reserve(particles.size() + kSimdWidth - 1);
	mCurParticles.assign(reinterpret_cast<const PxVec4*>(particles.begin()),
	                     reinterpret_cast<const PxVec4*>(particles.end()));

	// 7 dummy particles used in SIMD solver
	mCurParticles.resize(particles.size() + kSimdWidth - 1, PxVec4(0.0f));
	mPrevParticles = mCurParticles;

	mCurParticles.resize(particles.size());
	mPrevParticles.resize(particles.size());

	mFabric.incRefCount();
}

namespace
{
// copy vector and make same capacity
void copyVector(nv::cloth::Vector<PxVec4>::Type& dst, const nv::cloth::Vector<PxVec4>::Type& src)
{
	dst.reserve(src.capacity());
	dst.assign(src.begin(), src.end());

	// ensure valid dummy data
	dst.resize(src.capacity(), PxVec4(0.0f));
	dst.resize(src.size());
}
}

// copy constructor, supports rebinding to a different factory
cloth::SwCloth::SwCloth(SwFactory& factory, const SwCloth& cloth)
: mFactory(factory)
, mFabric(cloth.mFabric)
, mPhaseConfigs(cloth.mPhaseConfigs)
, mCapsuleIndices(cloth.mCapsuleIndices)
, mStartCollisionSpheres(cloth.mStartCollisionSpheres)
, mTargetCollisionSpheres(cloth.mTargetCollisionSpheres)
, mStartCollisionPlanes(cloth.mStartCollisionPlanes)
, mTargetCollisionPlanes(cloth.mTargetCollisionPlanes)
, mStartCollisionTriangles(cloth.mStartCollisionTriangles)
, mTargetCollisionTriangles(cloth.mTargetCollisionTriangles)
, mVirtualParticleIndices(cloth.mVirtualParticleIndices)
, mVirtualParticleWeights(cloth.mVirtualParticleWeights)
, mNumVirtualParticles(cloth.mNumVirtualParticles)
, mSelfCollisionIndices(cloth.mSelfCollisionIndices)
, mRestPositions(cloth.mRestPositions)
{
	copy(*this, cloth);

	// carry over capacity (using as dummy particles)
	copyVector(mCurParticles, cloth.mCurParticles);
	copyVector(mPrevParticles, cloth.mPrevParticles);
	copyVector(mMotionConstraints.mStart, cloth.mMotionConstraints.mStart);
	copyVector(mMotionConstraints.mTarget, cloth.mMotionConstraints.mTarget);
	copyVector(mSeparationConstraints.mStart, cloth.mSeparationConstraints.mStart);
	copyVector(mSeparationConstraints.mTarget, cloth.mSeparationConstraints.mTarget);
	copyVector(mParticleAccelerations, cloth.mParticleAccelerations);

	//Both cloth and this have a reference to fabric. The factory that created fabric does not have to be the same as mFactory.
	//mFabric needs to outlive both cloth instances. (this is checked with refcount asserts).
	mFabric.incRefCount();
}

cloth::SwCloth::~SwCloth()
{
	mFabric.decRefCount();
}

// bounds = lower[3], upper[3]
void cloth::SwCloth::setParticleBounds(const float* bounds)
{
	for (uint32_t i = 0; i < 3; ++i)
	{
		array(mParticleBoundsCenter)[i] = (bounds[3 + i] + bounds[i]) * 0.5f;
		array(mParticleBoundsHalfExtent)[i] = (bounds[3 + i] - bounds[i]) * 0.5f;
	}
}

cloth::Range<PxVec4> cloth::SwCloth::push(SwConstraints& constraints)
{
	uint32_t n = uint32_t(mCurParticles.size());

	if (!constraints.mTarget.capacity())
		constraints.mTarget.resize((n + 3) & ~3, PxVec4(0.0f)); // reserve multiple of 4 for SIMD

	constraints.mTarget.resizeUninitialized(n);
	PxVec4* data = &constraints.mTarget.front();
	Range<PxVec4> result(data, data + constraints.mTarget.size());

	if (constraints.mStart.empty()) // initialize start first
		constraints.mStart.swap(constraints.mTarget);

	return result;
}

void cloth::SwCloth::clear(SwConstraints& constraints)
{
	Vector<PxVec4>::Type().swap(constraints.mStart);
	Vector<PxVec4>::Type().swap(constraints.mTarget);
}

cloth::Range<const PxVec3> cloth::SwCloth::clampTriangleCount(Range<const PxVec3> range, uint32_t)
{
	return range;
}

#include "ClothImpl.h"

namespace nv
{
namespace cloth
{

Cloth* SwCloth::clone(Factory& factory) const
{
	return factory.clone(*this);
}

uint32_t SwCloth::getNumParticles() const
{
	return uint32_t(mCurParticles.size());
}

void SwCloth::lockParticles() const
{
}

void SwCloth::unlockParticles() const
{
}

MappedRange<physx::PxVec4> SwCloth::getCurrentParticles()
{
	return getMappedParticles(&mCurParticles.front());
}

MappedRange<const physx::PxVec4> SwCloth::getCurrentParticles() const
{
	return getMappedParticles(&mCurParticles.front());
}

MappedRange<physx::PxVec4> SwCloth::getPreviousParticles()
{
	return getMappedParticles(&mPrevParticles.front());
}

MappedRange<const physx::PxVec4> SwCloth::getPreviousParticles() const
{
	return getMappedParticles(&mPrevParticles.front());
}

GpuParticles SwCloth::getGpuParticles()
{
	GpuParticles result = { 0, 0, 0 };
	return result;
}

void SwCloth::setPhaseConfig(Range<const PhaseConfig> configs)
{
	mPhaseConfigs.resize(0);

	// transform phase config to use in solver
	for (; !configs.empty(); configs.popFront())
		if (configs.front().mStiffness > 0.0f)
			mPhaseConfigs.pushBack(transform(configs.front()));

	wakeUp();
}

void SwCloth::setSelfCollisionIndices(Range<const uint32_t> indices)
{
	ContextLockType lock(mFactory);
	mSelfCollisionIndices.assign(indices.begin(), indices.end());
	notifyChanged();
	wakeUp();
}

uint32_t SwCloth::getNumVirtualParticles() const
{
	return uint32_t(mNumVirtualParticles);
}

Range<PxVec4> SwCloth::getParticleAccelerations()
{
	if (mParticleAccelerations.empty())
	{
		uint32_t n = uint32_t(mCurParticles.size());
		mParticleAccelerations.resize(n, PxVec4(0.0f));
	}

	wakeUp();

	PxVec4* data = &mParticleAccelerations.front();
	return Range<PxVec4>(data, data + mParticleAccelerations.size());
}

void SwCloth::clearParticleAccelerations()
{
	Vector<PxVec4>::Type().swap(mParticleAccelerations);
	wakeUp();
}

void SwCloth::setVirtualParticles(Range<const uint32_t[4]> indices, Range<const PxVec3> weights)
{
	mNumVirtualParticles = 0;

	// shuffle indices to form independent SIMD sets
	uint16_t numParticles = uint16_t(mCurParticles.size());
	TripletScheduler scheduler(indices); //the TripletScheduler makes a copy so indices is not modified
	scheduler.simd(numParticles, 4);

	mVirtualParticleIndices.swap(scheduler.mPaddedTriplets);

	// precompute 1/dot(w,w)
	Vector<PxVec4>::Type().swap(mVirtualParticleWeights); //clear and trim
	mVirtualParticleWeights.reserve(weights.size());
	for (; !weights.empty(); weights.popFront())
	{
		PxVec3 w = weights.front();
		float scale = 1 / w.magnitudeSquared();
		mVirtualParticleWeights.pushBack(PxVec4( w.x, w.y, w.z, scale ));
	}

	notifyChanged();
}

} // namespace cloth
} // namespace nv
