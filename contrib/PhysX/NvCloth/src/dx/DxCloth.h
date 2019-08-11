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
#include "DxFactory.h"
#include "DxFabric.h"
#include "ClothImpl.h"

namespace nv
{
namespace cloth
{

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

template<>
class ClothTraits<DxCloth>
{
public:
	typedef DxFactory FactoryType;
	typedef DxFabric FabricType;
	typedef DxContextLock ContextLockType;
};

class DxCloth : protected DxContextLock, public ClothImpl<DxCloth>
{
	DxCloth(const DxCloth&); // not implemented
	DxCloth& operator = (const DxCloth&); // not implemented

  public:
	typedef DxFactory FactoryType;
	typedef DxFabric FabricType;
	typedef DxContextLock ContextLockType;

	typedef DxVectorMap<DxBatchedVector<physx::PxVec3> > MappedVec3fVectorType;
	typedef DxVectorMap<DxBatchedVector<physx::PxVec4> > MappedVec4fVectorType;
	typedef DxVectorMap<DxBatchedVector<Vec4us> > MappedVec4usVectorType;
	typedef DxVectorMap<DxBatchedVector<IndexPair> > MappedIndexVectorType;
	typedef DxVectorMap<DxBatchedVector<uint32_t> > MappedMaskVectorType;

	DxCloth(DxFactory&, DxFabric&, Range<const physx::PxVec4>);
	DxCloth(DxFactory&, const DxCloth&);
	~DxCloth(); // not virtual on purpose

  public:
	virtual Cloth* clone(Factory& factory) const;
	uint32_t getNumParticles() const;

	void lockParticles() const;
	void unlockParticles() const;

	MappedRange<physx::PxVec4> getCurrentParticles();
	MappedRange<const physx::PxVec4> getCurrentParticles() const;
	MappedRange<physx::PxVec4> getPreviousParticles();
	MappedRange<const physx::PxVec4> getPreviousParticles() const;
	GpuParticles getGpuParticles();

	void setPhaseConfig(Range<const PhaseConfig> configs);
	void setSelfCollisionIndices(Range<const uint32_t> indices);
	uint32_t getNumVirtualParticles() const;
	Range<physx::PxVec4> getParticleAccelerations();
	void clearParticleAccelerations();
	void setVirtualParticles(Range<const uint32_t[4]> indices, Range<const physx::PxVec3> weights);

	void notifyChanged();

	bool updateClothData(DxClothData&);   // expects acquired context
	uint32_t getSharedMemorySize() const; // without particle data

	// expects transformed configs, doesn't call notifyChanged()
	void setPhaseConfigInternal(Range<const PhaseConfig>);

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
	DxBatchedVector<uint32_t> mVirtualParticleSetSizes;
	DxBatchedVector<Vec4us> mVirtualParticleIndices;
	DxBatchedVector<physx::PxVec4> mVirtualParticleWeights;

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

	uint32_t mSharedMemorySize;

	void* mUserData;
};
}
}
