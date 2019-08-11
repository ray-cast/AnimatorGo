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
#include "../BoundingBox.h"
#include "../Vec4T.h"
#include "CuPhaseConfig.h"
#include "CuPinnedAllocator.h"
#include "CuContextLock.h"
#include "CuDeviceVector.h"
#include <foundation/PxVec4.h>
#include <foundation/PxVec3.h>
#include <foundation/PxTransform.h>
#include "NvCloth/Allocator.h"
#include "ClothImpl.h"
#include "CuFactory.h"

namespace nv
{
namespace cloth
{

class CuFabric;
struct CuClothData;

struct CuConstraints
{
	CuConstraints(CUcontext ctx)
	: mStart(ctx), mTarget(ctx), mHostCopy(ctx)
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

	CuDeviceVector<physx::PxVec4> mStart;
	CuDeviceVector<physx::PxVec4> mTarget;
	CuHostVector<physx::PxVec4, CU_MEMHOSTALLOC_DEVICEMAP>::Type mHostCopy;
};

class CuCloth;

template<>
class ClothTraits<CuCloth>
{
public:
	typedef CuFactory FactoryType;
	typedef CuFabric FabricType;
	typedef CuContextLock ContextLockType;
};

class CuCloth : protected CuContextLock, public ClothImpl<CuCloth>
{
	CuCloth(); // not implemented

  public:
	CuCloth& operator = (const CuCloth&);
	typedef CuFactory FactoryType;
	typedef CuFabric FabricType;
	typedef CuContextLock ContextLockType;

	typedef CuHostVector<physx::PxVec3, CU_MEMHOSTALLOC_DEVICEMAP>::Type& MappedVec3fVectorType;
	typedef CuHostVector<physx::PxVec4, CU_MEMHOSTALLOC_DEVICEMAP>::Type& MappedVec4fVectorType;
	typedef CuHostVector<IndexPair, CU_MEMHOSTALLOC_DEVICEMAP>::Type& MappedIndexVectorType;
	typedef CuHostVector<uint32_t, CU_MEMHOSTALLOC_DEVICEMAP>::Type& MappedMaskVectorType;

	CuCloth(CuFactory&, CuFabric&, Range<const physx::PxVec4>);
	CuCloth(CuFactory&, const CuCloth&);
	~CuCloth(); // not virtual on purpose

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

	bool updateClothData(CuClothData&);   // expects acquired context
	uint32_t getSharedMemorySize() const; // without particle data

	// expects transformed configs, doesn't call notifyChanged()
	void setPhaseConfigInternal(Range<const PhaseConfig> configs);

	Range<physx::PxVec4> push(CuConstraints&);
	void clear(CuConstraints&);

	void syncDeviceParticles();
	void syncHostParticles();

	Range<const physx::PxVec3> clampTriangleCount(Range<const physx::PxVec3>, uint32_t);

  public:
	CuFactory& mFactory;
	CuFabric& mFabric;

	bool mClothDataDirty;
	bool mClothCostDirty;

	// particle data
	uint32_t mNumParticles;
	CuDeviceVector<physx::PxVec4> mParticles; // cur, prev
	CuHostVector<physx::PxVec4, CU_MEMHOSTALLOC_DEVICEMAP>::Type mParticlesHostCopy;
	bool mDeviceParticlesDirty;
	bool mHostParticlesDirty;

	CuDeviceVector<CuPhaseConfig> mPhaseConfigs; // transformed!
	Vector<PhaseConfig>::Type mHostPhaseConfigs; // transformed!

	// tether constraints stuff
	float mTetherConstraintLogStiffness;
	float mTetherConstraintScale;

	// motion constraints stuff
	CuConstraints mMotionConstraints;
	float mMotionConstraintScale;
	float mMotionConstraintBias;
	float mMotionConstraintLogStiffness;

	// separation constraints stuff
	CuConstraints mSeparationConstraints;

	// particle acceleration stuff
	CuDeviceVector<physx::PxVec4> mParticleAccelerations;
	CuHostVector<physx::PxVec4, CU_MEMHOSTALLOC_DEVICEMAP>::Type mParticleAccelerationsHostCopy;

	// collision stuff
	CuHostVector<IndexPair, CU_MEMHOSTALLOC_DEVICEMAP>::Type mCapsuleIndices;
	CuHostVector<physx::PxVec4, CU_MEMHOSTALLOC_DEVICEMAP>::Type mStartCollisionSpheres;
	CuHostVector<physx::PxVec4, CU_MEMHOSTALLOC_DEVICEMAP>::Type mTargetCollisionSpheres;
	CuHostVector<uint32_t, CU_MEMHOSTALLOC_DEVICEMAP>::Type mConvexMasks;
	CuHostVector<physx::PxVec4, CU_MEMHOSTALLOC_DEVICEMAP>::Type mStartCollisionPlanes;
	CuHostVector<physx::PxVec4, CU_MEMHOSTALLOC_DEVICEMAP>::Type mTargetCollisionPlanes;
	CuHostVector<physx::PxVec3, CU_MEMHOSTALLOC_DEVICEMAP>::Type mStartCollisionTriangles;
	CuHostVector<physx::PxVec3, CU_MEMHOSTALLOC_DEVICEMAP>::Type mTargetCollisionTriangles;
	bool mEnableContinuousCollision;
	float mCollisionMassScale;
	float mFriction;

	// virtual particles
	CuDeviceVector<uint32_t> mVirtualParticleSetSizes;
	CuDeviceVector<Vec4us> mVirtualParticleIndices;
	CuDeviceVector<physx::PxVec4> mVirtualParticleWeights;

	// self collision
	float mSelfCollisionDistance;
	float mSelfCollisionLogStiffness;

	CuDeviceVector<physx::PxVec4> mRestPositions;
	CuDeviceVector<uint32_t> mSelfCollisionIndices;
	Vector<uint32_t>::Type mSelfCollisionIndicesHost;

	// 4 (position) + 2 (key) per particle + cellStart (8322)
	CuDeviceVector<float> mSelfCollisionData;

	uint32_t mSharedMemorySize;

	void* mUserData;
};
}
}
