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

#include "SwSolverKernel.h"
#include "SwCloth.h"
#include "SwClothData.h"
#include "SwFabric.h"
#include "SwFactory.h"
#include "PointInterpolator.h"
#include "BoundingBox.h"
#include <foundation/PxProfiler.h>

using namespace physx;

#define NV_AVX (NV_SIMD_SIMD&&(PX_WIN32 || PX_WIN64) && PX_VC >= 10)
#ifdef _MSC_VER 
#pragma warning(disable : 4127) // conditional expression is constant
#endif

#if NV_AVX
namespace avx
{
// defined in SwSolveConstraints.cpp

void initialize();

template <bool, uint32_t>
void solveConstraints(float* __restrict posIt, const float* __restrict rIt, const float* __restrict stIt, const float* __restrict rEnd,
                      const uint16_t* __restrict iIt, const __m128& stiffnessEtc, const __m128& stiffnessExponent);
}

namespace
{
uint32_t getAvxSupport()
{
// Checking for AVX requires 3 things:
// 1) CPUID indicates that the OS uses XSAVE and XRSTORE
// 2) CPUID indicates support for AVX
// 3) XGETBV indicates registers are saved and restored on context switch

#if _MSC_FULL_VER < 160040219 || !defined(_XCR_XFEATURE_ENABLED_MASK)
	// need at least VC10 SP1 and compile on at least Win7 SP1
	return 0;
#else
	int cpuInfo[4];
	__cpuid(cpuInfo, 1);
	int avxFlags = 3 << 27; // checking 1) and 2) above
	if ((cpuInfo[2] & avxFlags) != avxFlags)
		return 0; // xgetbv not enabled or no AVX support

	if ((_xgetbv(_XCR_XFEATURE_ENABLED_MASK) & 0x6) != 0x6)
		return 0; // OS does not save YMM registers

	avx::initialize();

#if _MSC_VER < 1700
	return 1;
#else
	int fmaFlags = 1 << 12;
	if ((cpuInfo[2] & fmaFlags) != fmaFlags)
		return 1; // no FMA3 support

	/* only using fma at the moment, don't lock out AMD's piledriver by requiring avx2
	__cpuid(cpuInfo, 7);
	int avx2Flags = 1 << 5;
	if ((cpuInfo[1] & avx2Flags) != avx2Flags)
	    return 1; // no AVX2 support
	*/

	return 2;
#endif // _MSC_VER
#endif // _MSC_FULL_VER
}

const uint32_t sAvxSupport = getAvxSupport(); // 0: no AVX, 1: AVX, 2: AVX+FMA
}
#endif

using namespace nv;
using namespace cloth;

namespace
{
/* simd constants */

const Simd4fTupleFactory sMaskW = simd4f(simd4i(0, 0, 0, ~0));
const Simd4fTupleFactory sMaskXY = simd4f(simd4i(~0, ~0, 0, 0));
const Simd4fTupleFactory sMaskXYZ = simd4f(simd4i(~0, ~0, ~0, 0));
const Simd4fTupleFactory sMaskYZW = simd4f(simd4i(0, ~0, ~0, ~0));
const Simd4fTupleFactory sMinusOneXYZOneW = simd4f(-1.0f, -1.0f, -1.0f, 1.0f);
const Simd4fTupleFactory sFloatMaxW = simd4f(0.0f, 0.0f, 0.0f, FLT_MAX);
const Simd4fTupleFactory sMinusFloatMaxXYZ = simd4f(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0.0f);

/* static worker functions */

/**
   This function performs explicit Euler integration based on position, where
   x_next = x_cur + (x_cur - x_prev) * dt_cur/dt_prev * damping + g * dt * dt
   The g * dt * dt term is folded into accelIt.
 */

template <typename T4f, typename AccelerationIterator>
void integrateParticles(T4f* __restrict curIt, T4f* __restrict curEnd, T4f* __restrict prevIt,
                        const T4f& scale, const AccelerationIterator& aIt, const T4f& prevBias)
{
	// local copy to avoid LHS
	AccelerationIterator accelIt(aIt);

	for (; curIt != curEnd; ++curIt, ++prevIt, ++accelIt)
	{
		T4f current = *curIt;
		T4f previous = *prevIt;
		// if (current.w == 0) current.w = previous.w
		current = select(current > sMinusFloatMaxXYZ, current, previous);
		T4f finiteMass = splat<3>(previous) > sFloatMaxW;
		T4f delta = (current - previous) * scale + *accelIt;
		*curIt = current + (delta & finiteMass);
		*prevIt = select(sMaskW, previous, current) + (prevBias & finiteMass);
	}
}

template <typename T4f, typename AccelerationIterator>
void integrateParticles(T4f* __restrict curIt, T4f* __restrict curEnd, T4f* __restrict prevIt,
                        const T4f (&prevMatrix)[3], const T4f (&curMatrix)[3], const AccelerationIterator& aIt,
                        const T4f& prevBias)
{
	// local copy to avoid LHS
	AccelerationIterator accelIt(aIt);

	for (; curIt != curEnd; ++curIt, ++prevIt, ++accelIt)
	{
		T4f current = *curIt;
		T4f previous = *prevIt;
		// if (current.w == 0) current.w = previous.w
		current = select(current > sMinusFloatMaxXYZ, current, previous);
		T4f finiteMass = splat<3>(previous) > sFloatMaxW;
		// curMatrix * current + prevMatrix * previous + accel
		T4f delta = cloth::transform(curMatrix, cloth::transform(prevMatrix, *accelIt, previous), current);
		*curIt = current + (delta & finiteMass);
		*prevIt = select(sMaskW, previous, current) + (prevBias & finiteMass);
	}
}

template <typename T4f, typename ConstraintIterator>
void constrainMotion(T4f* __restrict curIt, const T4f* __restrict curEnd, const ConstraintIterator& spheres,
                     const T4f& scaleBiasStiffness)
{
	T4f scale = splat<0>(scaleBiasStiffness);
	T4f bias = splat<1>(scaleBiasStiffness);
	T4f stiffness = splat<3>(scaleBiasStiffness);

	// local copy of iterator to maintain alignment
	ConstraintIterator sphIt = spheres;

	for (; curIt < curEnd; curIt += 4)
	{
		// todo: use msub where available
		T4f curPos0 = curIt[0];
		T4f curPos1 = curIt[1];
		T4f curPos2 = curIt[2];
		T4f curPos3 = curIt[3];

		//delta.xyz = sphereCenter - currentPosition
		//delta.w = sphereRadius
		T4f delta0 = *sphIt - (sMaskXYZ & curPos0);
		++sphIt;
		T4f delta1 = *sphIt - (sMaskXYZ & curPos1);
		++sphIt;
		T4f delta2 = *sphIt - (sMaskXYZ & curPos2);
		++sphIt;
		T4f delta3 = *sphIt - (sMaskXYZ & curPos3);
		++sphIt;

		T4f deltaX = delta0, deltaY = delta1, deltaZ = delta2, deltaW = delta3;
		transpose(deltaX, deltaY, deltaZ, deltaW);

		T4f sqrLength = gSimd4fEpsilon + deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;
		T4f radius = max(gSimd4fZero, deltaW * scale + bias);

		T4f slack = gSimd4fOne - radius * rsqrt(sqrLength);

		// if slack <= 0.0f then we don't want to affect particle
		// and can skip if all particles are unaffected
		T4f isPositive;
		if (anyGreater(slack, gSimd4fZero, isPositive))
		{
			// set invMass to zero if radius is zero (xyz will be unchanged)
			// curPos.w = radius > 0 ? curPos.w : 0
			// the first three components are compared against -FLT_MAX which is always true
			curPos0 = curPos0 & (splat<0>(radius) > sMinusFloatMaxXYZ);
			curPos1 = curPos1 & (splat<1>(radius) > sMinusFloatMaxXYZ);
			curPos2 = curPos2 & (splat<2>(radius) > sMinusFloatMaxXYZ);
			curPos3 = curPos3 & ((radius) > sMinusFloatMaxXYZ);
			// we don't have to splat the last one as the 4th element is already in the right place

			slack = slack * stiffness & isPositive;

			curIt[0] = curPos0 + (delta0 & sMaskXYZ) * splat<0>(slack);
			curIt[1] = curPos1 + (delta1 & sMaskXYZ) * splat<1>(slack);
			curIt[2] = curPos2 + (delta2 & sMaskXYZ) * splat<2>(slack);
			curIt[3] = curPos3 + (delta3 & sMaskXYZ) * splat<3>(slack);
		}
	}
}

template <typename T4f, typename ConstraintIterator>
void constrainSeparation(T4f* __restrict curIt, const T4f* __restrict curEnd, const ConstraintIterator& spheres)
{
	// local copy of iterator to maintain alignment
	ConstraintIterator sphIt = spheres;

	for (; curIt < curEnd; curIt += 4)
	{
		// todo: use msub where available
		T4f curPos0 = curIt[0];
		T4f curPos1 = curIt[1];
		T4f curPos2 = curIt[2];
		T4f curPos3 = curIt[3];

		//delta.xyz = sphereCenter - currentPosition
		//delta.w = sphereRadius
		T4f delta0 = *sphIt - (sMaskXYZ & curPos0);
		++sphIt;
		T4f delta1 = *sphIt - (sMaskXYZ & curPos1);
		++sphIt;
		T4f delta2 = *sphIt - (sMaskXYZ & curPos2);
		++sphIt;
		T4f delta3 = *sphIt - (sMaskXYZ & curPos3);
		++sphIt;

		T4f deltaX = delta0, deltaY = delta1, deltaZ = delta2, deltaW = delta3;
		transpose(deltaX, deltaY, deltaZ, deltaW);

		T4f sqrLength = gSimd4fEpsilon + deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;

		T4f slack = gSimd4fOne - deltaW * rsqrt<1>(sqrLength);

		// if slack >= 0.0f then we don't want to affect particle
		// and can skip if all particles are unaffected
		T4f isNegative;
		if (anyGreater(gSimd4fZero, slack, isNegative))
		{
			slack = slack & isNegative;

			curIt[0] = curPos0 + (delta0 & sMaskXYZ) * splat<0>(slack);
			curIt[1] = curPos1 + (delta1 & sMaskXYZ) * splat<1>(slack);
			curIt[2] = curPos2 + (delta2 & sMaskXYZ) * splat<2>(slack);
			curIt[3] = curPos3 + (delta3 & sMaskXYZ) * splat<3>(slack);
		}
	}
}



/**
    traditional gauss-seidel internal constraint solver
 */
template <bool useMultiplier, typename T4f>
void solveConstraints(float* __restrict posIt, const float* __restrict rIt, const float* __restrict stIt, const float* __restrict rEnd,
                      const uint16_t* __restrict iIt, const T4f& stiffnessEtc, const T4f& stiffnessExponent)
{
	//posIt		particle position (and invMass) iterator
	//rIt,rEnd	edge rest length iterator
	//iIt		index set iterator

	T4f stretchLimit, compressionLimit, multiplier;
	if (useMultiplier)
	{
		stretchLimit = splat<3>(stiffnessEtc);
		compressionLimit = splat<2>(stiffnessEtc);
		multiplier = splat<1>(stiffnessEtc);
	}
	T4f stiffness = splat<0>(stiffnessEtc);
	bool useStiffnessPerConstraint = stIt!=nullptr;

	for (; rIt != rEnd; rIt += 4, stIt += 4, iIt += 8)
	{
		//Calculate particle indices
		uint32_t p0i = iIt[0] * sizeof(PxVec4);
		uint32_t p0j = iIt[1] * sizeof(PxVec4);
		uint32_t p1i = iIt[2] * sizeof(PxVec4);
		uint32_t p1j = iIt[3] * sizeof(PxVec4);
		uint32_t p2i = iIt[4] * sizeof(PxVec4);
		uint32_t p2j = iIt[5] * sizeof(PxVec4);
		uint32_t p3i = iIt[6] * sizeof(PxVec4);
		uint32_t p3j = iIt[7] * sizeof(PxVec4);

		//Load particle positions
		//v.w = invMass
		T4f v0i = loadAligned(posIt, p0i);
		T4f v0j = loadAligned(posIt, p0j);
		T4f v1i = loadAligned(posIt, p1i);
		T4f v1j = loadAligned(posIt, p1j);
		T4f v2i = loadAligned(posIt, p2i);
		T4f v2j = loadAligned(posIt, p2j);
		T4f v3i = loadAligned(posIt, p3i);
		T4f v3j = loadAligned(posIt, p3j);

		//offset.xyz = posB - posA
		//offset.w = invMassB + invMassA
		T4f h0ij = v0j + v0i * sMinusOneXYZOneW;
		T4f h1ij = v1j + v1i * sMinusOneXYZOneW;
		T4f h2ij = v2j + v2i * sMinusOneXYZOneW;
		T4f h3ij = v3j + v3i * sMinusOneXYZOneW;

		//h xyz = offset
		//vw = invMass sum
		T4f hxij = h0ij, hyij = h1ij, hzij = h2ij, vwij = h3ij;
		transpose(hxij, hyij, hzij, vwij);

		//load rest lengths
		T4f rij = loadAligned(rIt);

		//Load/calculate the constraint stiffness
		T4f stij = useStiffnessPerConstraint ? gSimd4fOne - exp2(stiffnessExponent * static_cast<T4f>(loadAligned(stIt))) : stiffness;

		//squared distance between particles: e2 = epsilon + |h|^2
		T4f e2ij = gSimd4fEpsilon + hxij * hxij + hyij * hyij + hzij * hzij;

		//slack: er = 1 - r / sqrt(e2)
		//       or er = 0 if rest length < epsilon
		T4f erij = (gSimd4fOne - rij * rsqrt(e2ij)) & (rij > gSimd4fEpsilon);

		if (useMultiplier)
		{
			erij = erij - multiplier * max(compressionLimit, min(erij, stretchLimit));
		}

		//ex = er * stiffness / (epsilon + inMass sum)
		T4f exij = erij * stij * recip(gSimd4fEpsilon + vwij);

		//h = h * ex
		h0ij = h0ij * splat<0>(exij) & sMaskXYZ;
		h1ij = h1ij * splat<1>(exij) & sMaskXYZ;
		h2ij = h2ij * splat<2>(exij) & sMaskXYZ;
		h3ij = h3ij * splat<3>(exij) & sMaskXYZ;

		//pos = pos + h * invMass
		storeAligned(posIt, p0i, v0i + h0ij * splat<3>(v0i));
		storeAligned(posIt, p0j, v0j - h0ij * splat<3>(v0j));
		storeAligned(posIt, p1i, v1i + h1ij * splat<3>(v1i));
		storeAligned(posIt, p1j, v1j - h1ij * splat<3>(v1j));
		storeAligned(posIt, p2i, v2i + h2ij * splat<3>(v2i));
		storeAligned(posIt, p2j, v2j - h2ij * splat<3>(v2j));
		storeAligned(posIt, p3i, v3i + h3ij * splat<3>(v3i));
		storeAligned(posIt, p3j, v3j - h3ij * splat<3>(v3j));
	}
}

#if PX_WINDOWS_FAMILY && NV_SIMD_SSE2
#include "sse2/SwSolveConstraints.h"
#endif

// Calculates upper bound of all position deltas
template <typename T4f>
T4f calculateMaxDelta(const T4f* prevIt, const T4f* curIt, const T4f* curEnd)
{
	T4f maxDelta = gSimd4fZero;
	for (; curIt < curEnd; ++curIt, ++prevIt)
		maxDelta = max(maxDelta, abs(*curIt - *prevIt));

	return maxDelta & sMaskXYZ;
}

template <bool IsTurning, typename T4f>
void applyWind(T4f* __restrict curIt, const T4f* __restrict prevIt, const uint16_t* __restrict tIt,
               const uint16_t* __restrict tEnd, float itrDtf, float dragCoefficientf, float liftCoefficientf, float fluidDensityf, T4f wind,
               const T4f (&rotation)[3])
{
	// Note: Enabling wind can amplify bad behavior since the impulse scales with area,
	//  and the area of triangles increases when constraints are violated.
	// Using the initial triangle area based on the rest length is one possible way to
	//  prevent this, but is expensive (and incorrect for intentionally stretchy cloth).

	const T4f dragCoefficient = simd4f(dragCoefficientf);
	const T4f liftCoefficient = simd4f(liftCoefficientf);
	const T4f fluidDensity = simd4f(fluidDensityf);
	const T4f itrDt = simd4f(itrDtf);
	const T4f oneThird = simd4f(1.0f / 3.0f);

	for (; tIt < tEnd; tIt += 3)
	{
		//Get the triangle vertex indices
		uint16_t i0 = tIt[0];
		uint16_t i1 = tIt[1];
		uint16_t i2 = tIt[2];

		//Get the current particle positions
		T4f c0 = curIt[i0];
		T4f c1 = curIt[i1];
		T4f c2 = curIt[i2];

		//Previous positions
		T4f p0 = prevIt[i0];
		T4f p1 = prevIt[i1];
		T4f p2 = prevIt[i2];

		T4f current = oneThird * (c0 + c1 + c2);
		T4f previous = oneThird * (p0 + p1 + p2);

		//offset of the triangle center, including wind
		T4f delta = current - previous + wind; //wind is also already multiplied by dt in the iteration state so everything is in the same units

		if (IsTurning)
		{
			// add rotation of frame
			//rotation = inverse local space rotation for one iteration
			delta = cloth::transform(rotation, current, delta - current);
		}

		//not normalized
		T4f normal = cross3(c2 - c0, c1 - c0);

		T4f doubleArea = sqrt(dot3(normal, normal));
		normal = normal / doubleArea;

		T4f invSqrScale = dot3(delta, delta);
		T4f isZero = invSqrScale < gSimd4fEpsilon;
		T4f scale = rsqrt(invSqrScale);
		T4f deltaLength = sqrt(invSqrScale);

		//scale 'normalizes' delta, doubleArea normalized normal
		T4f cosTheta = dot3(normal, delta) * scale;
		T4f sinTheta = sqrt(max(gSimd4fZero, gSimd4fOne - cosTheta * cosTheta));

		// orthogonal to delta, in delta-normal plane, same length as delta
		T4f liftDir = cross3(cross3(delta, normal), delta * scale);

		// sin(theta) * cos(theta) = 0.5 * sin(2 * theta)
		T4f lift = liftCoefficient * cosTheta * sinTheta * liftDir * deltaLength / itrDt;
		T4f drag = dragCoefficient * abs(cosTheta) * delta * deltaLength / itrDt; 

		T4f impulse = (drag + lift) * fluidDensity * doubleArea & ~isZero; //fluidDensity compensates for double area

		curIt[i0] = c0 - ((impulse * splat<3>(c0)) & sMaskXYZ);
		curIt[i1] = c1 - ((impulse * splat<3>(c1)) & sMaskXYZ);
		curIt[i2] = c2 - ((impulse * splat<3>(c2)) & sMaskXYZ);
	}
}

} // anonymous namespace

template <typename T4f>
cloth::SwSolverKernel<T4f>::SwSolverKernel(SwCloth const& cloth, SwClothData& clothData,
                                              SwKernelAllocator& allocator, IterationStateFactory& factory)
: mCloth(cloth)
, mClothData(clothData)
, mAllocator(allocator)
, mCollision(clothData, allocator)
, mSelfCollision(clothData, allocator)
, mState(factory.create<T4f>(cloth))
{
	mClothData.verify();
}

template <typename T4f>
void cloth::SwSolverKernel<T4f>::operator()()
{
	simulateCloth();
}

template <typename T4f>
size_t cloth::SwSolverKernel<T4f>::estimateTemporaryMemory(const SwCloth& cloth)
{
	size_t collisionTempMemory = SwCollision<T4f>::estimateTemporaryMemory(cloth);
	size_t selfCollisionTempMemory = SwSelfCollision<T4f>::estimateTemporaryMemory(cloth);

	size_t tempMemory = std::max(collisionTempMemory, selfCollisionTempMemory);
	size_t persistentMemory = SwCollision<T4f>::estimatePersistentMemory(cloth);

	// account for any allocator overhead (this could be exposed in the allocator)
	size_t maxAllocs = 32;
	size_t maxPerAllocationOverhead = 32;
	size_t maxAllocatorOverhead = maxAllocs * maxPerAllocationOverhead;

	return maxAllocatorOverhead + persistentMemory + tempMemory;
}

template <typename T4f>
template <typename AccelerationIterator>
void cloth::SwSolverKernel<T4f>::integrateParticles(AccelerationIterator& accelIt, const T4f& prevBias)
{
	T4f* curIt = reinterpret_cast<T4f*>(mClothData.mCurParticles);
	T4f* curEnd = curIt + mClothData.mNumParticles;
	T4f* prevIt = reinterpret_cast<T4f*>(mClothData.mPrevParticles);

	if (!mState.mIsTurning)
	{
		//We use mPrevMatrix to store the scale if we are not rotating
		::integrateParticles(curIt, curEnd, prevIt, mState.mPrevMatrix[0], accelIt, prevBias);
	}
	else
	{
		::integrateParticles(curIt, curEnd, prevIt, mState.mPrevMatrix, mState.mCurMatrix, accelIt, prevBias);
	}
}

template <typename T4f>
void cloth::SwSolverKernel<T4f>::integrateParticles()
{
	NV_CLOTH_PROFILE_ZONE("cloth::SwSolverKernel::integrateParticles", /*ProfileContext::None*/ 0);

	const T4f* startAccelIt = reinterpret_cast<const T4f*>(mClothData.mParticleAccelerations);

	// dt^2 (todo: should this be the smoothed dt used for gravity?)
	const T4f sqrIterDt = simd4f(sqr(mState.mIterDt)) & static_cast<T4f>(sMaskXYZ);

	if (!startAccelIt)
	{
		// no per-particle accelerations, use a constant
		ConstantIterator<T4f> accelIt(mState.mCurBias);
		integrateParticles(accelIt, mState.mPrevBias);
	}
	else
	{
		// iterator implicitly scales by dt^2 and adds gravity
		ScaleBiasIterator<T4f, const T4f*> accelIt(startAccelIt, sqrIterDt, mState.mCurBias);
		integrateParticles(accelIt, mState.mPrevBias);
	}
}

template <typename T4f>
void cloth::SwSolverKernel<T4f>::constrainTether()
{
	if (0.0f == mClothData.mTetherConstraintStiffness || !mClothData.mNumTethers)
		return;

	NV_CLOTH_PROFILE_ZONE("cloth::SwSolverKernel::solveTethers", /*ProfileContext::None*/ 0);

	uint32_t numParticles = mClothData.mNumParticles;
	uint32_t numTethers = mClothData.mNumTethers;
	NV_CLOTH_ASSERT(0 == numTethers % numParticles); // the particles can have multiple tethers, but each particle has the same amount

	//particle iterators
	float* __restrict curIt = mClothData.mCurParticles;
	const float* __restrict curFirst = curIt;
	const float* __restrict curEnd = curIt + 4 * numParticles;

	//Tether iterators
	typedef const SwTether* __restrict TetherIter;
	TetherIter tFirst = mClothData.mTethers;
	TetherIter tEnd = tFirst + numTethers;

	//Tether properties
	T4f stiffness =
	    static_cast<T4f>(sMaskXYZ) & simd4f(numParticles * mClothData.mTetherConstraintStiffness / numTethers);
	T4f scale = simd4f(mClothData.mTetherConstraintScale);

	//Loop through all particles
	for (; curIt != curEnd; curIt += 4, ++tFirst)
	{
		T4f position = loadAligned(curIt); //Get the first particle
		T4f offset = gSimd4fZero; //We accumulate the offset in this variable

		//Loop through all tethers connected to our particle
		for (TetherIter tIt = tFirst; tIt < tEnd; tIt += numParticles)
		{
			NV_CLOTH_ASSERT(tIt->mAnchor < numParticles);

			//Get the particle on the other end of the tether
			T4f anchor = loadAligned(curFirst, tIt->mAnchor * sizeof(PxVec4));
			T4f delta = anchor - position;
			T4f sqrLength = gSimd4fEpsilon + dot3(delta, delta);

			T4f tetherLength = load(&tIt->mLength);
			tetherLength = splat<0>(tetherLength);

			T4f radius = tetherLength * scale;
			T4f slack = gSimd4fOne - radius * rsqrt(sqrLength);

			offset = offset + delta * max(slack, gSimd4fZero);
		}

		storeAligned(curIt, position + offset * stiffness); //Apply accumulated offset
	}
}

template <typename T4f>
void cloth::SwSolverKernel<T4f>::solveFabric()
{
	NV_CLOTH_PROFILE_ZONE("cloth::SwSolverKernel::solveFabric", /*ProfileContext::None*/ 0);

	float* pIt = mClothData.mCurParticles;

	//Phase configuration
	const PhaseConfig* cIt = mClothData.mConfigBegin;
	const PhaseConfig* cEnd = mClothData.mConfigEnd;

	const uint32_t* pBegin = mClothData.mPhases;
	const float* rBegin = mClothData.mRestvalues;
	const float* stBegin = mClothData.mStiffnessValues;

	const uint32_t* sBegin = mClothData.mSets;
	const uint16_t* iBegin = mClothData.mIndices;

	uint32_t totalConstraints = 0;

	T4f stiffnessExponent = simd4f(mCloth.mStiffnessFrequency * mState.mIterDt);

	//Loop through all phase configs
	for (; cIt != cEnd; ++cIt)
	{
		//Get the set for this config
		const uint32_t* sIt = sBegin + pBegin[cIt->mPhaseIndex];

		//Get rest value iterators from set
		const float* rIt = rBegin + sIt[0];
		const float* rEnd = rBegin + sIt[1]; //start of next set is the end of ours
		const float* stIt = stBegin?stBegin + sIt[0]:nullptr;

		//Constraint particle indices
		const uint16_t* iIt = iBegin + sIt[0] * 2; //x2 as we have 2 indices for every rest length

		totalConstraints += uint32_t(rEnd - rIt);

		// (stiffness, multiplier, compressionLimit, stretchLimit)
		T4f config = load(&cIt->mStiffness);
		// stiffness specified as fraction of constraint error per-millisecond
		T4f scaledConfig = gSimd4fOne - exp2(config * stiffnessExponent);
		T4f stiffness = select(sMaskXY, scaledConfig, config);

		int neutralMultiplier = allEqual(sMaskYZW & stiffness, gSimd4fZero);

#if NV_AVX
		switch(sAvxSupport)
		{
		case 2:
#if _MSC_VER >= 1700
			neutralMultiplier ? avx::solveConstraints<false, 2>(pIt, rIt, stIt, rEnd, iIt, stiffness, stiffnessExponent)
			                  : avx::solveConstraints<true, 2>(pIt, rIt, stIt, rEnd, iIt, stiffness, stiffnessExponent);
			break;
#endif
		case 1:
			neutralMultiplier ? avx::solveConstraints<false, 1>(pIt, rIt, stIt, rEnd, iIt, stiffness, stiffnessExponent)
			                  : avx::solveConstraints<true, 1>(pIt, rIt, stIt, rEnd, iIt, stiffness, stiffnessExponent);
			break;
		default:
#endif
			neutralMultiplier ? solveConstraints<false>(pIt, rIt, stIt, rEnd, iIt, stiffness, stiffnessExponent)
			                  : solveConstraints<true>(pIt, rIt, stIt, rEnd, iIt, stiffness, stiffnessExponent);
#if NV_AVX
			break;
		}
#endif
	}
}

template <typename T4f>
void cloth::SwSolverKernel<T4f>::applyWind()
{
	if (mClothData.mDragCoefficient == 0.0f && mClothData.mLiftCoefficient == 0.0f)
		return;

	NV_CLOTH_PROFILE_ZONE("cloth::SwSolverKernel::applyWind", /*ProfileContext::None*/ 0);

	T4f* curIt = reinterpret_cast<T4f*>(mClothData.mCurParticles);
	T4f* prevIt = reinterpret_cast<T4f*>(mClothData.mPrevParticles);

	const uint16_t* tIt = mClothData.mTriangles;
	const uint16_t* tEnd = tIt + 3 * mClothData.mNumTriangles;

	if (mState.mIsTurning)
	{
		::applyWind<true>(curIt, prevIt, tIt, tEnd, mState.mIterDt, mClothData.mDragCoefficient, mClothData.mLiftCoefficient, mClothData.mFluidDensity, mState.mWind,
		                  mState.mRotationMatrix);
	}
	else
	{
		::applyWind<false>(curIt, prevIt, tIt, tEnd, mState.mIterDt, mClothData.mDragCoefficient, mClothData.mLiftCoefficient, mClothData.mFluidDensity, mState.mWind,
		                   mState.mRotationMatrix);
	}
}

template <typename T4f>
void cloth::SwSolverKernel<T4f>::constrainMotion()
{
	if (!mClothData.mStartMotionConstraints)
		return;

	NV_CLOTH_PROFILE_ZONE("cloth::SwSolverKernel::constrainMotion", /*ProfileContext::None*/ 0);

	T4f* curIt = reinterpret_cast<T4f*>(mClothData.mCurParticles);
	T4f* curEnd = curIt + mClothData.mNumParticles;

	const T4f* startIt = reinterpret_cast<const T4f*>(mClothData.mStartMotionConstraints);
	const T4f* targetIt = reinterpret_cast<const T4f*>(mClothData.mTargetMotionConstraints);

	T4f scaleBias = load(&mCloth.mMotionConstraintScale);
	T4f stiffness = simd4f(mClothData.mMotionConstraintStiffness);
	T4f scaleBiasStiffness = select(sMaskXYZ, scaleBias, stiffness);

	if (!mClothData.mTargetMotionConstraints)
	{
		// no interpolation, use the start positions
		return ::constrainMotion(curIt, curEnd, startIt, scaleBiasStiffness);
	}

	if (mState.mRemainingIterations == 1)
	{
		// use the target positions on last iteration
		return ::constrainMotion(curIt, curEnd, targetIt, scaleBiasStiffness);
	}

	// otherwise use an interpolating iterator
	LerpIterator<T4f, const T4f*> interpolator(startIt, targetIt, mState.getCurrentAlpha());
	::constrainMotion(curIt, curEnd, interpolator, scaleBiasStiffness);
}

template <typename T4f>
void cloth::SwSolverKernel<T4f>::constrainSeparation()
{
	if (!mClothData.mStartSeparationConstraints)
		return;

	NV_CLOTH_PROFILE_ZONE("cloth::SwSolverKernel::constrainSeparation", /*ProfileContext::None*/ 0);

	T4f* curIt = reinterpret_cast<T4f*>(mClothData.mCurParticles);
	T4f* curEnd = curIt + mClothData.mNumParticles;

	const T4f* startIt = reinterpret_cast<const T4f*>(mClothData.mStartSeparationConstraints);
	const T4f* targetIt = reinterpret_cast<const T4f*>(mClothData.mTargetSeparationConstraints);

	if (!mClothData.mTargetSeparationConstraints)
	{
		// no interpolation, use the start positions
		return ::constrainSeparation(curIt, curEnd, startIt);
	}

	if (mState.mRemainingIterations == 1)
	{
		// use the target positions on last iteration
		return ::constrainSeparation(curIt, curEnd, targetIt);
	}
	// otherwise use an interpolating iterator
	LerpIterator<T4f, const T4f*> interpolator(startIt, targetIt, mState.getCurrentAlpha());
	::constrainSeparation(curIt, curEnd, interpolator);
}

template <typename T4f>
void cloth::SwSolverKernel<T4f>::collideParticles()
{
	NV_CLOTH_PROFILE_ZONE("cloth::SwSolverKernel::collideParticles", /*ProfileContext::None*/ 0);

	mCollision(mState);
}

template <typename T4f>
void cloth::SwSolverKernel<T4f>::selfCollideParticles()
{
	NV_CLOTH_PROFILE_ZONE("cloth::SwSolverKernel::selfCollideParticles", /*ProfileContext::None*/ 0);

	mSelfCollision();
}

template <typename T4f>
void cloth::SwSolverKernel<T4f>::updateSleepState()
{
	NV_CLOTH_PROFILE_ZONE("cloth::SwSolverKernel::updateSleepState", /*ProfileContext::None*/ 0);

	mClothData.mSleepTestCounter += std::max(1u, uint32_t(mState.mIterDt * 1000));
	if (mClothData.mSleepTestCounter >= mCloth.mSleepTestInterval)
	{
		const T4f* prevIt = reinterpret_cast<T4f*>(mClothData.mPrevParticles);
		const T4f* curIt = reinterpret_cast<T4f*>(mClothData.mCurParticles);
		const T4f* curEnd = curIt + mClothData.mNumParticles;

		// calculate max particle delta since last iteration
		T4f maxDelta = calculateMaxDelta(prevIt, curIt, curEnd);

		++mClothData.mSleepPassCounter;
		T4f threshold = simd4f(mCloth.mSleepThreshold * mState.mIterDt);
		if (anyGreaterEqual(maxDelta, threshold))
			mClothData.mSleepPassCounter = 0;

		mClothData.mSleepTestCounter -= mCloth.mSleepTestInterval;
	}
}

template <typename T4f>
void cloth::SwSolverKernel<T4f>::iterateCloth()
{
	// note on invMass (stored in current/previous positions.w):
	// integrateParticles()
	//   - if (current.w == 0) current.w = previous.w
	// constraintMotion()
	//   - if (constraint.radius <= 0) current.w = 0
	// computeBounds()
	//   - if (current.w > 0) current.w = previous.w
	// collideParticles()
	//   - if (collides) current.w *= 1/massScale
	// after simulate()
	//   - previous.w: original invMass as set by user
	//   - current.w: zeroed by motion constraints and mass-scaled by collision

	// integrate positions
	integrateParticles();

	// apply drag and lift
	applyWind();

	// motion constraints
	constrainMotion();

	// solve tether constraints
	constrainTether();

	// solve edge constraints
	solveFabric();

	// separation constraints
	constrainSeparation();

	// perform character collision
	collideParticles();

	// perform self collision
	selfCollideParticles();

	// test wake / sleep conditions
	updateSleepState();
}

template <typename T4f>
void cloth::SwSolverKernel<T4f>::simulateCloth()
{
	while (mState.mRemainingIterations)
	{
		iterateCloth();
		mState.update();
	}
}

// explicit template instantiation
#if NV_SIMD_SIMD
template class cloth::SwSolverKernel<Simd4f>;
#endif
#if NV_SIMD_SCALAR
template class cloth::SwSolverKernel<Scalar4f>;
#endif
