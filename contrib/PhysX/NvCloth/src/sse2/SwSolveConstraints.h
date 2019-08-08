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

#include "NvSimd/sse2/NvSse2Simd4f.h"

__pragma(warning(push))
#pragma warning(disable : 4127) // conditional expression is constant

template <bool useMultiplier>
void solveConstraints(float* __restrict posIt, const float* __restrict rIt, const float* __restrict stIt, const float* __restrict rEnd,
						const uint16_t* __restrict iIt, const Simd4f& stiffnessEtc, const Simd4f& stiffnessExponent)
{
	PX_UNUSED(stIt);
	PX_UNUSED(stiffnessEtc);
	PX_UNUSED(stiffnessExponent);
	__m128 sOne = _mm_set1_ps(1.0f);

	__m128 stretchLimit, compressionLimit, multiplier;
	if (useMultiplier)
	{
		stretchLimit = _mm_shuffle_ps(stiffnessEtc, stiffnessEtc, 0xff);
		compressionLimit = _mm_shuffle_ps(stiffnessEtc, stiffnessEtc, 0xaa);
		multiplier = _mm_shuffle_ps(stiffnessEtc, stiffnessEtc, 0x55);
	}
	__m128 stiffness = _mm_shuffle_ps(stiffnessEtc, stiffnessEtc, 0x00);
	bool useStiffnessPerConstraint = nullptr != stIt;

	for (; rIt != rEnd; rIt += 4, stIt += 4, iIt += 8)
	{
		float* p0i = posIt + iIt[0] * 4;
		float* p0j = posIt + iIt[1] * 4;
		float* p1i = posIt + iIt[2] * 4;
		float* p1j = posIt + iIt[3] * 4;
		float* p2i = posIt + iIt[4] * 4;
		float* p2j = posIt + iIt[5] * 4;
		float* p3i = posIt + iIt[6] * 4;
		float* p3j = posIt + iIt[7] * 4;

		__m128 v0i = _mm_load_ps(p0i);
		__m128 v0j = _mm_load_ps(p0j);
		__m128 v1i = _mm_load_ps(p1i);
		__m128 v1j = _mm_load_ps(p1j);
		__m128 v2i = _mm_load_ps(p2i);
		__m128 v2j = _mm_load_ps(p2j);
		__m128 v3i = _mm_load_ps(p3i);
		__m128 v3j = _mm_load_ps(p3j);

		__m128 h0ij = _mm_add_ps(v0j, _mm_mul_ps(v0i, sMinusOneXYZOneW));
		__m128 h1ij = _mm_add_ps(v1j, _mm_mul_ps(v1i, sMinusOneXYZOneW));
		__m128 h2ij = _mm_add_ps(v2j, _mm_mul_ps(v2i, sMinusOneXYZOneW));
		__m128 h3ij = _mm_add_ps(v3j, _mm_mul_ps(v3i, sMinusOneXYZOneW));

		__m128 a = _mm_unpacklo_ps(h0ij, h2ij);
		__m128 b = _mm_unpackhi_ps(h0ij, h2ij);
		__m128 c = _mm_unpacklo_ps(h1ij, h3ij);
		__m128 d = _mm_unpackhi_ps(h1ij, h3ij);

		__m128 hxij = _mm_unpacklo_ps(a, c);
		__m128 hyij = _mm_unpackhi_ps(a, c);
		__m128 hzij = _mm_unpacklo_ps(b, d);
		__m128 vwij = _mm_unpackhi_ps(b, d);

		__m128 rij = _mm_load_ps(rIt);
		__m128 e2ij = _mm_add_ps(gSimd4fEpsilon, _mm_add_ps(_mm_mul_ps(hxij, hxij),
		                                                    _mm_add_ps(_mm_mul_ps(hyij, hyij), _mm_mul_ps(hzij, hzij))));

		//Load/calculate the constraint stiffness
		__m128 stij = useStiffnessPerConstraint ? _mm_set_ps1(1.0f) - exp2(stiffnessExponent * _mm_load_ps(stIt)) : stiffness;


		__m128 mask = _mm_cmpnle_ps(rij, gSimd4fEpsilon);
		__m128 erij = _mm_and_ps(_mm_sub_ps(sOne, _mm_mul_ps(rij, _mm_rsqrt_ps(e2ij))), mask);

		if (useMultiplier)
		{
			erij = _mm_sub_ps(erij, _mm_mul_ps(multiplier, _mm_max_ps(compressionLimit, _mm_min_ps(erij, stretchLimit))));
		}
		__m128 exij = _mm_mul_ps(erij, _mm_mul_ps(stij, _mm_rcp_ps(_mm_add_ps(gSimd4fEpsilon, vwij))));

		__m128 exlo = _mm_and_ps(sMaskXY, exij);
		__m128 exhi = _mm_andnot_ps(sMaskXY, exij);

		__m128 f0ij = _mm_mul_ps(h0ij, _mm_shuffle_ps(exlo, exlo, 0xc0));
		__m128 f1ij = _mm_mul_ps(h1ij, _mm_shuffle_ps(exlo, exlo, 0xd5));
		__m128 f2ij = _mm_mul_ps(h2ij, _mm_shuffle_ps(exhi, exhi, 0x2a));
		__m128 f3ij = _mm_mul_ps(h3ij, _mm_shuffle_ps(exhi, exhi, 0x3f));

		__m128 u0i = _mm_add_ps(v0i, _mm_mul_ps(f0ij, _mm_shuffle_ps(v0i, v0i, 0xff)));
		__m128 u0j = _mm_sub_ps(v0j, _mm_mul_ps(f0ij, _mm_shuffle_ps(v0j, v0j, 0xff)));
		__m128 u1i = _mm_add_ps(v1i, _mm_mul_ps(f1ij, _mm_shuffle_ps(v1i, v1i, 0xff)));
		__m128 u1j = _mm_sub_ps(v1j, _mm_mul_ps(f1ij, _mm_shuffle_ps(v1j, v1j, 0xff)));
		__m128 u2i = _mm_add_ps(v2i, _mm_mul_ps(f2ij, _mm_shuffle_ps(v2i, v2i, 0xff)));
		__m128 u2j = _mm_sub_ps(v2j, _mm_mul_ps(f2ij, _mm_shuffle_ps(v2j, v2j, 0xff)));
		__m128 u3i = _mm_add_ps(v3i, _mm_mul_ps(f3ij, _mm_shuffle_ps(v3i, v3i, 0xff)));
		__m128 u3j = _mm_sub_ps(v3j, _mm_mul_ps(f3ij, _mm_shuffle_ps(v3j, v3j, 0xff)));

		_mm_store_ps(p0i, u0i);
		_mm_store_ps(p0j, u0j);
		_mm_store_ps(p1i, u1i);
		_mm_store_ps(p1j, u1j);
		_mm_store_ps(p2i, u2i);
		_mm_store_ps(p2j, u2j);
		_mm_store_ps(p3i, u3i);
		_mm_store_ps(p3j, u3j);
	}
}

__pragma(warning(pop))
