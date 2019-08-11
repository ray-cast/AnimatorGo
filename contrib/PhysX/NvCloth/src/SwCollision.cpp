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

#include "SwCollision.h"
#include "SwCloth.h"
#include "SwClothData.h"
#include "IterationState.h"
#include "BoundingBox.h"
#include "PointInterpolator.h"
#include "SwCollisionHelpers.h"
#include <foundation/PxProfiler.h>
#include <cstring> // for memset
#include "ps/PsSort.h"

using namespace nv;
using namespace physx;
using namespace cloth;

// the particle trajectory needs to penetrate more than 0.2 * radius to trigger continuous collision
template <typename T4f>
const T4f cloth::SwCollision<T4f>::sSkeletonWidth = simd4f(cloth::sqr(1 - 0.2f) - 1);

#if NV_SIMD_SSE2
const Simd4i cloth::Gather<Simd4i>::sIntSignBit = simd4i(0x80000000);
const Simd4i cloth::Gather<Simd4i>::sSignedMask = sIntSignBit | simd4i(0x7);
#elif NV_SIMD_NEON
const Simd4i cloth::Gather<Simd4i>::sPack = simd4i(0x00000000, 0x04040404, 0x08080808, 0x0c0c0c0c);
const Simd4i cloth::Gather<Simd4i>::sOffset = simd4i(0x03020100);
const Simd4i cloth::Gather<Simd4i>::sShift = simd4i(2);
const Simd4i cloth::Gather<Simd4i>::sMask = simd4i(7);
#endif

namespace
{
const Simd4fTupleFactory sMaskX = simd4f(simd4i(~0, 0, 0, 0));
const Simd4fTupleFactory sMaskZ = simd4f(simd4i(0, 0, ~0, 0));
const Simd4fTupleFactory sMaskW = simd4f(simd4i(0, 0, 0, ~0));
const Simd4fTupleFactory gSimd4fOneXYZ = simd4f(1.0f, 1.0f, 1.0f, 0.0f);
const Simd4fScalarFactory sGridLength = simd4f(8 - 1e-3f); // sGridSize
const Simd4fScalarFactory sGridExpand = simd4f(1e-4f);
const Simd4fTupleFactory sMinusFloatMaxXYZ = simd4f(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0.0f);

#if PX_PROFILE || PX_DEBUG
template <typename T4f>
uint32_t horizontalSum(const T4f& x)
{
	const float* p = array(x);
	return uint32_t(0.5f + p[0] + p[1] + p[2] + p[3]);
}
#endif

// 7 elements are written to ptr!
template <typename T4f>
void storeBounds(float* ptr, const cloth::BoundingBox<T4f>& bounds)
{
	store(ptr, bounds.mLower);
	store(ptr + 3, bounds.mUpper);
}
}

struct cloth::SphereData
{
	PxVec3 center;
	float radius;
};

struct cloth::ConeData
{
	PxVec3 center;
	float radius; // cone radius at center
	PxVec3 axis;
	float slope; // tan(alpha)

	float sqrCosine; // cos^2(alpha)
	float halfLength;

	uint32_t firstMask;
	uint32_t bothMask;
};

struct cloth::TriangleData
{
	PxVec3 base;
	float edge0DotEdge1;

	PxVec3 edge0;
	float edge0SqrLength;

	PxVec3 edge1;
	float edge1SqrLength;

	PxVec3 normal;
	float padding;

	float det;
	float denom;

	float edge0InvSqrLength;
	float edge1InvSqrLength;
};

namespace nv
{
namespace cloth
{
template <typename T4f>
BoundingBox<T4f> expandBounds(const BoundingBox<T4f>& bbox, const SphereData* sIt, const SphereData* sEnd)
{
	BoundingBox<T4f> result = bbox;
	for (; sIt != sEnd; ++sIt)
	{
		T4f p = loadAligned(array(sIt->center));
		T4f r = splat<3>(p);
		result.mLower = min(result.mLower, p - r);
		result.mUpper = max(result.mUpper, p + r);
	}
	return result;
}
}
}

namespace
{
template <typename T4f, typename SrcIterator>
void generateSpheres(T4f* dIt, const SrcIterator& src, uint32_t count)
{
	// have to copy out iterator to ensure alignment is maintained
	for (SrcIterator sIt = src; 0 < count--; ++sIt, ++dIt)
		*dIt = max(sMinusFloatMaxXYZ, *sIt); // clamp radius to 0
}

void generateCones(cloth::ConeData* dst, const cloth::SphereData* sourceSpheres, const cloth::IndexPair* capsuleIndices,
                   uint32_t numCones)
{
	cloth::ConeData* cIt = dst;
	for (const cloth::IndexPair* iIt = capsuleIndices, *iEnd = iIt + numCones; iIt != iEnd; ++iIt, ++cIt)
	{
		// w element contains sphere radii
		PxVec4 first = reinterpret_cast<const PxVec4&>(sourceSpheres[iIt->first]);
		PxVec4 second = reinterpret_cast<const PxVec4&>(sourceSpheres[iIt->second]);

		PxVec4 center = (second + first) * 0.5f;
		PxVec4 axis = (second - first) * 0.5f; //half axis
		//axis.w = half of radii difference

		// |Axis|^2
		float sqrAxisHalfLength = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;

		// http://jwilson.coe.uga.edu/emt669/Student.Folders/Kertscher.Jeff/Essay.3/Tangents.html
		// |Axis|^2 = |Cone|^2 + (sphere2Radius-sphere1Radius)^2
		float sqrConeHalfLength = sqrAxisHalfLength - cloth::sqr(axis.w);

		float invAxisHalfLength = 1 / sqrtf(sqrAxisHalfLength);
		float invConeHalfLength = 1 / sqrtf(sqrConeHalfLength);

		if (sqrConeHalfLength <= 0.0f)
			invAxisHalfLength = invConeHalfLength = 0.0f;

		float axisHalfLength = sqrAxisHalfLength * invAxisHalfLength;
		float slope = axis.w * invConeHalfLength;

		cIt->center = PxVec3(center.x, center.y, center.z );
		cIt->radius = (axis.w + first.w) * invConeHalfLength * axisHalfLength; //cone radius in the center
		cIt->axis = PxVec3(axis.x, axis.y, axis.z) * invAxisHalfLength;
		cIt->slope = slope;

		// cos()^2 = 1.0 - (radius difference / axis length)^2
		// cos()^2 = 1.0 - (opposite/hypotenuse)^2
		// cos()^2 = 1.0 - sin(angle between c2c1 and  c2t1)^2
		// cos()^2 = 1.0 - sin(angle between axis and  c2t1)^2
		cIt->sqrCosine = 1.0f - cloth::sqr(axis.w * invAxisHalfLength);
		cIt->halfLength = axisHalfLength;

		uint32_t firstMask = 0x1u << iIt->first;
		cIt->firstMask = firstMask;
		cIt->bothMask = firstMask | 0x1u << iIt->second;
	}
}

template <typename T4f, typename SrcIterator>
void generatePlanes(T4f* dIt, const SrcIterator& src, uint32_t count)
{
	// have to copy out iterator to ensure alignment is maintained
	for (SrcIterator sIt = src; 0 < count--; ++sIt, ++dIt)
		*dIt = *sIt;
}

template <typename T4f, typename SrcIterator>
void generateTriangles(cloth::TriangleData* dIt, const SrcIterator& src, uint32_t count)
{
	// have to copy out iterator to ensure alignment is maintained
	for (SrcIterator sIt = src; 0 < count--; ++dIt)
	{
		T4f p0 = *sIt;
		++sIt;
		T4f p1 = *sIt;
		++sIt;
		T4f p2 = *sIt;
		++sIt;

		T4f edge0 = p1 - p0;
		T4f edge1 = p2 - p0;
		T4f normal = cross3(edge0, edge1);

		T4f edge0SqrLength = dot3(edge0, edge0);
		T4f edge1SqrLength = dot3(edge1, edge1);
		T4f edge0DotEdge1 = dot3(edge0, edge1);
		T4f normalInvLength = rsqrt(dot3(normal, normal));

		T4f det = edge0SqrLength * edge1SqrLength - edge0DotEdge1 * edge0DotEdge1;
		T4f denom = edge0SqrLength + edge1SqrLength - edge0DotEdge1 - edge0DotEdge1;

		// there are definitely faster ways...
		T4f aux = select(sMaskX, det, denom);
		aux = select(sMaskZ, edge0SqrLength, aux);
		aux = select(sMaskW, edge1SqrLength, aux);

		storeAligned(&dIt->base.x, select(sMaskW, edge0DotEdge1, p0));
		storeAligned(&dIt->edge0.x, select(sMaskW, edge0SqrLength, edge0));
		storeAligned(&dIt->edge1.x, select(sMaskW, edge1SqrLength, edge1));
		storeAligned(&dIt->normal.x, normal * normalInvLength);
		storeAligned(&dIt->det, recip<1>(aux));
	}
}

} // namespace

template <typename T4f>
cloth::SwCollision<T4f>::CollisionData::CollisionData()
: mSpheres(0), mCones(0)
{
}

template <typename T4f>
cloth::SwCollision<T4f>::SwCollision(SwClothData& clothData, SwKernelAllocator& alloc)
: mClothData(clothData), mAllocator(alloc)
{
	allocate(mCurData);

	if (mClothData.mEnableContinuousCollision || mClothData.mFrictionScale > 0.0f)
	{
		allocate(mPrevData);

		generateSpheres(reinterpret_cast<T4f*>(mPrevData.mSpheres),
		                reinterpret_cast<const T4f*>(clothData.mStartCollisionSpheres), clothData.mNumSpheres);

		generateCones(mPrevData.mCones, mPrevData.mSpheres, clothData.mCapsuleIndices, clothData.mNumCapsules);
	}
}

template <typename T4f>
cloth::SwCollision<T4f>::~SwCollision()
{
	deallocate(mCurData);
	deallocate(mPrevData);
}

template <typename T4f>
void cloth::SwCollision<T4f>::operator()(const IterationState<T4f>& state)
{
	mNumCollisions = 0;

	collideConvexes(state);  // discrete convex collision, no friction
	collideTriangles(state); // discrete triangle collision, no friction

	computeBounds();

	if (!mClothData.mNumSpheres)
		return;

	bool lastIteration = state.mRemainingIterations == 1;

	const T4f* targetSpheres = reinterpret_cast<const T4f*>(mClothData.mTargetCollisionSpheres);

	// generate sphere and cone collision data
	if (!lastIteration)
	{
		// interpolate spheres
		LerpIterator<T4f, const T4f*> pIter(reinterpret_cast<const T4f*>(mClothData.mStartCollisionSpheres),
		                                          targetSpheres, state.getCurrentAlpha());
		generateSpheres(reinterpret_cast<T4f*>(mCurData.mSpheres), pIter, mClothData.mNumSpheres);
	}
	else
	{
		// otherwise use the target spheres directly
		generateSpheres(reinterpret_cast<T4f*>(mCurData.mSpheres), targetSpheres, mClothData.mNumSpheres);
	}

	// generate cones even if test below fails because
	// continuous collision might need it in next iteration
	generateCones(mCurData.mCones, mCurData.mSpheres, mClothData.mCapsuleIndices, mClothData.mNumCapsules);

	if (buildAcceleration())
	{
		if (mClothData.mEnableContinuousCollision)
			collideContinuousParticles();

		mergeAcceleration(reinterpret_cast<uint32_t*>(mSphereGrid));
		mergeAcceleration(reinterpret_cast<uint32_t*>(mConeGrid));

		if (!mClothData.mEnableContinuousCollision)
			collideParticles();

		collideVirtualParticles();
	}

	if (mPrevData.mSpheres)
		shdfnd::swap(mCurData, mPrevData);
}

template <typename T4f>
size_t cloth::SwCollision<T4f>::estimateTemporaryMemory(const SwCloth& cloth)
{
	size_t numTriangles = cloth.mStartCollisionTriangles.size();
	size_t numPlanes = cloth.mStartCollisionPlanes.size();

	const size_t kTriangleDataSize = sizeof(TriangleData) * numTriangles;
	const size_t kPlaneDataSize = sizeof(PxVec4) * numPlanes * 2;

	return std::max(kTriangleDataSize, kPlaneDataSize);
}

template <typename T4f>
size_t cloth::SwCollision<T4f>::estimatePersistentMemory(const SwCloth& cloth)
{
	size_t numCapsules = cloth.mCapsuleIndices.size();
	size_t numSpheres = cloth.mStartCollisionSpheres.size();

	size_t sphereDataSize = sizeof(SphereData) * numSpheres * 2;
	size_t coneDataSize = sizeof(ConeData) * numCapsules * 2;

	return sphereDataSize + coneDataSize;
}

template <typename T4f>
void cloth::SwCollision<T4f>::allocate(CollisionData& data)
{
	data.mSpheres = static_cast<SphereData*>(mAllocator.allocate(sizeof(SphereData) * mClothData.mNumSpheres));

	data.mCones = static_cast<ConeData*>(mAllocator.allocate(sizeof(ConeData) * mClothData.mNumCapsules));
}

template <typename T4f>
void cloth::SwCollision<T4f>::deallocate(const CollisionData& data)
{
	mAllocator.deallocate(data.mSpheres);
	mAllocator.deallocate(data.mCones);
}

template <typename T4f>
void cloth::SwCollision<T4f>::computeBounds()
{
	NV_CLOTH_PROFILE_ZONE("cloth::SwSolverKernel::computeBounds", /*ProfileContext::None*/ 0);

	T4f* prevIt = reinterpret_cast<T4f*>(mClothData.mPrevParticles);
	T4f* curIt = reinterpret_cast<T4f*>(mClothData.mCurParticles);
	T4f* curEnd = curIt + mClothData.mNumParticles;
	T4f floatMaxXYZ = -static_cast<T4f>(sMinusFloatMaxXYZ);

	T4f lower = simd4f(FLT_MAX), upper = -lower;
	for (; curIt < curEnd; ++curIt, ++prevIt)
	{
		T4f current = *curIt;
		lower = min(lower, current);
		upper = max(upper, current);
		// if (current.w > 0) current.w = previous.w
		*curIt = select(current > floatMaxXYZ, *prevIt, current);
	}

	BoundingBox<T4f> curBounds;
	curBounds.mLower = lower;
	curBounds.mUpper = upper;

	// don't change this order, storeBounds writes 7 floats
	BoundingBox<T4f> prevBounds = loadBounds<T4f>(mClothData.mCurBounds);
	storeBounds(mClothData.mCurBounds, curBounds);
	storeBounds(mClothData.mPrevBounds, prevBounds);
}

namespace
{
template <typename T4i>
T4i andNotIsZero(const T4i& left, const T4i& right)
{
	return (left & ~right) == gSimd4iZero;
}
}

// build per-axis mask arrays of spheres on the right/left of grid cell
template <typename T4f>
void cloth::SwCollision<T4f>::buildSphereAcceleration(const SphereData* sIt)
{
	static const int maxIndex = sGridSize - 1;

	uint32_t mask = 0x1; //single bit mask for current sphere
	const SphereData* sEnd = sIt + mClothData.mNumSpheres;
	for (; sIt != sEnd; ++sIt, mask <<= 1)
	{
		T4f sphere = loadAligned(array(sIt->center));
		T4f radius = splat<3>(sphere);

		//calculate the first and last cell index, for each axis, that contains the sphere
		T4i first = intFloor(min(max((sphere - radius) * mGridScale + mGridBias, gSimd4fZero), sGridLength)); //use both min and max to deal with bad grid scales
		T4i last = intFloor(min(max((sphere + radius) * mGridScale + mGridBias, gSimd4fZero), sGridLength));

		const int* firstIdx = array(first);
		const int* lastIdx = array(last);

		uint32_t* firstIt = reinterpret_cast<uint32_t*>(mSphereGrid);
		uint32_t* lastIt = firstIt + 3 * sGridSize;

		//loop through the 3 axes 
		for (uint32_t i = 0; i < 3; ++i, firstIt += sGridSize, lastIt += sGridSize)
		{
			//mark the sphere and everything to the right
			for (int j = firstIdx[i]; j <= maxIndex; ++j)
				firstIt[j] |= mask;

			//mark the sphere and everything to the left
			for (int j = lastIdx[i]; j >= 0; --j)
				lastIt[j] |= mask;
		}
	}
}

// generate cone masks from sphere masks
template <typename T4f>
void cloth::SwCollision<T4f>::buildConeAcceleration()
{
	const ConeData* coneIt = mCurData.mCones;
	const ConeData* coneEnd = coneIt + mClothData.mNumCapsules;
	for (uint32_t coneMask = 0x1; coneIt != coneEnd; ++coneIt, coneMask <<= 1)
	{
		if (coneIt->radius == 0.0f)
			continue;

		uint32_t spheresMask = coneIt->bothMask;

		uint32_t* sphereIt = reinterpret_cast<uint32_t*>(mSphereGrid);
		uint32_t* sphereEnd = sphereIt + 6 * sGridSize;
		uint32_t* gridIt = reinterpret_cast<uint32_t*>(mConeGrid);
		for (; sphereIt != sphereEnd; ++sphereIt, ++gridIt)
			if (*sphereIt & spheresMask)
				*gridIt |= coneMask;
	}
}

// convert right/left mask arrays into single overlap array
template <typename T4f>
void cloth::SwCollision<T4f>::mergeAcceleration(uint32_t* firstIt)
{
	uint32_t* firstEnd = firstIt + 3 * sGridSize;
	uint32_t* lastIt = firstEnd;
	for (; firstIt != firstEnd; ++firstIt, ++lastIt)
		*firstIt &= *lastIt;
}

// build mask of spheres/cones touching a regular grid along each axis
template <typename T4f>
bool cloth::SwCollision<T4f>::buildAcceleration()
{
	// determine single bounding box around all spheres
	BoundingBox<T4f> sphereBounds =
	    expandBounds(emptyBounds<T4f>(), mCurData.mSpheres, mCurData.mSpheres + mClothData.mNumSpheres);

	// determine single bounding box around all particles
	BoundingBox<T4f> particleBounds = loadBounds<T4f>(mClothData.mCurBounds);

	if (mClothData.mEnableContinuousCollision)
	{
		// extend bounds to include movement from previous frame
		sphereBounds = expandBounds(sphereBounds, mPrevData.mSpheres, mPrevData.mSpheres + mClothData.mNumSpheres);
		particleBounds = expandBounds(particleBounds, loadBounds<T4f>(mClothData.mPrevBounds));
	}

	BoundingBox<T4f> bounds = intersectBounds(sphereBounds, particleBounds);

	// no collision checks needed if the intersection between particle bounds and sphere bounds is empty
	T4f edgeLength = (bounds.mUpper - bounds.mLower) & ~static_cast<T4f>(sMaskW);
	if (!allGreaterEqual(edgeLength, gSimd4fZero))
		return false;

	// calculate an expanded bounds to account for numerical inaccuracy
	const T4f expandedLower = bounds.mLower - abs(bounds.mLower) * sGridExpand;
	const T4f expandedUpper = bounds.mUpper + abs(bounds.mUpper) * sGridExpand;
	const T4f expandedEdgeLength = max(expandedUpper - expandedLower, gSimd4fEpsilon);

	// make grid minimal thickness and strict upper bound of spheres
	// grid maps bounds to 0-7 space (sGridLength =~= 8)
	mGridScale = sGridLength * recip<1>(expandedEdgeLength);
	mGridBias = -expandedLower * mGridScale;
	array(mGridBias)[3] = 1.0f; // needed for collideVirtualParticles()

	NV_CLOTH_ASSERT(allTrue(((bounds.mLower * mGridScale + mGridBias) >= simd4f(0.0f)) | sMaskW));
	NV_CLOTH_ASSERT(allTrue(((bounds.mUpper * mGridScale + mGridBias) < simd4f(8.0f)) | sMaskW));

	memset(mSphereGrid, 0, sizeof(uint32_t) * 6 * (sGridSize));
	if (mClothData.mEnableContinuousCollision)
		buildSphereAcceleration(mPrevData.mSpheres);
	buildSphereAcceleration(mCurData.mSpheres);

	memset(mConeGrid, 0, sizeof(uint32_t) * 6 * (sGridSize));
	buildConeAcceleration();

	return true;
}

#ifdef _MSC_VER
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE inline __attribute__((always_inline))
#endif

template <typename T4f>
FORCE_INLINE typename cloth::SwCollision<T4f>::ShapeMask& cloth::SwCollision<T4f>::ShapeMask::
operator = (const ShapeMask& right)
{
	mCones = right.mCones;
	mSpheres = right.mSpheres;
	return *this;
}

template <typename T4f>
FORCE_INLINE typename cloth::SwCollision<T4f>::ShapeMask& cloth::SwCollision<T4f>::ShapeMask::
operator &= (const ShapeMask& right)
{
	mCones = mCones & right.mCones;
	mSpheres = mSpheres & right.mSpheres;
	return *this;
}

// get collision shape masks from a single cell from a single axis of the acceleration structure
template <typename T4f>
FORCE_INLINE typename cloth::SwCollision<T4f>::ShapeMask
cloth::SwCollision<T4f>::getShapeMask(const T4f& position, const T4i* __restrict sphereGrid,
                                         const T4i* __restrict coneGrid)
{
	// position are the grid positions along a single axis (x, y, or z)
	Gather<T4i> gather(intFloor(position));

	// get the bitmask indicating which cones/spheres overlap the grid cell
	ShapeMask result;
	result.mCones = gather(coneGrid);
	result.mSpheres = gather(sphereGrid);
	return result;
}

// lookup acceleration structure and return mask of potential intersection collision shapes
template <typename T4f>
FORCE_INLINE typename cloth::SwCollision<T4f>::ShapeMask
cloth::SwCollision<T4f>::getShapeMask(const T4f* __restrict positions) const
{
	// positions are the particle positions
	T4f posX = positions[0] * splat<0>(mGridScale) + splat<0>(mGridBias);
	T4f posY = positions[1] * splat<1>(mGridScale) + splat<1>(mGridBias);
	T4f posZ = positions[2] * splat<2>(mGridScale) + splat<2>(mGridBias);

	// AND together the bit masks so only the cones/spheres remain
	//  that overlap with the particle posision on all axis
	ShapeMask result = getShapeMask(posX, mSphereGrid, mConeGrid); // X
	result &= getShapeMask(posY, mSphereGrid + 2, mConeGrid + 2); // Y 
	result &= getShapeMask(posZ, mSphereGrid + 4, mConeGrid + 4); // Z

	return result;
}

// lookup acceleration structure and return mask of potential intersection collision shapes for CCD
template <typename T4f>
FORCE_INLINE typename cloth::SwCollision<T4f>::ShapeMask
cloth::SwCollision<T4f>::getShapeMask(const T4f* __restrict prevPos, const T4f* __restrict curPos) const
{
	// same as getShapeMask(const T4f* __restrict positions) but for continuous collision detection
	T4f scaleX = splat<0>(mGridScale);
	T4f scaleY = splat<1>(mGridScale);
	T4f scaleZ = splat<2>(mGridScale);

	T4f biasX = splat<0>(mGridBias);
	T4f biasY = splat<1>(mGridBias);
	T4f biasZ = splat<2>(mGridBias);

	T4f prevX = prevPos[0] * scaleX + biasX;
	T4f prevY = prevPos[1] * scaleY + biasY;
	T4f prevZ = prevPos[2] * scaleZ + biasZ;

	T4f curX = curPos[0] * scaleX + biasX;
	T4f curY = curPos[1] * scaleY + biasY;
	T4f curZ = curPos[2] * scaleZ + biasZ;

	// get maximum extent corner of the AABB containing both prevPos and curPos
	T4f maxX = min(max(prevX, curX), sGridLength);
	T4f maxY = min(max(prevY, curY), sGridLength);
	T4f maxZ = min(max(prevZ, curZ), sGridLength);

	ShapeMask result = getShapeMask(maxX, mSphereGrid, mConeGrid); // X
	result &= getShapeMask(maxY, mSphereGrid + 2, mConeGrid + 2); // Y
	result &= getShapeMask(maxZ, mSphereGrid + 4, mConeGrid + 4); // Z

	// get min extent corner of the AABB containing both prevPos and curPos
	T4f zero = gSimd4fZero;
	T4f minX = max(min(prevX, curX), zero);
	T4f minY = max(min(prevY, curY), zero);
	T4f minZ = max(min(prevZ, curZ), zero);

	result &= getShapeMask(minX, mSphereGrid + 6, mConeGrid + 6); // X
	result &= getShapeMask(minY, mSphereGrid + 8, mConeGrid + 8); // Y
	result &= getShapeMask(minZ, mSphereGrid + 10, mConeGrid + 10); // Z

	return result;
}

template <typename T4f>
struct cloth::SwCollision<T4f>::ImpulseAccumulator
{
	ImpulseAccumulator()
	: mDeltaX(gSimd4fZero)
	, mDeltaY(mDeltaX)
	, mDeltaZ(mDeltaX)
	, mVelX(mDeltaX)
	, mVelY(mDeltaX)
	, mVelZ(mDeltaX)
	, mNumCollisions(gSimd4fEpsilon)
	{
	}

	void add(const T4f& x, const T4f& y, const T4f& z, const T4f& scale, const T4f& mask)
	{
		NV_CLOTH_ASSERT(allTrue((mask & x) == (mask & x)));
		NV_CLOTH_ASSERT(allTrue((mask & y) == (mask & y)));
		NV_CLOTH_ASSERT(allTrue((mask & z) == (mask & z)));
		NV_CLOTH_ASSERT(allTrue((mask & scale) == (mask & scale)));

		T4f maskedScale = scale & mask;
		mDeltaX = mDeltaX + x * maskedScale;
		mDeltaY = mDeltaY + y * maskedScale;
		mDeltaZ = mDeltaZ + z * maskedScale;
		mNumCollisions = mNumCollisions + (gSimd4fOne & mask);
	}

	void addVelocity(const T4f& vx, const T4f& vy, const T4f& vz, const T4f& mask)
	{
		NV_CLOTH_ASSERT(allTrue((mask & vx) == (mask & vx)));
		NV_CLOTH_ASSERT(allTrue((mask & vy) == (mask & vy)));
		NV_CLOTH_ASSERT(allTrue((mask & vz) == (mask & vz)));

		mVelX = mVelX + (vx & mask);
		mVelY = mVelY + (vy & mask);
		mVelZ = mVelZ + (vz & mask);
	}

	void subtract(const T4f& x, const T4f& y, const T4f& z, const T4f& scale, const T4f& mask)
	{
		NV_CLOTH_ASSERT(allTrue((mask & x) == (mask & x)));
		NV_CLOTH_ASSERT(allTrue((mask & y) == (mask & y)));
		NV_CLOTH_ASSERT(allTrue((mask & z) == (mask & z)));
		NV_CLOTH_ASSERT(allTrue((mask & scale) == (mask & scale)));

		T4f maskedScale = scale & mask;
		mDeltaX = mDeltaX - x * maskedScale;
		mDeltaY = mDeltaY - y * maskedScale;
		mDeltaZ = mDeltaZ - z * maskedScale;
		mNumCollisions = mNumCollisions + (gSimd4fOne & mask);
	}

	T4f mDeltaX, mDeltaY, mDeltaZ; //depenetration delta
	T4f mVelX, mVelY, mVelZ; //frame offset of the collision shape (velocity * dt)
	T4f mNumCollisions;
};

template <typename T4f>
FORCE_INLINE void cloth::SwCollision<T4f>::collideSpheres(const T4i& sphereMask, const T4f* positions,
                                                             ImpulseAccumulator& accum) const
{
	const float* __restrict spherePtr = array(mCurData.mSpheres->center);

	bool frictionEnabled = mClothData.mFrictionScale > 0.0f;

	T4i mask4 = horizontalOr(sphereMask);
	uint32_t mask = uint32_t(array(mask4)[0]);
	while (mask)
	{
		uint32_t test = mask - 1;
		uint32_t offset = findBitSet(mask & ~test) * sizeof(SphereData);
		mask = mask & test;

		T4f sphere = loadAligned(spherePtr, offset);

		T4f deltaX = positions[0] - splat<0>(sphere);
		T4f deltaY = positions[1] - splat<1>(sphere);
		T4f deltaZ = positions[2] - splat<2>(sphere);

		T4f sqrDistance = gSimd4fEpsilon + deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;
		T4f negativeScale = gSimd4fOne - rsqrt(sqrDistance) * splat<3>(sphere);
		// negativeScale = 1 - radius/|position-sphere|

		T4f contactMask;
		if (!anyGreater(gSimd4fZero, negativeScale, contactMask))
			continue;

		accum.subtract(deltaX, deltaY, deltaZ, negativeScale, contactMask);
		// -= delta * negativeScale
		//  = delta - delta * radius/|position-sphere|

		if (frictionEnabled)
		{
			// load previous sphere pos
			const float* __restrict prevSpherePtr = array(mPrevData.mSpheres->center);

			T4f prevSphere = loadAligned(prevSpherePtr, offset);
			T4f velocity = sphere - prevSphere;

			accum.addVelocity(splat<0>(velocity), splat<1>(velocity), splat<2>(velocity), contactMask);
		}
	}
}

template <typename T4f>
FORCE_INLINE typename cloth::SwCollision<T4f>::T4i
cloth::SwCollision<T4f>::collideCones(const T4f* __restrict positions, ImpulseAccumulator& accum) const
{
	const float* __restrict centerPtr = array(mCurData.mCones->center);
	const float* __restrict axisPtr = array(mCurData.mCones->axis);
	const int32_t* __restrict auxiliaryPtr = reinterpret_cast<const int32_t*>(&mCurData.mCones->sqrCosine);

	bool frictionEnabled = mClothData.mFrictionScale > 0.0f;

	ShapeMask shapeMask = getShapeMask(positions);
	T4i mask4 = horizontalOr(shapeMask.mCones);
	uint32_t mask = uint32_t(array(mask4)[0]);
	while (mask)
	{
		uint32_t test = mask - 1;
		uint32_t coneIndex = findBitSet(mask & ~test);
		uint32_t offset = coneIndex * sizeof(ConeData);
		mask = mask & test;

		T4i test4 = mask4 - gSimd4iOne;
		T4f culled = simd4f(andNotIsZero(shapeMask.mCones, test4));
		mask4 = mask4 & test4;

		T4f center = loadAligned(centerPtr, offset);

		// offset from center of cone to particle
		// delta = pos - center
		T4f deltaX = positions[0] - splat<0>(center);
		T4f deltaY = positions[1] - splat<1>(center);
		T4f deltaZ = positions[2] - splat<2>(center);

		//axis of the cone
		T4f axis = loadAligned(axisPtr, offset);

		T4f axisX = splat<0>(axis);
		T4f axisY = splat<1>(axis);
		T4f axisZ = splat<2>(axis);
		T4f slope = splat<3>(axis);

		// distance along cone axis (from center)
		T4f dot = deltaX * axisX + deltaY * axisY + deltaZ * axisZ;
		// interpolate radius
		T4f radius = dot * slope + splat<3>(center);

		// set radius to zero if cone is culled
		radius = max(radius, gSimd4fZero) & ~culled;

		// distance to axis
		// sqrDistance = |delta|^2 - |dot|^2
		T4f sqrDistance = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ - dot * dot;

		T4i auxiliary = loadAligned(auxiliaryPtr, offset);
		T4i bothMask = splat<3>(auxiliary);

		T4f contactMask;
		if (!anyGreater(radius * radius, sqrDistance, contactMask))
		{
			// cone only culled when spheres culled, ok to clear those too
			shapeMask.mSpheres = shapeMask.mSpheres & ~bothMask;
			continue;
		}

		// clamp to a small positive epsilon to avoid numerical error
		// making sqrDistance negative when point lies on the cone axis
		sqrDistance = max(sqrDistance, gSimd4fEpsilon);

		T4f invDistance = rsqrt(sqrDistance);

		//offset base to take slope in to account
		T4f base = dot + slope * sqrDistance * invDistance;

		// force left/rightMask to false if not inside cone
		base = base & contactMask;

		T4f halfLength = splat<1>(simd4f(auxiliary));
		T4i leftMask = simd4i(base < -halfLength);
		T4i rightMask = simd4i(base > halfLength);

		// we use both mask because of the early out above.
		T4i firstMask = splat<2>(auxiliary);
		T4i secondMask = firstMask ^ bothMask;
		shapeMask.mSpheres = shapeMask.mSpheres & ~(firstMask & ~leftMask);
		shapeMask.mSpheres = shapeMask.mSpheres & ~(secondMask & ~rightMask);

		//contact normal direction
		deltaX = deltaX - base * axisX;
		deltaY = deltaY - base * axisY;
		deltaZ = deltaZ - base * axisZ;

		T4f sqrCosine = splat<0>(simd4f(auxiliary));
		T4f scale = radius * invDistance * sqrCosine - sqrCosine;

		contactMask = contactMask & ~simd4f(leftMask | rightMask);

		if (!anyTrue(contactMask))
			continue;

		accum.add(deltaX, deltaY, deltaZ, scale, contactMask);

		if (frictionEnabled)
		{
			uint32_t s0 = mClothData.mCapsuleIndices[coneIndex].first;
			uint32_t s1 = mClothData.mCapsuleIndices[coneIndex].second;

			float* prevSpheres = reinterpret_cast<float*>(mPrevData.mSpheres);
			float* curSpheres = reinterpret_cast<float*>(mCurData.mSpheres);

			// todo: could pre-compute sphere velocities or it might be
			// faster to compute cur/prev sphere positions directly
			T4f s0p0 = loadAligned(prevSpheres, s0 * sizeof(SphereData));
			T4f s0p1 = loadAligned(curSpheres, s0 * sizeof(SphereData));

			T4f s1p0 = loadAligned(prevSpheres, s1 * sizeof(SphereData));
			T4f s1p1 = loadAligned(curSpheres, s1 * sizeof(SphereData));

			T4f v0 = s0p1 - s0p0;
			T4f v1 = s1p1 - s1p0;
			T4f vd = v1 - v0;

			// dot is in the range -1 to 1, scale and bias to 0 to 1
			dot = dot * gSimd4fHalf + gSimd4fHalf;

			// interpolate velocity at contact points
			T4f vx = splat<0>(v0) + dot * splat<0>(vd);
			T4f vy = splat<1>(v0) + dot * splat<1>(vd);
			T4f vz = splat<2>(v0) + dot * splat<2>(vd);

			accum.addVelocity(vx, vy, vz, contactMask);
		}
	}

	return shapeMask.mSpheres;
}

template <typename T4f>
FORCE_INLINE void cloth::SwCollision<T4f>::collideSpheres(const T4i& sphereMask, const T4f* __restrict prevPos,
                                                             T4f* __restrict curPos, ImpulseAccumulator& accum) const
{
	const float* __restrict prevSpheres = array(mPrevData.mSpheres->center);
	const float* __restrict curSpheres = array(mCurData.mSpheres->center);

	bool frictionEnabled = mClothData.mFrictionScale > 0.0f;

	T4i mask4 = horizontalOr(sphereMask);
	uint32_t mask = uint32_t(array(mask4)[0]);
	while (mask)
	{
		uint32_t test = mask - 1;
		uint32_t offset = findBitSet(mask & ~test) * sizeof(SphereData);
		mask = mask & test;

		T4f prevSphere = loadAligned(prevSpheres, offset);
		T4f prevX = prevPos[0] - splat<0>(prevSphere);
		T4f prevY = prevPos[1] - splat<1>(prevSphere);
		T4f prevZ = prevPos[2] - splat<2>(prevSphere);
		T4f prevRadius = splat<3>(prevSphere);

		T4f curSphere = loadAligned(curSpheres, offset);
		T4f curX = curPos[0] - splat<0>(curSphere);
		T4f curY = curPos[1] - splat<1>(curSphere);
		T4f curZ = curPos[2] - splat<2>(curSphere);
		T4f curRadius = splat<3>(curSphere);

		T4f sqrDistance = gSimd4fEpsilon + curX * curX + curY * curY + curZ * curZ;

		T4f dotPrevPrev = prevX * prevX + prevY * prevY + prevZ * prevZ - prevRadius * prevRadius;
		T4f dotPrevCur = prevX * curX + prevY * curY + prevZ * curZ - prevRadius * curRadius;
		T4f dotCurCur = sqrDistance - curRadius * curRadius;

		T4f discriminant = dotPrevCur * dotPrevCur - dotCurCur * dotPrevPrev;
		T4f sqrtD = sqrt(discriminant); //we get -nan if there are no roots
		T4f halfB = dotPrevCur - dotPrevPrev;
		T4f minusA = dotPrevCur - dotCurCur + halfB;

		// time of impact or 0 if prevPos inside sphere
		T4f toi = recip(minusA) * min(gSimd4fZero, halfB + sqrtD);
		T4f collisionMask = (toi < gSimd4fOne) & (halfB < sqrtD);

		// skip continuous collision if the (un-clamped) particle
		// trajectory only touches the outer skin of the cone.
		T4f rMin = prevRadius + halfB * minusA * (curRadius - prevRadius);
		collisionMask = collisionMask & (discriminant > minusA * rMin * rMin * sSkeletonWidth);

		// a is negative when one relative sphere is contained in the other,
		// which is already handled by discrete collision.
		collisionMask = collisionMask & (minusA < -static_cast<T4f>(gSimd4fEpsilon));

		if (!allEqual(collisionMask, gSimd4fZero))
		{
			T4f deltaX = prevX - curX;
			T4f deltaY = prevY - curY;
			T4f deltaZ = prevZ - curZ;

			T4f oneMinusToi = (gSimd4fOne - toi) & collisionMask;

			// reduce ccd impulse if (clamped) particle trajectory stays in sphere skin,
			// i.e. scale by exp2(-k) or 1/(1+k) with k = (tmin - toi) / (1 - toi)
			T4f minusK = sqrtD * recip(minusA * oneMinusToi) & (oneMinusToi > gSimd4fEpsilon);
			oneMinusToi = oneMinusToi * recip(gSimd4fOne - minusK);


			//Move curXYZ to toi point
			curX = curX + deltaX * oneMinusToi;
			curY = curY + deltaY * oneMinusToi;
			curZ = curZ + deltaZ * oneMinusToi;
			//curXYZ is now touching the sphere at toi
			//Note that curXYZ is also relative to the sphere center at toi

			//We assume that the point sticks to the sphere until the end of the frame
			curPos[0] = splat<0>(curSphere) + curX;
			curPos[1] = splat<1>(curSphere) + curY;
			curPos[2] = splat<2>(curSphere) + curZ;

			sqrDistance = gSimd4fEpsilon + curX * curX + curY * curY + curZ * curZ;
		}

		T4f negativeScale = gSimd4fOne - rsqrt(sqrDistance) * curRadius;

		T4f contactMask;
		if (!anyGreater(gSimd4fZero, negativeScale, contactMask))
			continue;

		accum.subtract(curX, curY, curZ, negativeScale, contactMask);

		if (frictionEnabled)
		{
			T4f velocity = curSphere - prevSphere;
			accum.addVelocity(splat<0>(velocity), splat<1>(velocity), splat<2>(velocity), contactMask);
		}
	}
}

template <typename T4f>
FORCE_INLINE typename cloth::SwCollision<T4f>::T4i
cloth::SwCollision<T4f>::collideCones(const T4f* __restrict prevPos, T4f* __restrict curPos,
                                         ImpulseAccumulator& accum) const
{
	const float* __restrict prevCenterPtr = array(mPrevData.mCones->center);
	const float* __restrict prevAxisPtr = array(mPrevData.mCones->axis);
	const int32_t* __restrict prevAuxiliaryPtr = reinterpret_cast<const int32_t*>(&mPrevData.mCones->sqrCosine);

	const float* __restrict curCenterPtr = array(mCurData.mCones->center);
	const float* __restrict curAxisPtr = array(mCurData.mCones->axis);
	const int32_t* __restrict curAuxiliaryPtr = reinterpret_cast<const int32_t*>(&mCurData.mCones->sqrCosine);

	bool frictionEnabled = mClothData.mFrictionScale > 0.0f;

	ShapeMask shapeMask = getShapeMask(prevPos, curPos);
	T4i mask4 = horizontalOr(shapeMask.mCones);
	uint32_t mask = uint32_t(array(mask4)[0]);
	while (mask)
	{
		uint32_t test = mask - 1;
		uint32_t coneIndex = findBitSet(mask & ~test);
		uint32_t offset = coneIndex * sizeof(ConeData);
		mask = mask & test;

		T4i test4 = mask4 - gSimd4iOne;
		T4f culled = simd4f(andNotIsZero(shapeMask.mCones, test4));
		mask4 = mask4 & test4;

		T4f prevCenter = loadAligned(prevCenterPtr, offset);
		T4f prevAxis = loadAligned(prevAxisPtr, offset);
		T4f prevAxisX = splat<0>(prevAxis);
		T4f prevAxisY = splat<1>(prevAxis);
		T4f prevAxisZ = splat<2>(prevAxis);
		T4f prevSlope = splat<3>(prevAxis);

		T4f prevX = prevPos[0] - splat<0>(prevCenter);
		T4f prevY = prevPos[1] - splat<1>(prevCenter);
		T4f prevZ = prevPos[2] - splat<2>(prevCenter);
		T4f prevT = prevY * prevAxisZ - prevZ * prevAxisY;
		T4f prevU = prevZ * prevAxisX - prevX * prevAxisZ;
		T4f prevV = prevX * prevAxisY - prevY * prevAxisX;
		T4f prevDot = prevX * prevAxisX + prevY * prevAxisY + prevZ * prevAxisZ; //distance along the axis
		T4f prevRadius = prevDot * prevSlope + splat<3>(prevCenter);

		T4f curCenter = loadAligned(curCenterPtr, offset);
		T4f curAxis = loadAligned(curAxisPtr, offset);
		T4f curAxisX = splat<0>(curAxis);
		T4f curAxisY = splat<1>(curAxis);
		T4f curAxisZ = splat<2>(curAxis);
		T4f curSlope = splat<3>(curAxis);
		T4i curAuxiliary = loadAligned(curAuxiliaryPtr, offset);

		T4f curX = curPos[0] - splat<0>(curCenter);
		T4f curY = curPos[1] - splat<1>(curCenter);
		T4f curZ = curPos[2] - splat<2>(curCenter);
		//curTUV = cross(curXYZ, curAxisXYZ)
		T4f curT = curY * curAxisZ - curZ * curAxisY;
		T4f curU = curZ * curAxisX - curX * curAxisZ;
		T4f curV = curX * curAxisY - curY * curAxisX;
		T4f curDot = curX * curAxisX + curY * curAxisY + curZ * curAxisZ;
		T4f curRadius = curDot * curSlope + splat<3>(curCenter);

		//Magnitude of cross product gives area of parallelogram |curAxisXYZ|*parallelogramHeight, |curAxisXYZ|=1 
		//parallelogramHeight is distance between the axis and the point
		T4f curSqrDistance = gSimd4fEpsilon + curT * curT + curU * curU + curV * curV;

		// set radius to zero if cone is culled
		prevRadius = max(prevRadius, gSimd4fZero) & ~culled;
		curRadius = max(curRadius, gSimd4fZero) & ~culled;

		//Use quadratic equation to solve for time of impact (against infinite cone)
		T4f dotPrevPrev = prevT * prevT + prevU * prevU + prevV * prevV - prevRadius * prevRadius;
		T4f dotPrevCur = prevT * curT + prevU * curU + prevV * curV - prevRadius * curRadius;
		T4f dotCurCur = curSqrDistance - curRadius * curRadius;

		T4f discriminant = dotPrevCur * dotPrevCur - dotCurCur * dotPrevPrev;
		T4f sqrtD = sqrt(discriminant);
		T4f halfB = dotPrevCur - dotPrevPrev;
		T4f minusA = dotPrevCur - dotCurCur + halfB;

		// time of impact or 0 if prevPos inside cone
		T4f toi = recip(minusA) * min(gSimd4fZero, halfB + sqrtD);
		T4f collisionMask = (toi < gSimd4fOne) & (halfB < sqrtD);

		// skip continuous collision if the (un-clamped) particle
		// trajectory only touches the outer skin of the cone.
		T4f rMin = prevRadius + halfB * minusA * (curRadius - prevRadius);
		collisionMask = collisionMask & (discriminant > minusA * rMin * rMin * sSkeletonWidth);

		// a is negative when one cone is contained in the other,
		// which is already handled by discrete collision.
		collisionMask = collisionMask & (minusA < -static_cast<T4f>(gSimd4fEpsilon));

		// test if any particle hits infinite cone (and 0<time of impact<1)
		if (!allEqual(collisionMask, gSimd4fZero))
		{
			T4f deltaX = prevX - curX;
			T4f deltaY = prevY - curY;
			T4f deltaZ = prevZ - curZ;

			// interpolate delta at toi
			T4f posX = prevX - deltaX * toi;
			T4f posY = prevY - deltaY * toi;
			T4f posZ = prevZ - deltaZ * toi;

			//                                axisHalfLength
			T4f curScaledAxis = curAxis * splat<1>(simd4f(curAuxiliary));
			T4i prevAuxiliary = loadAligned(prevAuxiliaryPtr, offset);
			T4f deltaScaledAxis = curScaledAxis - prevAxis * splat<1>(simd4f(prevAuxiliary));

			T4f oneMinusToi = gSimd4fOne - toi;

			// interpolate axis at toi
			T4f axisX = splat<0>(curScaledAxis) - splat<0>(deltaScaledAxis) * oneMinusToi;
			T4f axisY = splat<1>(curScaledAxis) - splat<1>(deltaScaledAxis) * oneMinusToi;
			T4f axisZ = splat<2>(curScaledAxis) - splat<2>(deltaScaledAxis) * oneMinusToi;
			T4f slope = (prevSlope * oneMinusToi + curSlope * toi);

			T4f sqrHalfLength = axisX * axisX + axisY * axisY + axisZ * axisZ;
			T4f invHalfLength = rsqrt(sqrHalfLength);
			// distance along toi cone axis (from center)
			T4f dot = (posX * axisX + posY * axisY + posZ * axisZ) * invHalfLength;

			//point line distance
			T4f sqrDistance = posX * posX + posY * posY + posZ * posZ - dot * dot;
			T4f invDistance = rsqrt(sqrDistance) & (sqrDistance > gSimd4fZero);

			//offset base to take slope in to account
			T4f base = dot + slope * sqrDistance * invDistance;
			T4f scale = base * invHalfLength & collisionMask;
			// use invHalfLength to map base from [-HalfLength, +HalfLength]=inside to [-1, +1] =inside

			// test if any impact position is in cone section
			T4f cullMask = (abs(scale) < gSimd4fOne) & collisionMask;

			// test if any impact position is in cone section
			if (!allEqual(cullMask, gSimd4fZero))
			{
				//calculate unnormalized normal delta?
				//delta = prev - cur - (prevAxis - curScaledAxis)*scale
				deltaX = deltaX + splat<0>(deltaScaledAxis) * scale;
				deltaY = deltaY + splat<1>(deltaScaledAxis) * scale;
				deltaZ = deltaZ + splat<2>(deltaScaledAxis) * scale;

				oneMinusToi = oneMinusToi & cullMask;

				// reduce ccd impulse if (clamped) particle trajectory stays in cone skin,
				// i.e. scale by exp2(-k) or 1/(1+k) with k = (tmin - toi) / (1 - toi)
				// oneMinusToi = oneMinusToi * recip(gSimd4fOne - sqrtD * recip(minusA * oneMinusToi));
				T4f minusK = sqrtD * recip(minusA * oneMinusToi) & (oneMinusToi > gSimd4fEpsilon);
				oneMinusToi = oneMinusToi * recip(gSimd4fOne - minusK);

				//curX = cur + (prev - cur - (prevAxis - curScaledAxis)*scale)*(1-toi)
				//curX = cur + (prev - cur)*(1-toi) - ((prevAxis - curScaledAxis)*scale)*(1-toi)
				curX = curX + deltaX * oneMinusToi; 
				curY = curY + deltaY * oneMinusToi;
				curZ = curZ + deltaZ * oneMinusToi;

				//save adjusted values for discrete collision detection below
				curDot = curX * curAxisX + curY * curAxisY + curZ * curAxisZ;
				curRadius = curDot * curSlope + splat<3>(curCenter);
				curRadius = max(curRadius, gSimd4fZero) & ~culled;
				curSqrDistance = curX * curX + curY * curY + curZ * curZ - curDot * curDot;

				//take offset from toi and add it to the cone center at the end
				curPos[0] = splat<0>(curCenter) + curX;
				curPos[1] = splat<1>(curCenter) + curY;
				curPos[2] = splat<2>(curCenter) + curZ;
			}
		}

		// curPos inside cone (discrete collision)
		T4f contactMask;
		int anyContact = anyGreater(curRadius * curRadius, curSqrDistance, contactMask);

		T4i bothMask = splat<3>(curAuxiliary);

		// instead of culling continuous collision for ~collisionMask, and discrete
		// collision for ~contactMask, disable both if ~collisionMask & ~contactMask
		T4i cullMask = bothMask & ~simd4i(collisionMask | contactMask);
		shapeMask.mSpheres = shapeMask.mSpheres & ~cullMask;

		if (!anyContact)
			continue;

		T4f invDistance = rsqrt(curSqrDistance) & (curSqrDistance > gSimd4fZero);
		T4f base = curDot + curSlope * curSqrDistance * invDistance;

		T4f halfLength = splat<1>(simd4f(curAuxiliary));
		T4i leftMask = simd4i(base < -halfLength);
		T4i rightMask = simd4i(base > halfLength);

		// can only skip continuous sphere collision if post-ccd position
		// is on code side *and* particle had cone-ccd collision.
		T4i firstMask = splat<2>(curAuxiliary);
		T4i secondMask = firstMask ^ bothMask;
		cullMask = (firstMask & ~leftMask) | (secondMask & ~rightMask);
		shapeMask.mSpheres = shapeMask.mSpheres & ~(cullMask & simd4i(collisionMask));

		T4f deltaX = curX - base * curAxisX;
		T4f deltaY = curY - base * curAxisY;
		T4f deltaZ = curZ - base * curAxisZ;

		T4f sqrCosine = splat<0>(simd4f(curAuxiliary));
		T4f scale = curRadius * invDistance * sqrCosine - sqrCosine;

		contactMask = contactMask & ~simd4f(leftMask | rightMask);

		if (!anyTrue(contactMask))
			continue;

		accum.add(deltaX, deltaY, deltaZ, scale, contactMask);

		if (frictionEnabled)
		{
			uint32_t s0 = mClothData.mCapsuleIndices[coneIndex].first;
			uint32_t s1 = mClothData.mCapsuleIndices[coneIndex].second;

			float* prevSpheres = reinterpret_cast<float*>(mPrevData.mSpheres);
			float* curSpheres = reinterpret_cast<float*>(mCurData.mSpheres);

			// todo: could pre-compute sphere velocities or it might be
			// faster to compute cur/prev sphere positions directly
			T4f s0p0 = loadAligned(prevSpheres, s0 * sizeof(SphereData));
			T4f s0p1 = loadAligned(curSpheres, s0 * sizeof(SphereData));

			T4f s1p0 = loadAligned(prevSpheres, s1 * sizeof(SphereData));
			T4f s1p1 = loadAligned(curSpheres, s1 * sizeof(SphereData));

			T4f v0 = s0p1 - s0p0;
			T4f v1 = s1p1 - s1p0;
			T4f vd = v1 - v0;

			// dot is in the range -1 to 1, scale and bias to 0 to 1
			curDot = curDot * gSimd4fHalf + gSimd4fHalf;

			// interpolate velocity at contact points
			T4f vx = splat<0>(v0) + curDot * splat<0>(vd);
			T4f vy = splat<1>(v0) + curDot * splat<1>(vd);
			T4f vz = splat<2>(v0) + curDot * splat<2>(vd);

			accum.addVelocity(vx, vy, vz, contactMask);
		}
	}

	return shapeMask.mSpheres;
}

namespace
{

template <typename T4f>
PX_INLINE void calculateFrictionImpulse(const T4f& deltaX, const T4f& deltaY, const T4f& deltaZ,
                                        const T4f& velX, const T4f& velY, const T4f& velZ,
                                        const T4f* curPos, const T4f* prevPos, const T4f& scale,
                                        const T4f& coefficient, const T4f& mask, T4f* impulse)
{
	// calculate collision normal
	T4f deltaSq = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;

	T4f rcpDelta = rsqrt(deltaSq + gSimd4fEpsilon);

	T4f nx = deltaX * rcpDelta;
	T4f ny = deltaY * rcpDelta;
	T4f nz = deltaZ * rcpDelta;

	// calculate relative velocity
	// velXYZ is scaled by one over the number of collisions (scale) since all collisions accumulate into 
	//  that variable during collision detection
	T4f rvx = curPos[0] - prevPos[0] - velX * scale;
	T4f rvy = curPos[1] - prevPos[1] - velY * scale;
	T4f rvz = curPos[2] - prevPos[2] - velZ * scale;

	// calculate magnitude of relative normal velocity
	T4f rvn = rvx * nx + rvy * ny + rvz * nz;

	// calculate relative tangential velocity
	T4f rvtx = rvx - rvn * nx;
	T4f rvty = rvy - rvn * ny;
	T4f rvtz = rvz - rvn * nz;

	// calculate magnitude of relative tangential velocity
	T4f rcpVt = rsqrt(rvtx * rvtx + rvty * rvty + rvtz * rvtz + gSimd4fEpsilon);

	// magnitude of friction impulse (cannot be greater than -rvt)
	T4f j = max(-coefficient * deltaSq * rcpDelta * rcpVt, gSimd4fMinusOne) & mask;

	impulse[0] = rvtx * j;
	impulse[1] = rvty * j;
	impulse[2] = rvtz * j;
}

} // anonymous namespace

template <typename T4f>
void cloth::SwCollision<T4f>::collideParticles()
{
	const bool massScalingEnabled = mClothData.mCollisionMassScale > 0.0f;
	const T4f massScale = simd4f(mClothData.mCollisionMassScale);

	const bool frictionEnabled = mClothData.mFrictionScale > 0.0f;
	const T4f frictionScale = simd4f(mClothData.mFrictionScale); //Parameter set by user

	T4f curPos[4];
	T4f prevPos[4];

	float* __restrict prevIt = mClothData.mPrevParticles;
	float* __restrict pIt = mClothData.mCurParticles;
	float* __restrict pEnd = pIt + mClothData.mNumParticles * 4;
	//loop over particles 4 at a time
	for (; pIt < pEnd; pIt += 16, prevIt += 16)
	{
		curPos[0] = loadAligned(pIt, 0);
		curPos[1] = loadAligned(pIt, 16);
		curPos[2] = loadAligned(pIt, 32);
		curPos[3] = loadAligned(pIt, 48);
		transpose(curPos[0], curPos[1], curPos[2], curPos[3]); //group values by axis in simd structure

		ImpulseAccumulator accum;

		//first collide cones
		T4i sphereMask = collideCones(curPos, accum);
		//pass on hit mask to ignore sphere parts that are inside the cones
		collideSpheres(sphereMask, curPos, accum);

		T4f mask;
		if (!anyGreater(accum.mNumCollisions, gSimd4fEpsilon, mask))
			continue;

		T4f invNumCollisions = recip(accum.mNumCollisions);

		if (frictionEnabled)
		{
			prevPos[0] = loadAligned(prevIt, 0);
			prevPos[1] = loadAligned(prevIt, 16);
			prevPos[2] = loadAligned(prevIt, 32);
			prevPos[3] = loadAligned(prevIt, 48);
			transpose(prevPos[0], prevPos[1], prevPos[2], prevPos[3]);

			T4f frictionImpulse[3];
			calculateFrictionImpulse(accum.mDeltaX, accum.mDeltaY, accum.mDeltaZ, accum.mVelX, accum.mVelY, accum.mVelZ,
			                         curPos, prevPos, invNumCollisions, frictionScale, mask, frictionImpulse);

			prevPos[0] = prevPos[0] - frictionImpulse[0];
			prevPos[1] = prevPos[1] - frictionImpulse[1];
			prevPos[2] = prevPos[2] - frictionImpulse[2];

			transpose(prevPos[0], prevPos[1], prevPos[2], prevPos[3]);
			storeAligned(prevIt, 0, prevPos[0]);
			storeAligned(prevIt, 16, prevPos[1]);
			storeAligned(prevIt, 32, prevPos[2]);
			storeAligned(prevIt, 48, prevPos[3]);
		}

		if (massScalingEnabled)
		{
			// calculate the inverse mass scale based on the collision impulse magnitude
			T4f dSq = invNumCollisions * invNumCollisions *
			             (accum.mDeltaX * accum.mDeltaX + accum.mDeltaY * accum.mDeltaY + accum.mDeltaZ * accum.mDeltaZ);

			T4f scale = recip(gSimd4fOne + massScale * dSq);

			// scale invmass
			curPos[3] = select(mask, curPos[3] * scale, curPos[3]);
		}

		//apply average de-penetration delta
		curPos[0] = curPos[0] + accum.mDeltaX * invNumCollisions;
		curPos[1] = curPos[1] + accum.mDeltaY * invNumCollisions;
		curPos[2] = curPos[2] + accum.mDeltaZ * invNumCollisions;

		transpose(curPos[0], curPos[1], curPos[2], curPos[3]);
		storeAligned(pIt, 0, curPos[0]);
		storeAligned(pIt, 16, curPos[1]);
		storeAligned(pIt, 32, curPos[2]);
		storeAligned(pIt, 48, curPos[3]);

#if PX_PROFILE || PX_DEBUG
		mNumCollisions += horizontalSum(accum.mNumCollisions);
#endif
	}
}

template <typename T4f>
void cloth::SwCollision<T4f>::collideVirtualParticles()
{
	const bool massScalingEnabled = mClothData.mCollisionMassScale > 0.0f;
	const T4f massScale = simd4f(mClothData.mCollisionMassScale);

	const bool frictionEnabled = mClothData.mFrictionScale > 0.0f;
	const T4f frictionScale = simd4f(mClothData.mFrictionScale);

	T4f curPos[3];

	const float* __restrict weights = mClothData.mVirtualParticleWeights;
	float* __restrict particles = mClothData.mCurParticles;
	float* __restrict prevParticles = mClothData.mPrevParticles;

	// move dummy particles outside of collision range
	T4f* __restrict dummy = mClothData.mNumParticles + reinterpret_cast<T4f*>(mClothData.mCurParticles);
	T4f invGridScale = recip(mGridScale) & (mGridScale > gSimd4fEpsilon);
	dummy[0] = dummy[1] = dummy[2] = invGridScale * mGridBias - invGridScale;

	const uint16_t* __restrict vpIt = mClothData.mVirtualParticlesBegin;
	const uint16_t* __restrict vpEnd = mClothData.mVirtualParticlesEnd;
	for (; vpIt != vpEnd; vpIt += 16)
	{
		// load 12 particles and 4 weights
		T4f p0v0 = loadAligned(particles, vpIt[0] * sizeof(PxVec4));
		T4f p0v1 = loadAligned(particles, vpIt[1] * sizeof(PxVec4));
		T4f p0v2 = loadAligned(particles, vpIt[2] * sizeof(PxVec4));
		T4f w0 = loadAligned(weights, vpIt[3] * sizeof(PxVec4));

		T4f p1v0 = loadAligned(particles, vpIt[4] * sizeof(PxVec4));
		T4f p1v1 = loadAligned(particles, vpIt[5] * sizeof(PxVec4));
		T4f p1v2 = loadAligned(particles, vpIt[6] * sizeof(PxVec4));
		T4f w1 = loadAligned(weights, vpIt[7] * sizeof(PxVec4));

		T4f p2v0 = loadAligned(particles, vpIt[8] * sizeof(PxVec4));
		T4f p2v1 = loadAligned(particles, vpIt[9] * sizeof(PxVec4));
		T4f p2v2 = loadAligned(particles, vpIt[10] * sizeof(PxVec4));
		T4f w2 = loadAligned(weights, vpIt[11] * sizeof(PxVec4));

		T4f p3v1 = loadAligned(particles, vpIt[13] * sizeof(PxVec4));
		T4f p3v0 = loadAligned(particles, vpIt[12] * sizeof(PxVec4));
		T4f p3v2 = loadAligned(particles, vpIt[14] * sizeof(PxVec4));
		T4f w3 = loadAligned(weights, vpIt[15] * sizeof(PxVec4));

		// interpolate particles and transpose
		T4f px = p0v0 * splat<0>(w0) + p0v1 * splat<1>(w0) + p0v2 * splat<2>(w0);
		T4f py = p1v0 * splat<0>(w1) + p1v1 * splat<1>(w1) + p1v2 * splat<2>(w1);
		T4f pz = p2v0 * splat<0>(w2) + p2v1 * splat<1>(w2) + p2v2 * splat<2>(w2);
		T4f pw = p3v0 * splat<0>(w3) + p3v1 * splat<1>(w3) + p3v2 * splat<2>(w3);
		transpose(px, py, pz, pw);

		curPos[0] = px;
		curPos[1] = py;
		curPos[2] = pz;

		ImpulseAccumulator accum;
		T4i sphereMask = collideCones(curPos, accum);
		collideSpheres(sphereMask, curPos, accum);

		T4f mask;
		if (!anyGreater(accum.mNumCollisions, gSimd4fEpsilon, mask))
			continue;

		T4f invNumCollisions = recip(accum.mNumCollisions);

		// displacement and transpose back
		T4f d0 = accum.mDeltaX * invNumCollisions;
		T4f d1 = accum.mDeltaY * invNumCollisions;
		T4f d2 = accum.mDeltaZ * invNumCollisions;
		T4f d3 = gSimd4fZero;
		transpose(d0, d1, d2, d3);

		// scale weights by 1/dot(w,w)
		T4f rw0 = w0 * splat<3>(w0);
		T4f rw1 = w1 * splat<3>(w1);
		T4f rw2 = w2 * splat<3>(w2);
		T4f rw3 = w3 * splat<3>(w3);

		if (frictionEnabled)
		{
			T4f q0v0 = loadAligned(prevParticles, vpIt[0] * sizeof(PxVec4));
			T4f q0v1 = loadAligned(prevParticles, vpIt[1] * sizeof(PxVec4));
			T4f q0v2 = loadAligned(prevParticles, vpIt[2] * sizeof(PxVec4));

			T4f q1v0 = loadAligned(prevParticles, vpIt[4] * sizeof(PxVec4));
			T4f q1v1 = loadAligned(prevParticles, vpIt[5] * sizeof(PxVec4));
			T4f q1v2 = loadAligned(prevParticles, vpIt[6] * sizeof(PxVec4));

			T4f q2v0 = loadAligned(prevParticles, vpIt[8] * sizeof(PxVec4));
			T4f q2v1 = loadAligned(prevParticles, vpIt[9] * sizeof(PxVec4));
			T4f q2v2 = loadAligned(prevParticles, vpIt[10] * sizeof(PxVec4));

			T4f q3v0 = loadAligned(prevParticles, vpIt[12] * sizeof(PxVec4));
			T4f q3v1 = loadAligned(prevParticles, vpIt[13] * sizeof(PxVec4));
			T4f q3v2 = loadAligned(prevParticles, vpIt[14] * sizeof(PxVec4));

			// calculate previous interpolated positions
			T4f qx = q0v0 * splat<0>(w0) + q0v1 * splat<1>(w0) + q0v2 * splat<2>(w0);
			T4f qy = q1v0 * splat<0>(w1) + q1v1 * splat<1>(w1) + q1v2 * splat<2>(w1);
			T4f qz = q2v0 * splat<0>(w2) + q2v1 * splat<1>(w2) + q2v2 * splat<2>(w2);
			T4f qw = q3v0 * splat<0>(w3) + q3v1 * splat<1>(w3) + q3v2 * splat<2>(w3);
			transpose(qx, qy, qz, qw);

			T4f prevPos[3] = { qx, qy, qz };
			T4f frictionImpulse[4];
			frictionImpulse[3] = gSimd4fZero;

			calculateFrictionImpulse(accum.mDeltaX, accum.mDeltaY, accum.mDeltaZ, accum.mVelX, accum.mVelY, accum.mVelZ,
			                         curPos, prevPos, invNumCollisions, frictionScale, mask, frictionImpulse);

			transpose(frictionImpulse[0], frictionImpulse[1], frictionImpulse[2], frictionImpulse[3]);

			//            (weight         * frictionImpulse   ) &  mask away if particle is static
			q0v0 = q0v0 - ((splat<0>(rw0) * frictionImpulse[0]) & ~(splat<3>(p0v0) == gSimd4fZero));
			q0v1 = q0v1 - ((splat<1>(rw0) * frictionImpulse[0]) & ~(splat<3>(p0v0) == gSimd4fZero));
			q0v2 = q0v2 - ((splat<2>(rw0) * frictionImpulse[0]) & ~(splat<3>(p0v0) == gSimd4fZero));
						  
			q1v0 = q1v0 - ((splat<0>(rw1) * frictionImpulse[1]) & ~(splat<3>(p0v0) == gSimd4fZero));
			q1v1 = q1v1 - ((splat<1>(rw1) * frictionImpulse[1]) & ~(splat<3>(p0v0) == gSimd4fZero));
			q1v2 = q1v2 - ((splat<2>(rw1) * frictionImpulse[1]) & ~(splat<3>(p0v0) == gSimd4fZero));
						  
			q2v0 = q2v0 - ((splat<0>(rw2) * frictionImpulse[2]) & ~(splat<3>(p0v0) == gSimd4fZero));
			q2v1 = q2v1 - ((splat<1>(rw2) * frictionImpulse[2]) & ~(splat<3>(p0v0) == gSimd4fZero));
			q2v2 = q2v2 - ((splat<2>(rw2) * frictionImpulse[2]) & ~(splat<3>(p0v0) == gSimd4fZero));
						  
			q3v0 = q3v0 - ((splat<0>(rw3) * frictionImpulse[3]) & ~(splat<3>(p0v0) == gSimd4fZero));
			q3v1 = q3v1 - ((splat<1>(rw3) * frictionImpulse[3]) & ~(splat<3>(p0v0) == gSimd4fZero));
			q3v2 = q3v2 - ((splat<2>(rw3) * frictionImpulse[3]) & ~(splat<3>(p0v0) == gSimd4fZero));

			// write back prev particles
			storeAligned(prevParticles, vpIt[0] * sizeof(PxVec4), q0v0);
			storeAligned(prevParticles, vpIt[1] * sizeof(PxVec4), q0v1);
			storeAligned(prevParticles, vpIt[2] * sizeof(PxVec4), q0v2);

			storeAligned(prevParticles, vpIt[4] * sizeof(PxVec4), q1v0);
			storeAligned(prevParticles, vpIt[5] * sizeof(PxVec4), q1v1);
			storeAligned(prevParticles, vpIt[6] * sizeof(PxVec4), q1v2);

			storeAligned(prevParticles, vpIt[8] * sizeof(PxVec4), q2v0);
			storeAligned(prevParticles, vpIt[9] * sizeof(PxVec4), q2v1);
			storeAligned(prevParticles, vpIt[10] * sizeof(PxVec4), q2v2);

			storeAligned(prevParticles, vpIt[12] * sizeof(PxVec4), q3v0);
			storeAligned(prevParticles, vpIt[13] * sizeof(PxVec4), q3v1);
			storeAligned(prevParticles, vpIt[14] * sizeof(PxVec4), q3v2);
		}

		if (massScalingEnabled)
		{
			// calculate the inverse mass scale based on the collision impulse
			T4f dSq = invNumCollisions * invNumCollisions *
			             (accum.mDeltaX * accum.mDeltaX + accum.mDeltaY * accum.mDeltaY + accum.mDeltaZ * accum.mDeltaZ);

			T4f weightScale = recip(gSimd4fOne + massScale * dSq);

			weightScale = weightScale - gSimd4fOne;
			T4f s0 = gSimd4fOne + splat<0>(weightScale) * (w0 & splat<0>(mask));
			T4f s1 = gSimd4fOne + splat<1>(weightScale) * (w1 & splat<1>(mask));
			T4f s2 = gSimd4fOne + splat<2>(weightScale) * (w2 & splat<2>(mask));
			T4f s3 = gSimd4fOne + splat<3>(weightScale) * (w3 & splat<3>(mask));

			//we don't care if particles are static here since we are only scaling (zero for static) mass
			p0v0 = p0v0 * (gSimd4fOneXYZ | (splat<0>(s0) & sMaskW));
			p0v1 = p0v1 * (gSimd4fOneXYZ | (splat<1>(s0) & sMaskW));
			p0v2 = p0v2 * (gSimd4fOneXYZ | (splat<2>(s0) & sMaskW));

			p1v0 = p1v0 * (gSimd4fOneXYZ | (splat<0>(s1) & sMaskW));
			p1v1 = p1v1 * (gSimd4fOneXYZ | (splat<1>(s1) & sMaskW));
			p1v2 = p1v2 * (gSimd4fOneXYZ | (splat<2>(s1) & sMaskW));

			p2v0 = p2v0 * (gSimd4fOneXYZ | (splat<0>(s2) & sMaskW));
			p2v1 = p2v1 * (gSimd4fOneXYZ | (splat<1>(s2) & sMaskW));
			p2v2 = p2v2 * (gSimd4fOneXYZ | (splat<2>(s2) & sMaskW));

			p3v0 = p3v0 * (gSimd4fOneXYZ | (splat<0>(s3) & sMaskW));
			p3v1 = p3v1 * (gSimd4fOneXYZ | (splat<1>(s3) & sMaskW));
			p3v2 = p3v2 * (gSimd4fOneXYZ | (splat<2>(s3) & sMaskW));
		}

		//            (weight      * delta) & mask away if particle is static
		p0v0 = p0v0 + ((splat<0>(rw0) * d0) & ~(splat<3>(p0v0) == gSimd4fZero));
		p0v1 = p0v1 + ((splat<1>(rw0) * d0) & ~(splat<3>(p0v1) == gSimd4fZero));
		p0v2 = p0v2 + ((splat<2>(rw0) * d0) & ~(splat<3>(p0v2) == gSimd4fZero));
																
		p1v0 = p1v0 + ((splat<0>(rw1) * d1) & ~(splat<3>(p1v0) == gSimd4fZero));
		p1v1 = p1v1 + ((splat<1>(rw1) * d1) & ~(splat<3>(p1v1) == gSimd4fZero));
		p1v2 = p1v2 + ((splat<2>(rw1) * d1) & ~(splat<3>(p1v2) == gSimd4fZero));
																
		p2v0 = p2v0 + ((splat<0>(rw2) * d2) & ~(splat<3>(p2v0) == gSimd4fZero));
		p2v1 = p2v1 + ((splat<1>(rw2) * d2) & ~(splat<3>(p2v1) == gSimd4fZero));
		p2v2 = p2v2 + ((splat<2>(rw2) * d2) & ~(splat<3>(p2v2) == gSimd4fZero));
																
		p3v0 = p3v0 + ((splat<0>(rw3) * d3) & ~(splat<3>(p3v0) == gSimd4fZero));
		p3v1 = p3v1 + ((splat<1>(rw3) * d3) & ~(splat<3>(p3v1) == gSimd4fZero));
		p3v2 = p3v2 + ((splat<2>(rw3) * d3) & ~(splat<3>(p3v2) == gSimd4fZero));

		// write back particles
		storeAligned(particles, vpIt[0] * sizeof(PxVec4), p0v0);
		storeAligned(particles, vpIt[1] * sizeof(PxVec4), p0v1);
		storeAligned(particles, vpIt[2] * sizeof(PxVec4), p0v2);

		storeAligned(particles, vpIt[4] * sizeof(PxVec4), p1v0);
		storeAligned(particles, vpIt[5] * sizeof(PxVec4), p1v1);
		storeAligned(particles, vpIt[6] * sizeof(PxVec4), p1v2);

		storeAligned(particles, vpIt[8] * sizeof(PxVec4), p2v0);
		storeAligned(particles, vpIt[9] * sizeof(PxVec4), p2v1);
		storeAligned(particles, vpIt[10] * sizeof(PxVec4), p2v2);

		storeAligned(particles, vpIt[12] * sizeof(PxVec4), p3v0);
		storeAligned(particles, vpIt[13] * sizeof(PxVec4), p3v1);
		storeAligned(particles, vpIt[14] * sizeof(PxVec4), p3v2);

#if PX_PROFILE || PX_DEBUG
		mNumCollisions += horizontalSum(accum.mNumCollisions);
#endif
	}
}

template <typename T4f>
void cloth::SwCollision<T4f>::collideContinuousParticles()
{
	T4f curPos[4];
	T4f prevPos[4];

	const bool massScalingEnabled = mClothData.mCollisionMassScale > 0.0f;
	const T4f massScale = simd4f(mClothData.mCollisionMassScale);

	const bool frictionEnabled = mClothData.mFrictionScale > 0.0f;
	const T4f frictionScale = simd4f(mClothData.mFrictionScale);

	float* __restrict prevIt = mClothData.mPrevParticles;
	float* __restrict curIt = mClothData.mCurParticles;
	float* __restrict curEnd = curIt + mClothData.mNumParticles * 4;

	for (; curIt < curEnd; curIt += 16, prevIt += 16)
	{
		prevPos[0] = loadAligned(prevIt, 0);
		prevPos[1] = loadAligned(prevIt, 16);
		prevPos[2] = loadAligned(prevIt, 32);
		prevPos[3] = loadAligned(prevIt, 48);
		transpose(prevPos[0], prevPos[1], prevPos[2], prevPos[3]);

		curPos[0] = loadAligned(curIt, 0);
		curPos[1] = loadAligned(curIt, 16);
		curPos[2] = loadAligned(curIt, 32);
		curPos[3] = loadAligned(curIt, 48);
		transpose(curPos[0], curPos[1], curPos[2], curPos[3]);

		ImpulseAccumulator accum;
		T4i sphereMask = collideCones(prevPos, curPos, accum);
		collideSpheres(sphereMask, prevPos, curPos, accum);

		T4f mask;
		if (!anyGreater(accum.mNumCollisions, gSimd4fEpsilon, mask))
			continue;

		T4f invNumCollisions = recip(accum.mNumCollisions);

		if (frictionEnabled)
		{
			T4f frictionImpulse[3];
			calculateFrictionImpulse(accum.mDeltaX, accum.mDeltaY, accum.mDeltaZ, accum.mVelX, accum.mVelY, accum.mVelZ,
			                         curPos, prevPos, invNumCollisions, frictionScale, mask, frictionImpulse);

			prevPos[0] = prevPos[0] - frictionImpulse[0];
			prevPos[1] = prevPos[1] - frictionImpulse[1];
			prevPos[2] = prevPos[2] - frictionImpulse[2];

			transpose(prevPos[0], prevPos[1], prevPos[2], prevPos[3]);
			storeAligned(prevIt, 0, prevPos[0]);
			storeAligned(prevIt, 16, prevPos[1]);
			storeAligned(prevIt, 32, prevPos[2]);
			storeAligned(prevIt, 48, prevPos[3]);
		}

		if (massScalingEnabled)
		{
			// calculate the inverse mass scale based on the collision impulse magnitude
			T4f dSq = invNumCollisions * invNumCollisions *
			             (accum.mDeltaX * accum.mDeltaX + accum.mDeltaY * accum.mDeltaY + accum.mDeltaZ * accum.mDeltaZ);

			T4f weightScale = recip(gSimd4fOne + massScale * dSq);

			// scale invmass
			curPos[3] = select(mask, curPos[3] * weightScale, curPos[3]);
		}

		curPos[0] = curPos[0] + accum.mDeltaX * invNumCollisions;
		curPos[1] = curPos[1] + accum.mDeltaY * invNumCollisions;
		curPos[2] = curPos[2] + accum.mDeltaZ * invNumCollisions;

		transpose(curPos[0], curPos[1], curPos[2], curPos[3]);
		storeAligned(curIt, 0, curPos[0]);
		storeAligned(curIt, 16, curPos[1]);
		storeAligned(curIt, 32, curPos[2]);
		storeAligned(curIt, 48, curPos[3]);

#if PX_PROFILE || PX_DEBUG
		mNumCollisions += horizontalSum(accum.mNumCollisions);
#endif
	}
}

template <typename T4f>
void cloth::SwCollision<T4f>::collideConvexes(const IterationState<T4f>& state)
{
	if (!mClothData.mNumConvexes)
		return;

	// times 2 for plane equation result buffer
	T4f* planes = static_cast<T4f*>(mAllocator.allocate(sizeof(T4f) * mClothData.mNumPlanes * 2));

	const T4f* targetPlanes = reinterpret_cast<const T4f*>(mClothData.mTargetCollisionPlanes);

	// generate plane collision data
	if (state.mRemainingIterations != 1)
	{
		// interpolate planes
		LerpIterator<T4f, const T4f*> planeIter(reinterpret_cast<const T4f*>(mClothData.mStartCollisionPlanes),
		                                              targetPlanes, state.getCurrentAlpha());

		// todo: normalize plane equations
		generatePlanes(planes, planeIter, mClothData.mNumPlanes);
	}
	else
	{
		// otherwise use the target planes directly
		generatePlanes(planes, targetPlanes, mClothData.mNumPlanes);
	}

	T4f curPos[4], prevPos[4];

	const bool frictionEnabled = mClothData.mFrictionScale > 0.0f;
	const T4f frictionScale = simd4f(mClothData.mFrictionScale);

	float* __restrict curIt = mClothData.mCurParticles;
	float* __restrict curEnd = curIt + mClothData.mNumParticles * 4;
	float* __restrict prevIt = mClothData.mPrevParticles;
	for (; curIt < curEnd; curIt += 16, prevIt += 16)
	{
		curPos[0] = loadAligned(curIt, 0);
		curPos[1] = loadAligned(curIt, 16);
		curPos[2] = loadAligned(curIt, 32);
		curPos[3] = loadAligned(curIt, 48);
		transpose(curPos[0], curPos[1], curPos[2], curPos[3]);

		ImpulseAccumulator accum;
		collideConvexes(planes, curPos, accum);

		T4f mask;
		if (!anyGreater(accum.mNumCollisions, gSimd4fEpsilon, mask))
			continue;

		T4f invNumCollisions = recip(accum.mNumCollisions);

		if (frictionEnabled)
		{
			prevPos[0] = loadAligned(prevIt, 0);
			prevPos[1] = loadAligned(prevIt, 16);
			prevPos[2] = loadAligned(prevIt, 32);
			prevPos[3] = loadAligned(prevIt, 48);
			transpose(prevPos[0], prevPos[1], prevPos[2], prevPos[3]);

			T4f frictionImpulse[3];
			calculateFrictionImpulse(accum.mDeltaX, accum.mDeltaY, accum.mDeltaZ, accum.mVelX, accum.mVelY, accum.mVelZ,
			                         curPos, prevPos, invNumCollisions, frictionScale, mask, frictionImpulse);

			prevPos[0] = prevPos[0] - frictionImpulse[0];
			prevPos[1] = prevPos[1] - frictionImpulse[1];
			prevPos[2] = prevPos[2] - frictionImpulse[2];

			transpose(prevPos[0], prevPos[1], prevPos[2], prevPos[3]);
			storeAligned(prevIt, 0, prevPos[0]);
			storeAligned(prevIt, 16, prevPos[1]);
			storeAligned(prevIt, 32, prevPos[2]);
			storeAligned(prevIt, 48, prevPos[3]);
		}

		curPos[0] = curPos[0] + accum.mDeltaX * invNumCollisions;
		curPos[1] = curPos[1] + accum.mDeltaY * invNumCollisions;
		curPos[2] = curPos[2] + accum.mDeltaZ * invNumCollisions;

		transpose(curPos[0], curPos[1], curPos[2], curPos[3]);
		storeAligned(curIt, 0, curPos[0]);
		storeAligned(curIt, 16, curPos[1]);
		storeAligned(curIt, 32, curPos[2]);
		storeAligned(curIt, 48, curPos[3]);

#if PX_PROFILE || PX_DEBUG
		mNumCollisions += horizontalSum(accum.mNumCollisions);
#endif
	}

	mAllocator.deallocate(planes);
}

template <typename T4f>
void cloth::SwCollision<T4f>::collideConvexes(const T4f* __restrict planes, T4f* __restrict curPos,
                                                 ImpulseAccumulator& accum)
{
	T4i result = gSimd4iZero;
	T4i mask4 = gSimd4iOne;

	const T4f* __restrict pIt, *pEnd = planes + mClothData.mNumPlanes;
	T4f* __restrict dIt = const_cast<T4f*>(pEnd);
	for (pIt = planes; pIt != pEnd; ++pIt, ++dIt)
	{
		*dIt = splat<3>(*pIt) + curPos[2] * splat<2>(*pIt) + curPos[1] * splat<1>(*pIt) + curPos[0] * splat<0>(*pIt);
		result = result | (mask4 & simd4i(*dIt < gSimd4fZero));
		mask4 = mask4 << 1; // todo: shift by T4i on consoles
	}

	if (allEqual(result, gSimd4iZero))
		return;

	const uint32_t* __restrict cIt = mClothData.mConvexMasks;
	const uint32_t* __restrict cEnd = cIt + mClothData.mNumConvexes;
	for (; cIt != cEnd; ++cIt)
	{
		uint32_t mask = *cIt;
		mask4 = simd4i(int(mask));
		if (!anyEqual(mask4 & result, mask4, mask4))
			continue;

		uint32_t test = mask - 1;
		uint32_t planeIndex = findBitSet(mask & ~test);
		T4f plane = planes[planeIndex];
		T4f planeX = splat<0>(plane);
		T4f planeY = splat<1>(plane);
		T4f planeZ = splat<2>(plane);
		T4f planeD = pEnd[planeIndex];
		while (mask &= test)
		{
			test = mask - 1;
			planeIndex = findBitSet(mask & ~test);
			plane = planes[planeIndex];
			T4f dist = pEnd[planeIndex];
			T4f closer = dist > planeD;
			planeX = select(closer, splat<0>(plane), planeX);
			planeY = select(closer, splat<1>(plane), planeY);
			planeZ = select(closer, splat<2>(plane), planeZ);
			planeD = max(dist, planeD);
		}

		accum.subtract(planeX, planeY, planeZ, planeD, simd4f(mask4));
	}
}

template <typename T4f>
void cloth::SwCollision<T4f>::collideTriangles(const IterationState<T4f>& state)
{
	if (!mClothData.mNumCollisionTriangles)
		return;

	TriangleData* triangles =
	    static_cast<TriangleData*>(mAllocator.allocate(sizeof(TriangleData) * mClothData.mNumCollisionTriangles));

	UnalignedIterator<T4f, 3> targetTriangles(mClothData.mTargetCollisionTriangles);

	// generate triangle collision data
	if (state.mRemainingIterations != 1)
	{
		// interpolate triangles
		LerpIterator<T4f, UnalignedIterator<T4f, 3> > triangleIter(mClothData.mStartCollisionTriangles,
		                                                                 targetTriangles, state.getCurrentAlpha());

		generateTriangles<T4f>(triangles, triangleIter, mClothData.mNumCollisionTriangles);
	}
	else
	{
		// otherwise use the target triangles directly
		generateTriangles<T4f>(triangles, targetTriangles, mClothData.mNumCollisionTriangles);
	}

	T4f positions[4];

	float* __restrict pIt = mClothData.mCurParticles;
	float* __restrict pEnd = pIt + mClothData.mNumParticles * 4;
	for (; pIt < pEnd; pIt += 16)
	{
		positions[0] = loadAligned(pIt, 0);
		positions[1] = loadAligned(pIt, 16);
		positions[2] = loadAligned(pIt, 32);
		positions[3] = loadAligned(pIt, 48);
		transpose(positions[0], positions[1], positions[2], positions[3]);

		ImpulseAccumulator accum;
		collideTriangles(triangles, positions, accum);

		T4f mask;
		if (!anyGreater(accum.mNumCollisions, gSimd4fEpsilon, mask))
			continue;

		T4f invNumCollisions = recip(accum.mNumCollisions);

		positions[0] = positions[0] + accum.mDeltaX * invNumCollisions;
		positions[1] = positions[1] + accum.mDeltaY * invNumCollisions;
		positions[2] = positions[2] + accum.mDeltaZ * invNumCollisions;

		transpose(positions[0], positions[1], positions[2], positions[3]);
		storeAligned(pIt, 0, positions[0]);
		storeAligned(pIt, 16, positions[1]);
		storeAligned(pIt, 32, positions[2]);
		storeAligned(pIt, 48, positions[3]);

#if PX_PROFILE || PX_DEBUG
		mNumCollisions += horizontalSum(accum.mNumCollisions);
#endif
	}

	mAllocator.deallocate(triangles);
}

template <typename T4f>
void cloth::SwCollision<T4f>::collideTriangles(const TriangleData* __restrict triangles, T4f* __restrict curPos,
                                                  ImpulseAccumulator& accum)
{
	T4f normalX, normalY, normalZ, normalD;
	normalX = normalY = normalZ = normalD = gSimd4fZero;
	T4f minSqrLength = gSimd4fFloatMax;

	const TriangleData* __restrict tIt, *tEnd = triangles + mClothData.mNumCollisionTriangles;
	for (tIt = triangles; tIt != tEnd; ++tIt)
	{
		T4f base = loadAligned(&tIt->base.x);
		T4f edge0 = loadAligned(&tIt->edge0.x);
		T4f edge1 = loadAligned(&tIt->edge1.x);
		T4f normal = loadAligned(&tIt->normal.x);
		T4f aux = loadAligned(&tIt->det);

		T4f dx = curPos[0] - splat<0>(base);
		T4f dy = curPos[1] - splat<1>(base);
		T4f dz = curPos[2] - splat<2>(base);

		T4f e0x = splat<0>(edge0);
		T4f e0y = splat<1>(edge0);
		T4f e0z = splat<2>(edge0);

		T4f e1x = splat<0>(edge1);
		T4f e1y = splat<1>(edge1);
		T4f e1z = splat<2>(edge1);

		T4f nx = splat<0>(normal);
		T4f ny = splat<1>(normal);
		T4f nz = splat<2>(normal);

		T4f deltaDotEdge0 = dx * e0x + dy * e0y + dz * e0z;
		T4f deltaDotEdge1 = dx * e1x + dy * e1y + dz * e1z;
		T4f deltaDotNormal = dx * nx + dy * ny + dz * nz;

		T4f edge0DotEdge1 = splat<3>(base);
		T4f edge0SqrLength = splat<3>(edge0);
		T4f edge1SqrLength = splat<3>(edge1);

		T4f s = edge1SqrLength * deltaDotEdge0 - edge0DotEdge1 * deltaDotEdge1;
		T4f t = edge0SqrLength * deltaDotEdge1 - edge0DotEdge1 * deltaDotEdge0;

		T4f sPositive = s > gSimd4fZero;
		T4f tPositive = t > gSimd4fZero;

		T4f det = splat<0>(aux);

		s = select(tPositive, s * det, deltaDotEdge0 * splat<2>(aux));
		t = select(sPositive, t * det, deltaDotEdge1 * splat<3>(aux));

		T4f clamp = gSimd4fOne < s + t;
		T4f numerator = edge1SqrLength - edge0DotEdge1 + deltaDotEdge0 - deltaDotEdge1;

		s = select(clamp, numerator * splat<1>(aux), s);

		s = max(gSimd4fZero, min(gSimd4fOne, s));
		t = max(gSimd4fZero, min(gSimd4fOne - s, t));

		dx = dx - e0x * s - e1x * t;
		dy = dy - e0y * s - e1y * t;
		dz = dz - e0z * s - e1z * t;

		T4f sqrLength = dx * dx + dy * dy + dz * dz;

		// slightly increase distance for colliding triangles
		T4f slack = (gSimd4fZero > deltaDotNormal) & simd4f(1e-4f);
		sqrLength = sqrLength + sqrLength * slack;

		T4f mask = sqrLength < minSqrLength;

		normalX = select(mask, nx, normalX);
		normalY = select(mask, ny, normalY);
		normalZ = select(mask, nz, normalZ);
		normalD = select(mask, deltaDotNormal, normalD);

		minSqrLength = min(sqrLength, minSqrLength);
	}

	T4f mask;
	if (!anyGreater(gSimd4fZero, normalD, mask))
		return;

	accum.subtract(normalX, normalY, normalZ, normalD, mask);
}

// explicit template instantiation
#if NV_SIMD_SIMD
template class cloth::SwCollision<Simd4f>;
#endif
#if NV_SIMD_SCALAR
template class cloth::SwCollision<Scalar4f>;
#endif

/*
namespace
{
    using namespace cloth;

    int test()
    {
        Simd4f vertices[] = {
            simd4f(0.0f, 0.0f, 0.0f, 0.0f),
            simd4f(0.1f, 0.0f, 0.0f, 0.0f),
            simd4f(0.0f, 0.1f, 0.0f, 0.0f)
        };
        TriangleData triangle;
        generateTriangles<Simd4f>(&triangle, &*vertices, 1);

        char buffer[1000];
        SwKernelAllocator alloc(buffer, 1000);

        SwClothData* cloth = static_cast<SwClothData*>(malloc(sizeof(SwClothData)));
        memset(cloth, 0, sizeof(SwClothData));
        cloth->mNumTriangles = 1;

        SwCollision<Simd4f> collision(*cloth, alloc);
        SwCollision<Simd4f>::ImpulseAccumulator accum;

        Simd4f particles[4] = {};
        for (float y = -0.1f; y < 0.0f; y += 0.2f)
        {
            for (float x = -0.1f; x < 0.0f; x += 0.2f)
            {
                particles[0] = simd4f(x);
                particles[1] = simd4f(y);
                particles[2] = simd4f(-1.0f);

                collision.collideTriangles(&triangle, particles, accum);
            }
        }

        return 0;
    }

    static int blah = test();
}
*/
