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

#include "SwFactory.h"
#include "SwFabric.h"
#include "SwCloth.h"

#include "ClothImpl.h"
#include "ClothBase.h"
#include "NvCloth/Allocator.h"

namespace nv
{
namespace cloth
{
class DxFactory;
class CuFactory;

// make range from vector
template <typename T, typename A>
Range<T> makeRange(physx::shdfnd::Array<T, A>& vec)
{
	T* ptr = vec.empty() ? 0 : vec.begin();
	return Range<T>(ptr, ptr + vec.size());
}

template <typename T, typename A>
Range<const T> makeRange(const physx::shdfnd::Array<T, A>& vec)
{
	const T* ptr = vec.empty() ? 0 : vec.begin();
	return Range<const T>(ptr, ptr + vec.size());
}

// fabric conversion
template <typename SrcClothType, typename DstFactoryType>
typename DstFactoryType::FabricType* convertFabric(const SrcClothType& srcFabric, DstFactoryType& dstFactory)
{
	typedef typename DstFactoryType::FabricType DstFabricType;

	// see if dstFactory already has a Fabric with this id
	DstFabricType* const* fIt = dstFactory.mFabrics.begin();
	DstFabricType* const* fEnd = dstFactory.mFabrics.end();
	for (; fIt != fEnd; ++fIt)
		if ((*fIt)->mId == srcFabric.mId)
		{
			(*fIt)->incRefCount();
			return *fIt; // found id, return existing fabric
		}

	// fabric does not exist so create a new one
	Vector<uint32_t>::Type phaseIndices(srcFabric.getNumPhases());
	Vector<uint32_t>::Type sets(srcFabric.getNumSets());
	Vector<float>::Type restvalues(srcFabric.getNumRestvalues());
	Vector<float>::Type stiffnessValues(srcFabric.getNumStiffnessValues());
	Vector<uint32_t>::Type indices(srcFabric.getNumIndices());
	Vector<uint32_t>::Type anchors(srcFabric.getNumTethers());
	Vector<float>::Type tetherLengths(srcFabric.getNumTethers());
	Vector<uint32_t>::Type triangles(srcFabric.getNumTriangles() * 3);

	Range<uint32_t> phaseIndexRange = makeRange(phaseIndices);
	Range<float> restvalueRange = makeRange(restvalues);
	Range<float> stiffnessValueRange = makeRange(stiffnessValues);
	Range<uint32_t> setRange = makeRange(sets);
	Range<uint32_t> indexRange = makeRange(indices);
	Range<uint32_t> anchorRange = makeRange(anchors);
	Range<float> lengthRange = makeRange(tetherLengths);
	Range<uint32_t> triangleRange = makeRange(triangles);

	srcFabric.mFactory.extractFabricData(srcFabric, phaseIndexRange, setRange, restvalueRange, stiffnessValueRange, indexRange, anchorRange,
	                                     lengthRange, triangleRange);

	DstFabricType* dstFabric =
	    static_cast<DstFabricType*>(dstFactory.createFabric(srcFabric.mNumParticles, phaseIndexRange, setRange, restvalueRange, stiffnessValueRange,
	                                                        indexRange, anchorRange, lengthRange, triangleRange));

	// give new fabric the same id as the source so it can be matched
	dstFabric->mId = srcFabric.mId;

	return dstFabric;
}

inline Range<const PhaseConfig> getPhaseConfigs(const SwCloth& cloth)
{
	return makeRange(cloth.mPhaseConfigs);
}
inline void setPhaseConfigs(SwCloth& cloth, Range<const PhaseConfig> phaseConfigs)
{
	cloth.mPhaseConfigs.assign(phaseConfigs.begin(), phaseConfigs.end());
}
inline Range<const physx::PxVec4> getParticleAccelerations(const SwCloth& cloth)
{
	return makeRange(cloth.mParticleAccelerations);
}
inline Range<const uint32_t> getSelfCollisionIndices(const SwCloth& cloth)
{
	return makeRange(cloth.mSelfCollisionIndices);
}

// cloth conversion
template <typename DstFactoryType, typename SrcClothType>
typename DstFactoryType::ClothType* convertCloth(DstFactoryType& dstFactory, const SrcClothType& srcCloth)
{
	typedef typename DstFactoryType::FabricType DstFabricType;
	typedef typename DstFactoryType::ClothType DstClothType;

	const Factory& srcFactory = srcCloth.mFactory;

	typename DstClothType::ContextLockType dstLock(dstFactory);
	typename SrcClothType::ContextLockType srcLock(srcCloth.mFactory);

	// particles
	MappedRange<const physx::PxVec4> curParticles = srcCloth.getCurrentParticles();

	// fabric
	DstFabricType& dstFabric = *convertFabric(srcCloth.mFabric, dstFactory);

	// create new cloth
	DstClothType* dstCloth = static_cast<DstClothType*>(dstFactory.createCloth(curParticles, dstFabric));
	dstFabric.decRefCount();

	// copy across common parameters
	copy(*dstCloth, srcCloth);

	// copy across previous particles
	MappedRange<const physx::PxVec4> prevParticles = srcCloth.getPreviousParticles();
	memcpy(dstCloth->getPreviousParticles().begin(), prevParticles.begin(), prevParticles.size() * sizeof(physx::PxVec4));

	// copy across transformed phase configs
	setPhaseConfigs(*dstCloth, getPhaseConfigs(srcCloth));

	// collision data
	Vector<physx::PxVec4>::Type spheres(srcCloth.getNumSpheres(), physx::PxVec4(0.0f));
	physx::PxVec4* spherePtr = spheres.empty() ? 0 : &spheres.front();
	Range<physx::PxVec4> sphereRange(spherePtr, spherePtr + spheres.size());
	Vector<uint32_t>::Type capsules(srcCloth.getNumCapsules() * 2);
	Range<uint32_t> capsuleRange = makeRange(capsules);
	Vector<physx::PxVec4>::Type planes(srcCloth.getNumPlanes(), physx::PxVec4(0.0f));
	physx::PxVec4* planePtr = planes.empty() ? 0 : &planes.front();
	Range<physx::PxVec4> planeRange(planePtr, planePtr + planes.size());
	Vector<uint32_t>::Type convexes(srcCloth.getNumConvexes());
	Range<uint32_t> convexRange = makeRange(convexes);
	Vector<physx::PxVec3>::Type triangles(srcCloth.getNumTriangles() * 3, physx::PxVec3(0.0f));
	physx::PxVec3* trianglePtr = triangles.empty() ? 0 : &triangles.front();
	Range<physx::PxVec3> triangleRange(trianglePtr, trianglePtr + triangles.size());

	srcFactory.extractCollisionData(srcCloth, sphereRange, capsuleRange, planeRange, convexRange, triangleRange);
	dstCloth->setSpheres(sphereRange, 0, 0);
	dstCloth->setCapsules(capsuleRange, 0, 0);
	dstCloth->setPlanes(planeRange, 0, 0);
	dstCloth->setConvexes(convexRange, 0, 0);
	dstCloth->setTriangles(triangleRange, 0, 0);

	// motion constraints, copy directly into new cloth buffer
	if (srcCloth.getNumMotionConstraints())
		srcFactory.extractMotionConstraints(srcCloth, dstCloth->getMotionConstraints());

	// separation constraints, copy directly into new cloth buffer
	if (srcCloth.getNumSeparationConstraints())
		srcFactory.extractSeparationConstraints(srcCloth, dstCloth->getSeparationConstraints());

	// particle accelerations
	if (srcCloth.getNumParticleAccelerations())
	{
		Range<const physx::PxVec4> accelerations = getParticleAccelerations(srcCloth);
		memcpy(dstCloth->getParticleAccelerations().begin(), accelerations.begin(),
		          accelerations.size() * sizeof(physx::PxVec4));
	}

	// self-collision indices
	dstCloth->setSelfCollisionIndices(getSelfCollisionIndices(srcCloth));

	// rest positions
	Vector<physx::PxVec4>::Type restPositions(srcCloth.getNumRestPositions());
	srcFactory.extractRestPositions(srcCloth, makeRange(restPositions));
	dstCloth->setRestPositions(makeRange(restPositions));

	// virtual particles
	if (srcCloth.getNumVirtualParticles())
	{
		Vector<Vec4u>::Type indices(srcCloth.getNumVirtualParticles());
		Vector<physx::PxVec3>::Type weights(srcCloth.getNumVirtualParticleWeights(), physx::PxVec3(0.0f));

		uint32_t(*indicesPtr)[4] = indices.empty() ? 0 : &array(indices.front());
		Range<uint32_t[4]> indicesRange(indicesPtr, indicesPtr + indices.size());

		physx::PxVec3* weightsPtr = weights.empty() ? 0 : &weights.front();
		Range<physx::PxVec3> weightsRange(weightsPtr, weightsPtr + weights.size());

		srcFactory.extractVirtualParticles(srcCloth, indicesRange, weightsRange);

		dstCloth->setVirtualParticles(indicesRange, weightsRange);
	}

	return dstCloth;
}

} // namespace cloth
} // namespace nv
