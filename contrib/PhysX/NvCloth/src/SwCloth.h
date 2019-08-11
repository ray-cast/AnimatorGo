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
#include "MovingAverage.h"
#include "IndexPair.h"
#include "Vec4T.h"
#include <foundation/PxVec4.h>
#include <foundation/PxVec3.h>
#include <foundation/PxTransform.h>
#include "SwFactory.h"
#include "SwFabric.h"
#include "ClothImpl.h"

namespace nv
{

namespace cloth
{

struct SwConstraints
{
	void pop()
	{
		if (!mTarget.empty())
		{
			mStart.swap(mTarget);
			mTarget.resize(0);
		}
	}

	Vector<physx::PxVec4>::Type mStart;
	Vector<physx::PxVec4>::Type mTarget;
};

struct SwContextLock
{
	SwContextLock(const SwFactory&)
	{
	}
};

class SwCloth;

template<>
class ClothTraits<SwCloth>
{
public:
	typedef SwFactory FactoryType;
	typedef SwFabric FabricType;
	typedef SwContextLock ContextLockType;
};

class SwCloth : public ClothImpl<SwCloth>
{
	SwCloth& operator = (const SwCloth&); // not implemented

  public:
	typedef SwFactory FactoryType;
	typedef SwFabric FabricType;
	typedef SwContextLock ContextLockType;

	typedef Vector<physx::PxVec3>::Type& MappedVec3fVectorType;
	typedef Vector<physx::PxVec4>::Type& MappedVec4fVectorType;
	typedef Vector<IndexPair>::Type& MappedIndexVectorType;
	typedef Vector<uint32_t>::Type& MappedMaskVectorType;

	SwCloth(SwFactory&, SwFabric&, Range<const physx::PxVec4>);
	SwCloth(SwFactory&, const SwCloth&);
	~SwCloth(); // not virtual on purpose

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

	void notifyChanged()
	{
	}

	void setParticleBounds(const float*);

	Range<physx::PxVec4> push(SwConstraints&);
	static void clear(SwConstraints&);

	static Range<const physx::PxVec3> clampTriangleCount(Range<const physx::PxVec3>, uint32_t);

  public:
	SwFactory& mFactory;
	SwFabric& mFabric;

	bool mClothCostDirty;

	// current and previous-iteration particle positions
	Vector<physx::PxVec4>::Type mCurParticles;
	Vector<physx::PxVec4>::Type mPrevParticles;

	Vector<PhaseConfig>::Type mPhaseConfigs; // transformed!

	// tether constraints stuff
	float mTetherConstraintLogStiffness;
	float mTetherConstraintScale;

	// motion constraints stuff
	SwConstraints mMotionConstraints;
	float mMotionConstraintScale;
	float mMotionConstraintBias;
	float mMotionConstraintLogStiffness;

	// separation constraints stuff
	SwConstraints mSeparationConstraints;

	// particle acceleration stuff
	Vector<physx::PxVec4>::Type mParticleAccelerations;

	// collision stuff
	Vector<IndexPair>::Type mCapsuleIndices;
	Vector<physx::PxVec4>::Type mStartCollisionSpheres;
	Vector<physx::PxVec4>::Type mTargetCollisionSpheres;
	Vector<uint32_t>::Type mConvexMasks;
	Vector<physx::PxVec4>::Type mStartCollisionPlanes;
	Vector<physx::PxVec4>::Type mTargetCollisionPlanes;
	Vector<physx::PxVec3>::Type mStartCollisionTriangles;
	Vector<physx::PxVec3>::Type mTargetCollisionTriangles;
	bool mEnableContinuousCollision;
	float mCollisionMassScale;
	float mFriction;

	// virtual particles
	Vector<Vec4us>::Type mVirtualParticleIndices;
	Vector<physx::PxVec4>::Type mVirtualParticleWeights;
	uint32_t mNumVirtualParticles;

	// self collision
	float mSelfCollisionDistance;
	float mSelfCollisionLogStiffness;

	Vector<uint32_t>::Type mSelfCollisionIndices;

	Vector<physx::PxVec4>::Type mRestPositions;

	// unused for CPU simulation
	void* mUserData;
};

} // namespace cloth
}
