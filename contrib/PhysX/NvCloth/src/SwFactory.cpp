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

#include "SwFactory.h"
#include "SwFabric.h"
#include "SwCloth.h"
#include "SwSolver.h"
#include <string.h> // for memcpy

using namespace nv;
using namespace physx;

namespace nv
{
namespace cloth
{
// defined in Factory.cpp
uint32_t getNextFabricId();
}
}

cloth::SwFactory::SwFactory()
{
}

cloth::SwFactory::~SwFactory()
{
	NV_CLOTH_ASSERT_WITH_MESSAGE("All fabrics created by this factory need to be deleted before this factory is destroyed.", mFabrics.size() == 0);
}

cloth::Fabric* cloth::SwFactory::createFabric(uint32_t numParticles, Range<const uint32_t> phaseIndices,
											  Range<const uint32_t> sets, Range<const float> restvalues, Range<const float> stiffnessValues,
											  Range<const uint32_t> indices, Range<const uint32_t> anchors,
                                              Range<const float> tetherLengths, Range<const uint32_t> triangles)
{
	return NV_CLOTH_NEW(SwFabric)(*this, numParticles, phaseIndices, sets, restvalues, stiffnessValues, indices, anchors, tetherLengths, triangles,
								  getNextFabricId());
}

cloth::Cloth* cloth::SwFactory::createCloth(Range<const PxVec4> particles, Fabric& fabric)
{
	return NV_CLOTH_NEW(SwCloth)(*this, static_cast<SwFabric&>(fabric), particles);
}

cloth::Solver* cloth::SwFactory::createSolver()
{
	return NV_CLOTH_NEW(SwSolver)();
}

cloth::Cloth* cloth::SwFactory::clone(const Cloth& cloth)
{
	if (cloth.getFactory().getPlatform() != Platform::CPU)
		return cloth.clone(*this); // forward to CuCloth

	// copy construct
	return NV_CLOTH_NEW(SwCloth)(*this, static_cast<const SwCloth&>(cloth));
}

void cloth::SwFactory::extractFabricData(const Fabric& fabric, Range<uint32_t> phaseIndices, Range<uint32_t> sets,
                                         Range<float> restvalues, Range<float> stiffnessValues, Range<uint32_t> indices, Range<uint32_t> anchors,
                                         Range<float> tetherLengths, Range<uint32_t> triangles) const
{
	const SwFabric& swFabric = static_cast<const SwFabric&>(fabric);

	NV_CLOTH_ASSERT(phaseIndices.empty() || phaseIndices.size() == swFabric.getNumPhases());
	NV_CLOTH_ASSERT(restvalues.empty() || restvalues.size() == swFabric.getNumRestvalues());
	NV_CLOTH_ASSERT(stiffnessValues.empty() || stiffnessValues.size() == swFabric.getNumStiffnessValues());
	NV_CLOTH_ASSERT(sets.empty() || sets.size() == swFabric.getNumSets());
	NV_CLOTH_ASSERT(indices.empty() || indices.size() == swFabric.getNumIndices());
	NV_CLOTH_ASSERT(anchors.empty() || anchors.size() == swFabric.getNumTethers());
	NV_CLOTH_ASSERT(tetherLengths.empty() || tetherLengths.size() == swFabric.getNumTethers());

	for (uint32_t i = 0; !phaseIndices.empty(); ++i, phaseIndices.popFront())
		phaseIndices.front() = swFabric.mPhases[i];

	typedef SwFabric::RestvalueContainer::ConstIterator RestvalueIterator;
	
	Vector<uint32_t>::Type::ConstIterator sEnd = swFabric.mSets.end(), sIt;
	RestvalueIterator rBegin = swFabric.mRestvalues.begin(), rIt = rBegin;
	RestvalueIterator stIt = swFabric.mStiffnessValues.begin();
	Vector<uint16_t>::Type::ConstIterator iIt = swFabric.mIndices.begin();

	uint32_t* sDst = sets.begin();
	float* rDst = restvalues.begin();
	float* stDst = stiffnessValues.begin();
	uint32_t* iDst = indices.begin();

	uint32_t numConstraints = 0;
	for (sIt = swFabric.mSets.begin(); ++sIt != sEnd;)
	{
		RestvalueIterator rEnd = rBegin + *sIt;
		for (; rIt != rEnd; ++rIt, ++stIt)
		{
			uint16_t i0 = *iIt++;
			uint16_t i1 = *iIt++;

			if (std::max(i0, i1) >= swFabric.mNumParticles)
				continue;

			if (!restvalues.empty())
				*rDst++ = *rIt;
			if (!stiffnessValues.empty())
				*stDst++ = *stIt;

			if (!indices.empty())
			{
				*iDst++ = i0;
				*iDst++ = i1;
			}

			++numConstraints;
		}

		if (!sets.empty())
			*sDst++ = numConstraints;
	}

	for (uint32_t i = 0; !anchors.empty(); ++i, anchors.popFront())
		anchors.front() = swFabric.mTethers[i].mAnchor;

	for (uint32_t i = 0; !tetherLengths.empty(); ++i, tetherLengths.popFront())
		tetherLengths.front() = swFabric.mTethers[i].mLength * swFabric.mTetherLengthScale;

	for (uint32_t i = 0; !triangles.empty(); ++i, triangles.popFront())
		triangles.front() = swFabric.mTriangles[i];
}

void cloth::SwFactory::extractCollisionData(const Cloth& cloth, Range<PxVec4> spheres, Range<uint32_t> capsules,
                                            Range<PxVec4> planes, Range<uint32_t> convexes, Range<PxVec3> triangles) const
{
	NV_CLOTH_ASSERT(&cloth.getFactory() == this);

	const SwCloth& swCloth = static_cast<const SwCloth&>(cloth);

	NV_CLOTH_ASSERT(spheres.empty() || spheres.size() == swCloth.mStartCollisionSpheres.size());
	NV_CLOTH_ASSERT(capsules.empty() || capsules.size() == swCloth.mCapsuleIndices.size() * 2);
	NV_CLOTH_ASSERT(planes.empty() || planes.size() == swCloth.mStartCollisionPlanes.size());
	NV_CLOTH_ASSERT(convexes.empty() || convexes.size() == swCloth.mConvexMasks.size());
	NV_CLOTH_ASSERT(triangles.empty() || triangles.size() == swCloth.mStartCollisionTriangles.size());

	if (!swCloth.mStartCollisionSpheres.empty() && !spheres.empty())
		memcpy(spheres.begin(), &swCloth.mStartCollisionSpheres.front(),
		       swCloth.mStartCollisionSpheres.size() * sizeof(PxVec4));

	if (!swCloth.mCapsuleIndices.empty() && !capsules.empty())
		memcpy(capsules.begin(), &swCloth.mCapsuleIndices.front(), swCloth.mCapsuleIndices.size() * sizeof(IndexPair));

	if (!swCloth.mStartCollisionPlanes.empty() && !planes.empty())
		memcpy(planes.begin(), &swCloth.mStartCollisionPlanes.front(),
		       swCloth.mStartCollisionPlanes.size() * sizeof(PxVec4));

	if (!swCloth.mConvexMasks.empty() && !convexes.empty())
		memcpy(convexes.begin(), &swCloth.mConvexMasks.front(), swCloth.mConvexMasks.size() * sizeof(uint32_t));

	if (!swCloth.mStartCollisionTriangles.empty() && !triangles.empty())
		memcpy(triangles.begin(), &swCloth.mStartCollisionTriangles.front(),
		       swCloth.mStartCollisionTriangles.size() * sizeof(PxVec3));
}

void cloth::SwFactory::extractMotionConstraints(const Cloth& cloth, Range<PxVec4> destConstraints) const
{
	NV_CLOTH_ASSERT(&cloth.getFactory() == this);

	const SwCloth& swCloth = static_cast<const SwCloth&>(cloth);

	Vector<PxVec4>::Type const& srcConstraints = !swCloth.mMotionConstraints.mTarget.empty()
	                                               ? swCloth.mMotionConstraints.mTarget
	                                               : swCloth.mMotionConstraints.mStart;

	if (!srcConstraints.empty())
	{
		// make sure dest array is big enough
		NV_CLOTH_ASSERT(destConstraints.size() == srcConstraints.size());

		memcpy(destConstraints.begin(), &srcConstraints.front(), srcConstraints.size() * sizeof(PxVec4));
	}
}

void cloth::SwFactory::extractSeparationConstraints(const Cloth& cloth, Range<PxVec4> destConstraints) const
{
	NV_CLOTH_ASSERT(&cloth.getFactory() == this);

	const SwCloth& swCloth = static_cast<const SwCloth&>(cloth);

	Vector<PxVec4>::Type const& srcConstraints = !swCloth.mSeparationConstraints.mTarget.empty()
	                                               ? swCloth.mSeparationConstraints.mTarget
	                                               : swCloth.mSeparationConstraints.mStart;

	if (!srcConstraints.empty())
	{
		// make sure dest array is big enough
		NV_CLOTH_ASSERT(destConstraints.size() == srcConstraints.size());

		memcpy(destConstraints.begin(), &srcConstraints.front(), srcConstraints.size() * sizeof(PxVec4));
	}
}

void cloth::SwFactory::extractParticleAccelerations(const Cloth& cloth, Range<PxVec4> destAccelerations) const
{
	NV_CLOTH_ASSERT(&cloth.getFactory() == this);

	const SwCloth& swCloth = static_cast<const SwCloth&>(cloth);

	if (!swCloth.mParticleAccelerations.empty())
	{
		// make sure dest array is big enough
		NV_CLOTH_ASSERT(destAccelerations.size() == swCloth.mParticleAccelerations.size());

		memcpy(destAccelerations.begin(), &swCloth.mParticleAccelerations.front(),
		       swCloth.mParticleAccelerations.size() * sizeof(PxVec4));
	}
}

void cloth::SwFactory::extractVirtualParticles(const Cloth& cloth, Range<uint32_t[4]> indices, Range<PxVec3> weights) const
{
	NV_CLOTH_ASSERT(this == &cloth.getFactory());

	const SwCloth& swCloth = static_cast<const SwCloth&>(cloth);

	uint32_t numIndices = cloth.getNumVirtualParticles();
	uint32_t numWeights = cloth.getNumVirtualParticleWeights();

	NV_CLOTH_ASSERT(indices.size() == numIndices || indices.empty());
	NV_CLOTH_ASSERT(weights.size() == numWeights || weights.empty());

	if (weights.size() == numWeights)
	{
		PxVec3* wDestIt = reinterpret_cast<PxVec3*>(weights.begin());

		// convert weights from vec4 to vec3
		Vector<PxVec4>::Type::ConstIterator wIt = swCloth.mVirtualParticleWeights.begin();
		Vector<PxVec4>::Type::ConstIterator wEnd = wIt + numWeights;

		for (; wIt != wEnd; ++wIt, ++wDestIt)
			*wDestIt = PxVec3( wIt->x, wIt->y, wIt->z );

		NV_CLOTH_ASSERT(wDestIt == weights.end());
	}
	if (indices.size() == numIndices)
	{
		// convert indices
		Vec4u* iDestIt = reinterpret_cast<Vec4u*>(indices.begin());
		Vector<Vec4us>::Type::ConstIterator iIt = swCloth.mVirtualParticleIndices.begin();
		Vector<Vec4us>::Type::ConstIterator iEnd = swCloth.mVirtualParticleIndices.end();

		uint32_t numParticles = uint32_t(swCloth.mCurParticles.size());

		for (; iIt != iEnd; ++iIt)
		{
			// skip dummy indices
			if (iIt->x < numParticles)
				// byte offset to element index
				*iDestIt++ = Vec4u(*iIt);
		}

		NV_CLOTH_ASSERT(&array(*iDestIt) == indices.end());
	}
}

void cloth::SwFactory::extractSelfCollisionIndices(const Cloth& cloth, Range<uint32_t> destIndices) const
{
	const SwCloth& swCloth = static_cast<const SwCloth&>(cloth);
	NV_CLOTH_ASSERT(destIndices.size() == swCloth.mSelfCollisionIndices.size());
	memcpy(destIndices.begin(), swCloth.mSelfCollisionIndices.begin(), destIndices.size() * sizeof(uint32_t));
}

void cloth::SwFactory::extractRestPositions(const Cloth& cloth, Range<PxVec4> destRestPositions) const
{
	const SwCloth& swCloth = static_cast<const SwCloth&>(cloth);
	NV_CLOTH_ASSERT(destRestPositions.size() == swCloth.mRestPositions.size());
	memcpy(destRestPositions.begin(), swCloth.mRestPositions.begin(), destRestPositions.size() * sizeof(PxVec4));
}
