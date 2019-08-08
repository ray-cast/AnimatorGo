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

#include "foundation/PxAssert.h"
#include "SwFabric.h"
#include "SwFactory.h"
#include "PsSort.h"
#include "limits.h" // for USHRT_MAX
#include <algorithm>
#include "PsUtilities.h"

using namespace nv;
using namespace physx;

cloth::SwTether::SwTether(uint16_t anchor, float length) : mAnchor(anchor), mLength(length)
{
}

cloth::SwFabric::SwFabric(SwFactory& factory, uint32_t numParticles, Range<const uint32_t> phaseIndices,
                          Range<const uint32_t> sets, Range<const float> restvalues, Range<const float> stiffnessValues,
						  Range<const uint32_t> indices, Range<const uint32_t> anchors, Range<const float> tetherLengths,
                          Range<const uint32_t> triangles, uint32_t id)
: mFactory(factory)
, mNumParticles(numParticles)
, mTetherLengthScale(1.0f), mId(id)
{
	// should no longer be prefixed with 0
	NV_CLOTH_ASSERT(sets.front() != 0);

#if PX_WINDOWS_FAMILY
	const uint32_t kSimdWidth = 8; // avx
#else
	const uint32_t kSimdWidth = 4;
#endif

	// consistency check
	NV_CLOTH_ASSERT(sets.back() == restvalues.size());
	NV_CLOTH_ASSERT(restvalues.size() * 2 == indices.size());
	NV_CLOTH_ASSERT(restvalues.size() == stiffnessValues.size() || stiffnessValues.size() == 0);
	NV_CLOTH_ASSERT(mNumParticles > *shdfnd::maxElement(indices.begin(), indices.end()));
	NV_CLOTH_ASSERT(mNumParticles + kSimdWidth - 1 <= USHRT_MAX);

	mPhases.assign(phaseIndices.begin(), phaseIndices.end());
	mSets.reserve(sets.size() + 1);
	mSets.pushBack(0); // prefix with 0

	mOriginalNumRestvalues = uint32_t(restvalues.size());

	// padd indices for SIMD
	const uint32_t* iBegin = indices.begin(), *iIt = iBegin;
	const float* rBegin = restvalues.begin(), *rIt = rBegin;
	const float* stBegin = stiffnessValues.begin(), *stIt = stBegin;
	const uint32_t* sIt, *sEnd = sets.end();
	for (sIt = sets.begin(); sIt != sEnd; ++sIt)
	{
		const float* rEnd = rBegin + *sIt;
		const float* stEnd = stBegin + *sIt;
		const uint32_t* iEnd = iBegin + *sIt * 2;
		uint32_t numConstraints = uint32_t(rEnd - rIt);

		for (; rIt != rEnd; ++rIt)
		{
			mRestvalues.pushBack(*rIt);
		}
		if (!stiffnessValues.empty())
		{
			for (; stIt != stEnd; ++stIt)
			{
				mStiffnessValues.pushBack(*stIt);
			}
		}
		for (; iIt != iEnd; ++iIt)
			mIndices.pushBack(uint16_t(*iIt));

		// add dummy indices to make multiple of 4
		for (; numConstraints &= kSimdWidth - 1; ++numConstraints)
		{
			mRestvalues.pushBack(-FLT_MAX);
			if (!stiffnessValues.empty())
				mStiffnessValues.pushBack(-FLT_MAX);
			uint32_t index = mNumParticles + numConstraints - 1;
			mIndices.pushBack(uint16_t(index));
			mIndices.pushBack(uint16_t(index));
		}

		mSets.pushBack(uint32_t(mRestvalues.size()));
	}

	// trim overallocations
	RestvalueContainer(mRestvalues.begin(), mRestvalues.end()).swap(mRestvalues);
	RestvalueContainer(mStiffnessValues.begin(), mStiffnessValues.end()).swap(mStiffnessValues);
	Vector<uint16_t>::Type(mIndices.begin(), mIndices.end()).swap(mIndices);

	// tethers
	NV_CLOTH_ASSERT(anchors.size() == tetherLengths.size());

	// pad to allow for direct 16 byte (unaligned) loads
	mTethers.reserve(anchors.size() + 2);
	for (; !anchors.empty(); anchors.popFront(), tetherLengths.popFront())
		mTethers.pushBack(SwTether(uint16_t(anchors.front()), tetherLengths.front()));

	// triangles
	mTriangles.reserve(triangles.size());
	const uint32_t* iEnd = triangles.end();
	for (iIt = triangles.begin(); iIt != iEnd; ++iIt)
		mTriangles.pushBack(uint16_t(*iIt));

	mFactory.mFabrics.pushBack(this);
}

cloth::SwFabric::~SwFabric()
{
	Vector<SwFabric*>::Type::Iterator fIt = mFactory.mFabrics.find(this);
	NV_CLOTH_ASSERT(fIt != mFactory.mFabrics.end());
	mFactory.mFabrics.replaceWithLast(fIt);
}

cloth::Factory& cloth::SwFabric::getFactory() const
{
	return mFactory;
}

uint32_t cloth::SwFabric::getNumPhases() const
{
	return uint32_t(mPhases.size());
}

uint32_t cloth::SwFabric::getNumRestvalues() const
{
	return mOriginalNumRestvalues;
}

uint32_t cloth::SwFabric::getNumStiffnessValues() const
{
	return mStiffnessValues.size()?mOriginalNumRestvalues:0;
}

uint32_t cloth::SwFabric::getNumSets() const
{
	return uint32_t(mSets.size() - 1);
}

uint32_t cloth::SwFabric::getNumIndices() const
{
	return 2 * mOriginalNumRestvalues;
}

uint32_t cloth::SwFabric::getNumParticles() const
{
	return mNumParticles;
}

uint32_t cloth::SwFabric::getNumTethers() const
{
	return uint32_t(mTethers.size());
}

uint32_t cloth::SwFabric::getNumTriangles() const
{
	return uint32_t(mTriangles.size()) / 3;
}

void cloth::SwFabric::scaleRestvalues(float scale)
{
	RestvalueContainer::Iterator rIt, rEnd = mRestvalues.end();
	for (rIt = mRestvalues.begin(); rIt != rEnd; ++rIt)
		*rIt *= scale;
}

void cloth::SwFabric::scaleTetherLengths(float scale)
{
	mTetherLengthScale *= scale;
}
