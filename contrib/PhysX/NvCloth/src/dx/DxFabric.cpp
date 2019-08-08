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

#include "DxFabric.h"
#include "DxContextLock.h"
#include "DxFactory.h"
#include <algorithm>
#include <PsUtilities.h>

#if NV_CLOTH_ENABLE_DX11

using namespace physx;
using namespace nv;

cloth::DxFabric::DxFabric(DxFactory& factory, uint32_t numParticles, Range<const uint32_t> phaseIndices,
                          Range<const uint32_t> sets, Range<const float> restvalues, Range<const float> stiffnessValues, Range<const uint32_t> indices,
                          Range<const uint32_t> anchors, Range<const float> tetherLengths,
                          Range<const uint32_t> triangles, uint32_t id)
: DxContextLock(factory)
, mFactory(factory)
, mNumParticles(numParticles)
, mPhases(phaseIndices.begin(), phaseIndices.end())
, mSets(sets.begin(), sets.end())
, mConstraints(mFactory.mConstraints)
, mConstraintsHostCopy(mFactory.mConstraintsHostCopy)
, mStiffnessValues(mFactory.mStiffnessValues)
, mTethers(mFactory.mTethers)
, mId(id)
{
	// should no longer be prefixed with 0
	NV_CLOTH_ASSERT(sets.front() != 0);

	NV_CLOTH_ASSERT(sets.back() == restvalues.size());
	NV_CLOTH_ASSERT(restvalues.size() * 2 == indices.size());
	NV_CLOTH_ASSERT(restvalues.size() == stiffnessValues.size() || stiffnessValues.size() == 0);
	NV_CLOTH_ASSERT(mNumParticles > *shdfnd::maxElement(indices.begin(), indices.end()));

	// manually convert uint32_t indices to uint16_t in temp memory
	Vector<DxConstraint>::Type hostConstraints;
	hostConstraints.resizeUninitialized(restvalues.size());
	Vector<DxConstraint>::Type::Iterator cIt = hostConstraints.begin();
	Vector<DxConstraint>::Type::Iterator cEnd = hostConstraints.end();

	const uint32_t* iIt = indices.begin();
	const float* rIt = restvalues.begin();
	for (; cIt != cEnd; ++cIt)
	{
		cIt->mRestvalue = *rIt++;
		cIt->mFirstIndex = uint16_t(*iIt++);
		cIt->mSecondIndex = uint16_t(*iIt++);
	}

	// copy to device vector in one go
#if 0
	// Workaround for NvAPI SCG device updateSubresource size limit
	mConstraintsHostCopy.assign(hostConstraints.begin(), hostConstraints.end());
	mConstraints.resize(mConstraintsHostCopy.size());
	mConstraints = mConstraintsHostCopy;
#else
	mConstraints.assign(hostConstraints.begin(), hostConstraints.end());
#endif

	mStiffnessValues.assign(stiffnessValues.begin(), stiffnessValues.end());

	// gather data per phase
	mFirstConstraintInPhase.reserve(phaseIndices.size());
	mNumConstraintsInPhase.reserve(phaseIndices.size());
	for (const uint32_t* pIt = phaseIndices.begin(); pIt != phaseIndices.end(); ++pIt)
	{
		uint32_t setIndex = *pIt;
		uint32_t firstIndex = setIndex ? sets[setIndex - 1] : 0;
		uint32_t lastIndex = sets[setIndex];
		mFirstConstraintInPhase.pushBack(firstIndex);
		mNumConstraintsInPhase.pushBack(lastIndex - firstIndex);
	}

	// tethers
	NV_CLOTH_ASSERT(anchors.size() == tetherLengths.size());
	mTetherLengthScale =
	    tetherLengths.empty() ? 1.0f : *shdfnd::maxElement(tetherLengths.begin(), tetherLengths.end()) / USHRT_MAX;
	float inverseScale = 1 / (mTetherLengthScale + FLT_EPSILON);
	Vector<DxTether>::Type tethers;
	tethers.reserve(anchors.size());
	for (; !anchors.empty(); anchors.popFront(), tetherLengths.popFront())
	{
		tethers.pushBack(DxTether(uint16_t(anchors.front()), uint16_t(tetherLengths.front() * inverseScale + 0.5f)));
	}
	mTethers.assign(tethers.begin(), tethers.end());

	// triangles
	Vector<uint16_t>::Type hostTriangles;
	hostTriangles.resizeUninitialized(triangles.size());
	Vector<uint16_t>::Type::Iterator tIt = hostTriangles.begin();

	for (; !triangles.empty(); triangles.popFront())
		*tIt++ = uint16_t(triangles.front());

	mTriangles.assign(hostTriangles.begin(), hostTriangles.end());

	DxContextLock::release();

	// add to factory
	mFactory.mFabrics.pushBack(this);
}

cloth::DxFabric::~DxFabric()
{
	DxContextLock::acquire();

	Vector<DxFabric*>::Type::Iterator fIt = mFactory.mFabrics.find(this);

	NV_CLOTH_ASSERT(fIt != mFactory.mFabrics.end());
	mFactory.mFabrics.replaceWithLast(fIt);
}

cloth::Factory& cloth::DxFabric::getFactory() const
{
	return mFactory;
}

uint32_t cloth::DxFabric::getNumPhases() const
{
	return uint32_t(mPhases.size());
}

uint32_t cloth::DxFabric::getNumRestvalues() const
{
	return uint32_t(mConstraints.size());
}

uint32_t cloth::DxFabric::getNumStiffnessValues() const
{
	return uint32_t(mStiffnessValues.size());
}

uint32_t cloth::DxFabric::getNumSets() const
{
	return uint32_t(mSets.size());
}

uint32_t cloth::DxFabric::getNumIndices() const
{
	return uint32_t(mConstraints.size()) * 2;
}

uint32_t cloth::DxFabric::getNumParticles() const
{
	return mNumParticles;
}

uint32_t cloth::DxFabric::getNumTethers() const
{
	return uint32_t(mTethers.size());
}

uint32_t cloth::DxFabric::getNumTriangles() const
{
	return uint32_t(mTriangles.size()) / 3;
}

void cloth::DxFabric::scaleRestvalues(float scale)
{
	DxContextLock contextLock(mFactory);

	Vector<DxConstraint>::Type constraints(uint32_t(mConstraints.size()));
	mFactory.copyToHost(constraints.begin(), mConstraints.buffer(), mConstraints.mOffset * sizeof(DxConstraint),
	                    constraints.size() * sizeof(DxConstraint));

	Vector<DxConstraint>::Type::Iterator cIt, cEnd = constraints.end();
	for (cIt = constraints.begin(); cIt != cEnd; ++cIt)
		cIt->mRestvalue *= scale;

	mConstraints = constraints;
}

void cloth::DxFabric::scaleTetherLengths(float scale)
{
	// cloth instances won't pick this up until DxClothData is dirty!
	mTetherLengthScale *= scale;
}

#endif // NV_CLOTH_ENABLE_DX11
