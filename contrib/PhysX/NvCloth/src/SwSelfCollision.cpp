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

#include "SwSelfCollision.h"
#include "SwCloth.h"
#include "SwClothData.h"
#include "SwCollisionHelpers.h"

#ifdef _MSC_VER 
#pragma warning(disable : 4127) // conditional expression is constant
#endif

using namespace nv;
using namespace cloth;

namespace
{

// returns sorted indices, output needs to be at least 2*(last - first) + 1024
void radixSort(const uint32_t* first, const uint32_t* last, uint16_t* out)
{
	// Note: This function is almost exactly duplicated in SwInterCollision.cpp
	// this sort uses a radix (bin) size of 256, requiring 4 bins to sort the 32 bit keys
	uint16_t n = uint16_t(last - first);

	uint16_t* buffer = out + 2 * n;
	uint16_t* __restrict histograms[] = { buffer, buffer + 256, buffer + 512, buffer + 768 };

	//zero the buffer memory used for the 4 buckets
	memset(buffer, 0, 1024 * sizeof(uint16_t));

	// build 4 histograms in one pass
	for (const uint32_t* __restrict it = first; it != last; ++it)
	{
		uint32_t key = *it;
		++histograms[0][0xff & key];
		++histograms[1][0xff & (key >> 8)];
		++histograms[2][0xff & (key >> 16)];
		++histograms[3][key >> 24];
	}

	// convert histograms to offset tables in-place
	uint16_t sums[4] = {0, 0, 0, 0};
	for (uint32_t i = 0; i < 256; ++i)
	{
		uint16_t temp0 = uint16_t(histograms[0][i] + sums[0]);
		histograms[0][i] = sums[0]; sums[0] = temp0;

		uint16_t temp1 = uint16_t(histograms[1][i] + sums[1]);
		histograms[1][i] = sums[1]; sums[1] = temp1;

		uint16_t temp2 = uint16_t(histograms[2][i] + sums[2]);
		histograms[2][i] = sums[2]; sums[2] = temp2;

		uint16_t temp3 = uint16_t(histograms[3][i] + sums[3]);
		histograms[3][i] = sums[3]; sums[3] = temp3;
	}

	NV_CLOTH_ASSERT(sums[0] == n && sums[1] == n && sums[2] == n && sums[3] == n);

#if PX_DEBUG
	memset(out, 0xff, 2 * n * sizeof(uint16_t));
#endif

	// sort 8 bits per pass

	uint16_t* __restrict indices[] = { out, out + n };

	for (uint16_t i = 0; i != n; ++i)
		indices[1][histograms[0][0xff & first[i]]++] = i;

	for (uint16_t i = 0, index; i != n; ++i)
	{
		index = indices[1][i];
		indices[0][histograms[1][0xff & (first[index] >> 8)]++] = index;
	}

	for (uint16_t i = 0, index; i != n; ++i)
	{
		index = indices[0][i];
		indices[1][histograms[2][0xff & (first[index] >> 16)]++] = index;
	
	}
	for (uint16_t i = 0, index; i != n; ++i)
	{
		index = indices[1][i];
		indices[0][histograms[3][first[index] >> 24]++] = index;
	}
}

template <typename T4f>
uint32_t longestAxis(const T4f& edgeLength)
{
	const float* e = array(edgeLength);

	if (e[0] > e[1])
		return uint32_t(e[0] > e[2] ? 0 : 2);
	else
		return uint32_t(e[1] > e[2] ? 1 : 2);
}

bool isSelfCollisionEnabled(const cloth::SwClothData& cloth)
{
	return std::min(cloth.mSelfCollisionDistance, cloth.mSelfCollisionStiffness) > 0.0f;
}

bool isSelfCollisionEnabled(const cloth::SwCloth& cloth)
{
	return std::min(cloth.mSelfCollisionDistance, -cloth.mSelfCollisionLogStiffness) > 0.0f;
}

// align x to a 2 byte boundary
inline uint32_t align2(uint32_t x)
{
	return (x + 1) & ~1;
}

} // anonymous namespace

template <typename T4f>
cloth::SwSelfCollision<T4f>::SwSelfCollision(cloth::SwClothData& clothData, cloth::SwKernelAllocator& alloc)
: mClothData(clothData), mAllocator(alloc)
{
	mCollisionDistance = simd4f(mClothData.mSelfCollisionDistance);
	mCollisionSquareDistance = mCollisionDistance * mCollisionDistance;
	mStiffness = gSimd4fMaskXYZ & static_cast<T4f>(simd4f(mClothData.mSelfCollisionStiffness));
}

template <typename T4f>
cloth::SwSelfCollision<T4f>::~SwSelfCollision()
{
}

template <typename T4f>
void cloth::SwSelfCollision<T4f>::operator()()
{
	mNumTests = mNumCollisions = 0;

	if (!isSelfCollisionEnabled(mClothData))
		return;

	T4f lowerBound = load(mClothData.mCurBounds);
	T4f edgeLength = max(load(mClothData.mCurBounds + 3) - lowerBound, gSimd4fEpsilon);

	// sweep along longest axis
	uint32_t sweepAxis = longestAxis(edgeLength);
	uint32_t hashAxis0 = (sweepAxis + 1) % 3;
	uint32_t hashAxis1 = (sweepAxis + 2) % 3;

	// reserve 0, 255, and 65535 for sentinel
	T4f cellSize = max(mCollisionDistance, simd4f(1.0f / 253) * edgeLength);
	array(cellSize)[sweepAxis] = array(edgeLength)[sweepAxis] / 65533;

	T4f one = gSimd4fOne;
	// +1 for sentinel 0 offset
	T4f gridSize = simd4f(254.0f);
	array(gridSize)[sweepAxis] = 65534.0f;

	T4f gridScale = recip<1>(cellSize);
	T4f gridBias = -lowerBound * gridScale + one;

	uint32_t numIndices = mClothData.mNumSelfCollisionIndices;
	void* buffer = mAllocator.allocate(getBufferSize(numIndices));

	const uint32_t* __restrict indices = mClothData.mSelfCollisionIndices;
	uint32_t* __restrict keys = reinterpret_cast<uint32_t*>(buffer);
	uint16_t* __restrict sortedIndices = reinterpret_cast<uint16_t*>(keys + numIndices);
	uint32_t* __restrict sortedKeys = reinterpret_cast<uint32_t*>(sortedIndices + align2(numIndices));

	const T4f* particles = reinterpret_cast<const T4f*>(mClothData.mCurParticles);

	// create keys
	for (uint32_t i = 0; i < numIndices; ++i)
	{
		// use all particles when no self collision indices are set
		uint32_t index = indices ? indices[i] : i;

		// grid coordinate
		T4f keyf = particles[index] * gridScale + gridBias;

		// need to clamp index because shape collision potentially
		// pushes particles outside of their original bounds
		Simd4i keyi = intFloor(max(one, min(keyf, gridSize)));

		const int32_t* ptr = array(keyi);
		keys[i] = uint32_t(ptr[sweepAxis] | (ptr[hashAxis0] << 16) | (ptr[hashAxis1] << 24));
	}

	// compute sorted key indices
	radixSort(keys, keys + numIndices, sortedIndices);

	// snoop histogram: offset of first index with 8 msb > 1 (0 is sentinel)
	// sortedIndices[2 * numIndices + 768 + 1] is actually histograms[3]+1 from radixSort
	uint16_t firstColumnSize = sortedIndices[2 * numIndices + 768 + 1];

	// sort keys using the sortedIndices
	for (uint32_t i = 0; i < numIndices; ++i)
		sortedKeys[i] = keys[sortedIndices[i]];
	sortedKeys[numIndices] = uint32_t(-1); // sentinel

	// do user provided index array indirection here if we have one
	//  so we don't need to keep branching for this condition later
	if (indices)
	{
		// sort indices (into no-longer-needed keys array)
		// the keys array is no longer used so we can reuse it to store indices[sortedIndices[i]]
		const uint16_t* __restrict oldSortedIndices = sortedIndices;
		sortedIndices = reinterpret_cast<uint16_t*>(keys);
		for (uint32_t i = 0; i < numIndices; ++i)
			sortedIndices[i] = uint16_t(indices[oldSortedIndices[i]]);
	}

	// calculate the number of buckets we need to search forward
	const Simd4i data = intFloor(gridScale * mCollisionDistance); //equal to or larger than floor(mCollisionDistance)
	uint32_t collisionDistance = 2 + static_cast<uint32_t>(array(data)[sweepAxis]);

	// collide particles
	if (mClothData.mRestPositions)
		collideParticles<true>(sortedKeys, firstColumnSize, sortedIndices, collisionDistance);
	else
		collideParticles<false>(sortedKeys, firstColumnSize, sortedIndices, collisionDistance);

	mAllocator.deallocate(buffer);

	// verify against brute force (disable collision response when testing)
	/*
	uint32_t numCollisions = mNumCollisions;
	mNumCollisions = 0;

	T4f* qarticles = reinterpret_cast<
	    T4f*>(mClothData.mCurParticles);
	for (uint32_t i = 0; i < numIndices; ++i)
	{
	    uint32_t indexI = indices ? indices[i] : i;
	    for (uint32_t j = i + 1; j < numIndices; ++j)
	    {
	        uint32_t indexJ = indices ? indices[j] : j;
	        collideParticles(qarticles[indexI], qarticles[indexJ]);
	    }
	}

	static uint32_t iter = 0; ++iter;
	if (numCollisions != mNumCollisions)
	    printf("%u: %u != %u\n", iter, numCollisions, mNumCollisions);
	*/
}

template <typename T4f>
size_t cloth::SwSelfCollision<T4f>::estimateTemporaryMemory(const SwCloth& cloth)
{
	uint32_t numIndices =
	    uint32_t(cloth.mSelfCollisionIndices.empty() ? cloth.mCurParticles.size() : cloth.mSelfCollisionIndices.size());
	return isSelfCollisionEnabled(cloth) ? getBufferSize(numIndices) : 0;
}

template <typename T4f>
size_t cloth::SwSelfCollision<T4f>::getBufferSize(uint32_t numIndices)
{
	uint32_t keysSize = numIndices * sizeof(uint32_t);
	uint32_t indicesSize = align2(numIndices) * sizeof(uint16_t);
	uint32_t radixSize = (numIndices + 1024) * sizeof(uint16_t);
	return keysSize + indicesSize + std::max(radixSize, keysSize + uint32_t(sizeof(uint32_t)));
}

template <typename T4f>
template <bool useRestParticles>
void cloth::SwSelfCollision<T4f>::collideParticles(T4f& pos0, T4f& pos1, const T4f& pos0rest,
                                                      const T4f& pos1rest)
{
	T4f diff = pos1 - pos0;
	T4f distSqr = dot3(diff, diff);

#if PX_DEBUG
	++mNumTests;
#endif

	if (allGreater(distSqr, mCollisionSquareDistance))
		return;

	if (useRestParticles)
	{
		// calculate distance in rest configuration, if less than collision
		// distance then ignore collision between particles in deformed config
		T4f restDiff = pos1rest - pos0rest;
		T4f restDistSqr = dot3(restDiff, restDiff);

		if (allGreater(mCollisionSquareDistance, restDistSqr))
			return;
	}

	T4f w0 = splat<3>(pos0);
	T4f w1 = splat<3>(pos1);

	T4f ratio = mCollisionDistance * rsqrt(distSqr);
	T4f scale = mStiffness * recip(gSimd4fEpsilon + w0 + w1);
	T4f delta = (scale * (diff - diff * ratio)) & gSimd4fMaskXYZ;

	pos0 = pos0 + delta * w0;
	pos1 = pos1 - delta * w1;

#if PX_DEBUG || PX_PROFILE
	++mNumCollisions;
#endif
}

template <typename T4f>
template <bool useRestParticles>
void cloth::SwSelfCollision<T4f>::collideParticles(const uint32_t* keys, uint16_t firstColumnSize,
                                                      const uint16_t* indices, uint32_t collisionDistance)
{
	//keys is an array of bucket keys for the particles
	//indices is an array of particle indices
	//collisionDistance is the number of buckets along the sweep axis we need to search after the current one

	T4f* __restrict particles = reinterpret_cast<T4f*>(mClothData.mCurParticles);
	T4f* __restrict restParticles =
	    useRestParticles ? reinterpret_cast<T4f*>(mClothData.mRestPositions) : particles;

	//16 lsb's are for the bucket
	const uint32_t bucketMask = 0x0000ffff;

	// offsets for cells (not along the sweep axis)
	//										[1]		[3]-[1]			[3]		  [1]+[3]
	const uint32_t keyOffsets[] = { 0, 0x00010000, 0x00ff0000, 0x01000000, 0x01010000 };

	const uint32_t* __restrict kFirst[5];
	const uint32_t* __restrict kLast[5];

	/*
	We use 5 first/last pairs to search the following cells
	=====================
	|   |   |   |   |   |
	=====================
	|   |   | 0 | 1 |   |
	=====================
	|   | 2 | 3 | 4 |   |
	=====================
	|   |   |   |   |   |
	=====================
	With 0 as the origin.
	This way collisions won't be double reported.
	*/

	{
		// optimization: scan forward iterator starting points once instead of 9 times
		const uint32_t* __restrict kIt = keys;

		uint32_t key = *kIt;
		//clamp first/lastKey to bucket
		uint32_t firstKey = key - std::min(collisionDistance, key & bucketMask);
		uint32_t lastKey = std::min(key + collisionDistance, key | bucketMask);

		//sweep 0
		kFirst[0] = kIt;
		//find next key in keys that is past lastKey
		while (*kIt < lastKey)
			++kIt;
		kLast[0] = kIt;

		//sweep 1...4
		for (uint32_t k = 1; k < 5; ++k)
		{
			// scan forward start point
			for (uint32_t n = firstKey + keyOffsets[k]; *kIt < n;)
				++kIt;
			kFirst[k] = kIt;

			// scan forward end point
			for (uint32_t n = lastKey + keyOffsets[k]; *kIt < n;)
				++kIt;
			kLast[k] = kIt;

			// jump forward once to second column to go from cell offset 1 to 2 quickly
			if(firstColumnSize)
				kIt = keys + firstColumnSize;
			firstColumnSize = 0;
		}
	}

	const uint16_t* __restrict iIt = indices;
	const uint16_t* __restrict iEnd = indices + mClothData.mNumSelfCollisionIndices;

	const uint16_t* __restrict jIt;
	const uint16_t* __restrict jEnd;

	//loop through all indices
	for (; iIt < iEnd; ++iIt, ++kFirst[0])
	{
		NV_CLOTH_ASSERT(*iIt < mClothData.mNumParticles);

		// load current particle once outside of inner loop
		T4f particle = particles[*iIt];
		T4f restParticle = restParticles[*iIt];

		uint32_t key = *kFirst[0];

		// range of keys we need to check against for this particle
		uint32_t firstKey = key - std::min(collisionDistance, key & bucketMask);
		uint32_t lastKey = std::min(key + collisionDistance, key | bucketMask);

		// scan forward end point
		while (*kLast[0] < lastKey)
			++kLast[0];

		// process potential colliders of same cell
		jEnd = indices + (kLast[0] - keys); //calculate index from key pointer
		for (jIt = iIt + 1; jIt < jEnd; ++jIt)
			collideParticles<useRestParticles>(particle, particles[*jIt], restParticle, restParticles[*jIt]);

		// process neighbor cells
		for (uint32_t k = 1; k < 5; ++k)
		{
			// scan forward start point
			for (uint32_t n = firstKey + keyOffsets[k]; *kFirst[k] < n;)
				++kFirst[k];

			// scan forward end point
			for (uint32_t n = lastKey + keyOffsets[k]; *kLast[k] < n;)
				++kLast[k];

			// process potential colliders
			jEnd = indices + (kLast[k] - keys);
			for (jIt = indices + (kFirst[k] - keys); jIt < jEnd; ++jIt)
				collideParticles<useRestParticles>(particle, particles[*jIt], restParticle, restParticles[*jIt]);
		}

		// store current particle
		particles[*iIt] = particle;
	}
}

// explicit template instantiation
#if NV_SIMD_SIMD
template class cloth::SwSelfCollision<Simd4f>;
#endif
#if NV_SIMD_SCALAR
template class cloth::SwSelfCollision<Scalar4f>;
#endif
