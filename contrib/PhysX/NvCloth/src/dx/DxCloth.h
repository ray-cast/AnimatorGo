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

#pragma once

#include "NvCloth/Range.h"
#include "NvCloth/PhaseConfig.h"
#include "../MovingAverage.h"
#include "../IndexPair.h"
#include "../Vec4T.h"
#include "DxClothData.h"
#include "DxContextLock.h"
#include "DxBatchedVector.h"
#include <foundation/PxVec4.h>
#include <foundation/PxVec3.h>
#include <foundation/PxTransform.h>

namespace nv
{
namespace cloth
{

class DxFabric;
class DxFactory;
struct DxClothData;

struct DxConstraints
{
	DxConstraints::DxConstraints(DxBatchedStorage<physx::PxVec4>& storage)
		: mStart(storage), mTarget(storage)
	{
	}

	void pop()
	{
		if (!mTarget.empty())
		{
			mStart.swap(mTarget);
			mTarget.resize(0);
		}
	}

	DxBatchedVector<physx::PxVec4> mStart;
	DxBatchedVector<physx::PxVec4> mTarget;
	Vector<physx::PxVec4>::Type mHostCopy;
};

class DxCloth : protected DxContextLock
{
	DxCloth(const DxCloth&); // not implemented
	DxCloth& operator = (const DxCloth&); // not implemented

  public:
	typedef DxFactory FactoryType;
	typedef DxFabric FabricType;
	typedef DxContextLock ContextLockType;

	template <typename>
	struct MapTraits;

	typedef DxVectorMap<DxBatchedVector<physx::PxVec3> > MappedVec3fVectorType;
	typedef DxVectorMap<DxBatchedVector<physx::PxVec4> > MappedVec4fVectorType;
	typedef DxVectorMap<DxBatchedVector<IndexPair> > MappedIndexVectorType;
	typedef DxVectorMap<DxBatchedVector<uint32_t> > MappedMaskVectorType;

	DxCloth(DxFactory&, DxFabric&, Range<const physx::PxVec4>);
	DxCloth(DxFactory&, const DxCloth&);
	~DxCloth(); // not virtual on purpose

  public:
	bool isSleeping() const
	{
		return mSleepPassCounter >= mSleepAfterCount;
	}
	void wakeUp()
	{
		mSleepPassCounter = 0;
	}

	void notifyChanged();

	bool updateClothData(DxClothData&);   // expects acquired context
	uint32_t getSharedMemorySize() const; // without particle data

	// expects transformed configs, doesn't call notifyChanged()
	void setPhaseConfig(Range<const PhaseConfig>);

	Range<physx::PxVec4> push(DxConstraints&);
	void clear(DxConstraints&);

	void mapParticles();
	void unmapParticles();

	Range<const physx::PxVec3> clampTriangleCount(Range<const physx::PxVec3>, uint32_t);

  public:
	DxFactory& mFactory;
	DxFabric& mFabric;

	bool mClothDataDirty;
	bool mClothCostDirty;

	// particle data
	uint32_t mNumParticles;
	DxBatchedVector<physx::PxVec4> mParticles; // cur, prev
	DxBatchedVector<physx::PxVec4> mParticlesHostCopy;
	physx::PxVec4* mParticlesMapPointer;
	uint32_t mParticlesMapRefCount;

	bool mDeviceParticlesDirty;
	bool mHostParticlesDirty;

	physx::PxVec3 mParticleBoundsCenter;
	physx::PxVec3 mParticleBoundsHalfExtent;

	physx::PxVec3 mGravity;
	physx::PxVec3 mLogDamping;
	physx::PxVec3 mLinearLogDrag;
	physx::PxVec3 mAngularLogDrag;
	physx::PxVec3 mLinearInertia;
	physx::PxVec3 mAngularInertia;
	physx::PxVec3 mCentrifugalInertia;
	float mSolverFrequency;
	float mStiffnessFrequency;

	physx::PxTransform mTargetMotion;
	physx::PxTransform mCurrentMotion;
	physx::PxVec3 mLinearVelocity;
	physx::PxVec3 mAngularVelocity;

	float mPrevIterDt;
	MovingAverage mIterDtAvg;

	DxBatchedVector<DxPhaseConfig> mPhaseConfigs;
	Vector<PhaseConfig>::Type mHostPhaseConfigs;

	// tether constraints stuff
	float mTetherConstraintLogStiffness;
	float mTetherConstraintScale;

	// motion constraints stuff
	DxConstraints mMotionConstraints;
	float mMotionConstraintScale;
	float mMotionConstraintBias;
	float mMotionConstraintLogStiffness;

	// separation constraints stuff
	DxConstraints mSeparationConstraints;

	// particle acceleration stuff
	DxBatchedVector<physx::PxVec4> mParticleAccelerations;
	Vector<physx::PxVec4>::Type mParticleAccelerationsHostCopy;

	// wind
	physx::PxVec3 mWind;
	float mDragLogCoefficient;
	float mLiftLogCoefficient;

	// collision stuff
	DxBatchedVector<IndexPair> mCapsuleIndices;
	DxBatchedVector<physx::PxVec4> mStartCollisionSpheres;
	DxBatchedVector<physx::PxVec4> mTargetCollisionSpheres;
	DxBatchedVector<uint32_t> mConvexMasks;
	DxBatchedVector<physx::PxVec4> mStartCollisionPlanes;
	DxBatchedVector<physx::PxVec4> mTargetCollisionPlanes;
	DxBatchedVector<physx::PxVec3> mStartCollisionTriangles;
	DxBatchedVector<physx::PxVec3> mTargetCollisionTriangles;
	bool mEnableContinuousCollision;
	float mCollisionMassScale;
	float mFriction;

	// virtual particles
	DxDeviceVector<uint32_t> mVirtualParticleSetSizes;
	DxDeviceVector<Vec4us> mVirtualParticleIndices;
	DxDeviceVector<physx::PxVec4> mVirtualParticleWeights;

	// self collision
	float mSelfCollisionDistance;
	float mSelfCollisionLogStiffness;

	DxBatchedVector<physx::PxVec4> mRestPositions;
	DxBatchedVector<uint32_t> mSelfCollisionIndices;
	Vector<uint32_t>::Type mSelfCollisionIndicesHost;

	DxBatchedVector<physx::PxVec4> mSelfCollisionParticles;
	// 2x(key) per particle + cellStart (8322)
	DxBatchedVector<uint32_t> mSelfCollisionData;

	bool mInitSelfCollisionData;

	// sleeping
	uint32_t mSleepTestInterval; // how often to test for movement
	uint32_t mSleepAfterCount;   // number of tests to pass before sleep
	float mSleepThreshold;       // max movement delta to pass test
	uint32_t mSleepPassCounter;  // how many tests passed
	uint32_t mSleepTestCounter;  // how many iterations since tested

	uint32_t mSharedMemorySize;

	void* mUserData;
};
}
}
