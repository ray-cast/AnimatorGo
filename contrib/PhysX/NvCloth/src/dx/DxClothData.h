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

#ifdef __cplusplus
#pragma once

#include <foundation/Px.h>
#include "Simd.h"

namespace nv
{
namespace cloth
{

class DxCloth;
template <typename>
struct IterationState;
#else  // __cplusplus
typedef unsigned int uint32_t;
typedef int int32_t;
#endif

static const uint32_t MaxParticlesInSharedMem = 1969;


struct DxPhaseConfig
{
	float mStiffness;
	float mStiffnessMultiplier;
	float mCompressionLimit;
	float mStretchLimit;

	uint32_t mFirstConstraint;
	uint32_t mNumConstraints;
};

struct DxConstraint
{
	float mRestvalue;
#ifdef __cplusplus
	uint16_t mFirstIndex;
	uint16_t mSecondIndex;
#else
	uint32_t mIndices; // 2x uint16_t
#endif
};

struct DxTether
{
#ifdef __cplusplus
	DxTether(uint16_t, uint16_t);
	uint16_t mAnchor;
	uint16_t mLength;
#else
	uint32_t mValue;   // 2x uint16_t
#endif
};

// reference to cloth instance bulk data (POD)
// should not need frequent updates (stored on device)
struct DxClothData
{
#ifdef __cplusplus
	DxClothData()
	{
	}
	DxClothData(DxCloth&);
#endif

	uint32_t mNumParticles;
	uint32_t mParticlesOffset;

	// fabric constraints
	uint32_t mNumPhases;
	uint32_t mPhaseConfigOffset;
	uint32_t mConstraintOffset;
	uint32_t mStiffnessOffset; //Offset inside per constraint stiffness buffer

	uint32_t mNumTethers;
	uint32_t mTetherOffset;
	float mTetherConstraintScale;

	uint32_t mNumTriangles;
	uint32_t mStartTriangleOffset;

	// motion constraint data
	float mMotionConstraintScale;
	float mMotionConstraintBias;

	// collision
	uint32_t mNumCapsules;
	uint32_t mCapsuleOffset;
	uint32_t mNumSpheres;

	uint32_t mNumPlanes;
	uint32_t mNumConvexes;
	uint32_t mConvexMasksOffset;

	uint32_t mNumCollisionTriangles;

	uint32_t mNumVirtualParticleSetSizes;

	uint32_t mEnableContinuousCollision; //bool stored in uint32_t for dx alignment
	float mCollisionMassScale;
	float mFrictionScale;

	float mSelfCollisionDistance;

	uint32_t mNumSelfCollisionIndices;
	uint32_t mSelfCollisionIndicesOffset;
	uint32_t mSelfCollisionParticlesOffset;
	uint32_t mSelfCollisionDataOffset;

	// sleep data
	uint32_t mSleepTestInterval;
	uint32_t mSleepAfterCount;
	float mSleepThreshold;
};

// per-frame data (stored in pinned memory)
struct DxFrameData
{
#ifdef __cplusplus
	DxFrameData()
	{
	} // not initializing pointers to 0!

	explicit DxFrameData(DxCloth&, uint32_t numSharedPositions, const IterationState<Simd4f>&, uint32_t firstIteration);
#endif

	bool mDeviceParticlesDirty;

	// number of particle copies that fit in shared memory (0, 1, or 2)
	uint32_t mNumSharedPositions;

	// iteration data
	float mIterDt;
	uint32_t mFirstIteration;
	uint32_t mNumIterations;

	float mTetherConstraintStiffness;

	// wind data
	float mDragCoefficient;
	float mLiftCoefficient;
	float mFluidDensity;
	float mRotation[9];

	// motion constraint data
	float mMotionConstraintStiffness;
	uint32_t mStartMotionConstrainsOffset;
	uint32_t mTargetMotionConstrainsOffset;

	// separation constraint data
	uint32_t mStartSeparationConstrainsOffset;
	uint32_t mTargetSeparationConstrainsOffset;

	// particle acceleration data
	uint32_t mParticleAccelerationsOffset;

	uint32_t mStartSphereOffset;
	uint32_t mTargetSphereOffset;

	uint32_t mStartCollisionPlaneOffset;
	uint32_t mTargetCollisionPlaneOffset;

	uint32_t mStartCollisionTrianglesOffset;
	uint32_t mTargetCollisionTrianglesOffset;

	float mSelfCollisionStiffness;

	float mParticleBounds[6]; // maxX, -minX, maxY, ...

	uint32_t mSleepPassCounter;
	uint32_t mSleepTestCounter;

	float mStiffnessExponent;

	uint32_t mRestPositionsOffset;

	bool mInitSelfCollisionData;
};

// per-iteration data (stored in pinned memory)
struct DxIterationData
{
#ifdef __cplusplus
	DxIterationData()
	{
	} // not initializing!
	explicit DxIterationData(const IterationState<Simd4f>&);
#endif
	float mIntegrationTrafo[24];
	float mWind[3];
	uint32_t mIsTurning;
};

#ifdef __cplusplus
} // namespace cloth
} // namespace nv
#endif
