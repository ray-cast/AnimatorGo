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

#include "DxClothData.h"
#include "DxCloth.h"
#include "DxFabric.h"
#include "DxContextLock.h"
#include "../IterationState.h"

#if NV_CLOTH_ENABLE_DX11

using namespace nv;

cloth::DxTether::DxTether(uint16_t anchor, uint16_t length) : mAnchor(anchor), mLength(length)
{
}

cloth::DxClothData::DxClothData(DxCloth& cloth)
{
	mNumParticles = cloth.mNumParticles;
	mParticlesOffset = cloth.mParticles.mOffset;

	mNumPhases = cloth.mPhaseConfigs.size();
	mPhaseConfigOffset = cloth.mPhaseConfigs.mOffset;
	mConstraintOffset = cloth.mFabric.mConstraints.mOffset;
	mStiffnessOffset = cloth.mFabric.mStiffnessValues.empty() ? -1: cloth.mFabric.mStiffnessValues.mOffset;

	mNumTriangles = cloth.mFabric.getNumTriangles();
	mStartTriangleOffset = cloth.mFabric.mTriangles.mOffset;

	mNumTethers = cloth.mFabric.mTethers.size();
	mTetherOffset = cloth.mFabric.mTethers.mOffset;
	mTetherConstraintScale = cloth.mTetherConstraintScale * cloth.mFabric.mTetherLengthScale;

	mMotionConstraintScale = cloth.mMotionConstraintScale;
	mMotionConstraintBias = cloth.mMotionConstraintBias;

	mNumCapsules = cloth.mCapsuleIndices.size();
	mCapsuleOffset = cloth.mCapsuleIndices.mOffset;
	mNumSpheres = cloth.mStartCollisionSpheres.size();

	mNumPlanes = cloth.mStartCollisionPlanes.size();
	mNumConvexes = cloth.mConvexMasks.size();
	mConvexMasksOffset = cloth.mConvexMasks.mOffset;

	mNumCollisionTriangles = uint32_t(cloth.mStartCollisionTriangles.size()) / 3;

	mNumVirtualParticleSetSizes = cloth.mVirtualParticleSetSizes.size();

	mEnableContinuousCollision = cloth.mEnableContinuousCollision;
	mCollisionMassScale = cloth.mCollisionMassScale;
	mFrictionScale = cloth.mFriction;

	mSelfCollisionDistance = cloth.mSelfCollisionDistance;
	mNumSelfCollisionIndices = cloth.mSelfCollisionIndices.empty() ? mNumParticles : cloth.mSelfCollisionIndices.size();
	mSelfCollisionIndicesOffset = cloth.mSelfCollisionIndices.empty() ? uint32_t(-1) : cloth.mSelfCollisionIndices.mOffset;
	mSelfCollisionParticlesOffset = cloth.mSelfCollisionParticles.mOffset;
	mSelfCollisionDataOffset = cloth.mSelfCollisionData.mOffset;

	mSleepTestInterval = cloth.mSleepTestInterval;
	mSleepAfterCount = cloth.mSleepAfterCount;
	mSleepThreshold = cloth.mSleepThreshold;
}

cloth::DxFrameData::DxFrameData(DxCloth& cloth, uint32_t numSharedPositions, const IterationState<Simd4f>& state, uint32_t firstIteration)
{
	mDeviceParticlesDirty = cloth.mDeviceParticlesDirty;

	mNumSharedPositions = numSharedPositions;

	mIterDt = state.mIterDt;

	mFirstIteration = firstIteration;
	mNumIterations = state.mRemainingIterations;

	Simd4f stiffnessExponent = simd4f(cloth.mStiffnessFrequency * mIterDt);
	{
		Simd4f logStiffness = simd4f(0.0f, cloth.mSelfCollisionLogStiffness, cloth.mMotionConstraintLogStiffness,
			cloth.mTetherConstraintLogStiffness);
		Simd4f stiffness = gSimd4fOne - exp2(logStiffness * stiffnessExponent);

		mTetherConstraintStiffness = array(stiffness)[3];
		mMotionConstraintStiffness = array(stiffness)[2];
		mSelfCollisionStiffness = array(stiffness)[1];
	}
	{
		Simd4f logStiffness = simd4f(cloth.mDragLogCoefficient, cloth.mLiftLogCoefficient, 0.0f, 0.0f);
		Simd4f stiffness = gSimd4fOne - exp2(logStiffness * stiffnessExponent);
		mDragCoefficient = array(stiffness)[0];
		mLiftCoefficient = array(stiffness)[1];
		mFluidDensity = cloth.mFluidDensity * 0.5f; //divide by 2 to so we don't have to compensate for double area from cross product in the solver
		for(int i = 0; i < 9; ++i)
			mRotation[i] = array(state.mRotationMatrix[i / 3])[i % 3];
	}

	mStartSphereOffset = cloth.mStartCollisionSpheres.mOffset;
	mTargetSphereOffset =
	    cloth.mTargetCollisionSpheres.empty() ? mStartSphereOffset : cloth.mTargetCollisionSpheres.mOffset;

	mStartCollisionPlaneOffset = cloth.mStartCollisionPlanes.mOffset;
	mTargetCollisionPlaneOffset =
		cloth.mTargetCollisionPlanes.empty() ? mStartCollisionPlaneOffset : cloth.mTargetCollisionPlanes.mOffset;

	mStartCollisionTrianglesOffset = cloth.mStartCollisionTriangles.mOffset;
	mTargetCollisionTrianglesOffset =
		cloth.mTargetCollisionTriangles.empty() ? mStartCollisionTrianglesOffset : cloth.mTargetCollisionTriangles.mOffset;

	for (uint32_t i = 0; i < 3; ++i)
	{
		float c = array(cloth.mParticleBoundsCenter)[i];
		float r = array(cloth.mParticleBoundsHalfExtent)[i];
		mParticleBounds[i * 2 + 0] = r + c;
		mParticleBounds[i * 2 + 1] = r - c;
	}

	mSleepPassCounter = cloth.mSleepPassCounter;
	mSleepTestCounter = cloth.mSleepTestCounter;

	mStiffnessExponent = cloth.mStiffnessFrequency * mIterDt;

	mStartMotionConstrainsOffset = cloth.mMotionConstraints.mStart.empty() ? uint32_t(-1) : cloth.mMotionConstraints.mStart.mOffset;
	mTargetMotionConstrainsOffset = cloth.mMotionConstraints.mTarget.empty() ? mStartMotionConstrainsOffset : cloth.mMotionConstraints.mTarget.mOffset;

	mStartSeparationConstrainsOffset = cloth.mSeparationConstraints.mStart.empty() ? uint32_t(-1) : cloth.mSeparationConstraints.mStart.mOffset;
	mTargetSeparationConstrainsOffset = cloth.mSeparationConstraints.mTarget.empty() ? mStartSeparationConstrainsOffset : cloth.mSeparationConstraints.mTarget.mOffset;

	mParticleAccelerationsOffset = cloth.mParticleAccelerations.mOffset;
	mRestPositionsOffset = cloth.mRestPositions.empty() ? uint32_t(-1) : cloth.mRestPositions.mOffset;

	mInitSelfCollisionData = cloth.mInitSelfCollisionData;
	cloth.mInitSelfCollisionData = false;
}

namespace
{
void copySquareTransposed(float* dst, const float* src)
{
	dst[0] = src[0];
	dst[1] = src[4];
	dst[2] = src[8];
	dst[3] = src[1];
	dst[4] = src[5];
	dst[5] = src[9];
	dst[6] = src[2];
	dst[7] = src[6];
	dst[8] = src[10];
}
}

cloth::DxIterationData::DxIterationData(const IterationState<Simd4f>& state)
{
	mIntegrationTrafo[0] = array(state.mPrevBias)[0];
	mIntegrationTrafo[1] = array(state.mPrevBias)[1];
	mIntegrationTrafo[2] = array(state.mPrevBias)[2];

	mIntegrationTrafo[3] = array(state.mCurBias)[0];
	mIntegrationTrafo[4] = array(state.mCurBias)[1];
	mIntegrationTrafo[5] = array(state.mCurBias)[2];

	copySquareTransposed(mIntegrationTrafo + 6, array(*state.mPrevMatrix));
	copySquareTransposed(mIntegrationTrafo + 15, array(*state.mCurMatrix));

	mIsTurning = uint32_t(state.mIsTurning);

	mWind[0] = array(state.mWind)[0];
	mWind[1] = array(state.mWind)[1];
	mWind[2] = array(state.mWind)[2];
}

#endif // NV_CLOTH_ENABLE_DX11
