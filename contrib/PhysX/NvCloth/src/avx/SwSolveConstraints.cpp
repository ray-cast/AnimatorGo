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

#pragma warning(push)
#pragma warning(disable : 4668) //'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
#pragma warning(disable : 4987) // nonstandard extension used: 'throw (...)'
#include <intrin.h>
#pragma warning(pop)

#pragma warning(disable : 4127) // conditional expression is constant

typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;

namespace avx
{
__m128 sMaskYZW;
__m256 sOne, sEpsilon, sMinusOneXYZOneW, sMaskXY;

void initialize()
{
	sMaskYZW = _mm_castsi128_ps(_mm_setr_epi32(0, ~0, ~0, ~0));
	sOne = _mm256_set1_ps(1.0f);
	sEpsilon = _mm256_set1_ps(1.192092896e-07f);
	sMinusOneXYZOneW = _mm256_setr_ps(-1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f);
	sMaskXY = _mm256_castsi256_ps(_mm256_setr_epi32(~0, ~0, 0, 0, ~0, ~0, 0, 0));
}

template <uint32_t>
__m256 fmadd_ps(__m256 a, __m256 b, __m256 c)
{
	return _mm256_add_ps(_mm256_mul_ps(a, b), c);
}
template <uint32_t>
__m256 fnmadd_ps(__m256 a, __m256 b, __m256 c)
{
	return _mm256_sub_ps(c, _mm256_mul_ps(a, b));
}
#if _MSC_VER >= 1700
template <>
__m256 fmadd_ps<2>(__m256 a, __m256 b, __m256 c)
{
	return _mm256_fmadd_ps(a, b, c);
}
template <>
__m256 fnmadd_ps<2>(__m256 a, __m256 b, __m256 c)
{
	return _mm256_fnmadd_ps(a, b, c);
}
#endif

template <uint32_t>
__m256 exp2(const __m256& v)
{
	// http://www.netlib.org/cephes/


	__m256 x = _mm256_min_ps(_mm256_max_ps(_mm256_set1_ps(-127.4999f), v), _mm256_set1_ps(127.4999f));

	// separate into integer and fractional part

	__m256 fx = _mm256_add_ps(x,_mm256_set1_ps(0.5f));
	__m128 fx0 = _mm256_extractf128_ps(fx,0);
	__m128 fx1 = _mm256_extractf128_ps(fx,1);

	__m128i ix0 = _mm_sub_epi32(_mm_cvttps_epi32(fx0), _mm_srli_epi32(_mm_castps_si128(fx0), 31));
	__m128i ix1 = _mm_sub_epi32(_mm_cvttps_epi32(fx1), _mm_srli_epi32(_mm_castps_si128(fx1), 31));
	__m256i ix = _mm256_loadu2_m128i(&ix0,&ix1);
	fx = _mm256_sub_ps(x,_mm256_cvtepi32_ps(ix));

	// exp2(fx) ~ 1 + 2 * P(fx) / (Q(fx) - P(fx))

	__m256 fx2 = _mm256_mul_ps(fx,fx);

	__m256 px = _mm256_mul_ps(fx,
						_mm256_add_ps(_mm256_add_ps(
							_mm256_set1_ps(1.51390680115615096133e+3f),
							_mm256_mul_ps(fx2, _mm256_set1_ps(2.02020656693165307700e+1f))),
							_mm256_mul_ps(fx2, _mm256_set1_ps(2.30933477057345225087e-2f))
						)
				);



	__m256 qx = _mm256_add_ps(
					_mm256_set1_ps(4.36821166879210612817e+3f),
					_mm256_mul_ps(
						fx2,
						_mm256_add_ps(_mm256_set1_ps(2.33184211722314911771e+2f), fx2)
					)
				);

	__m256 exp2fx = _mm256_mul_ps(px,_mm256_rcp_ps(_mm256_sub_ps(qx, px)));
	exp2fx = _mm256_add_ps(_mm256_add_ps(sOne, exp2fx), exp2fx);

	// exp2(ix)

	__m128 exp2ix0 = _mm_castsi128_ps(_mm_slli_epi32(_mm_add_epi32(ix0, _mm_set1_epi32(0x7f)), 23));
	__m128 exp2ix1 = _mm_castsi128_ps(_mm_slli_epi32(_mm_add_epi32(ix1, _mm_set1_epi32(0x7f)), 23));
	__m256 exp2ix = _mm256_loadu2_m128((float*)&exp2ix0,(float*)&exp2ix1);

	return _mm256_mul_ps(exp2fx, exp2ix);
}
#if _MSC_VER >= 1700
//AVX2
template <>
__m256 exp2<2>(const __m256& v)
{
	// http://www.netlib.org/cephes/


	__m256 x = _mm256_min_ps(_mm256_max_ps(_mm256_set1_ps(-127.4999f), v), _mm256_set1_ps(127.4999f));

	// separate into integer and fractional part

	__m256 fx = _mm256_add_ps(x,_mm256_set1_ps(0.5f));
	__m256i ix = _mm256_sub_epi32(_mm256_cvttps_epi32(fx), _mm256_srli_epi32(_mm256_castps_si256(fx), 31));
	fx = _mm256_sub_ps(x,_mm256_cvtepi32_ps(ix));

	// exp2(fx) ~ 1 + 2 * P(fx) / (Q(fx) - P(fx))

	__m256 fx2 = _mm256_mul_ps(fx,fx);

	__m256 px = _mm256_mul_ps(fx,
						_mm256_add_ps(_mm256_add_ps(
							_mm256_set1_ps(1.51390680115615096133e+3f),
							_mm256_mul_ps(fx2, _mm256_set1_ps(2.02020656693165307700e+1f))),
							_mm256_mul_ps(fx2, _mm256_set1_ps(2.30933477057345225087e-2f))
						)
				);



	__m256 qx = _mm256_add_ps(
					_mm256_set1_ps(4.36821166879210612817e+3f),
					_mm256_mul_ps(
						fx2,
						_mm256_add_ps(_mm256_set1_ps(2.33184211722314911771e+2f), fx2)
					)
				);

	__m256 exp2fx = _mm256_mul_ps(px,_mm256_rcp_ps(_mm256_sub_ps(qx, px)));
	exp2fx = _mm256_add_ps(_mm256_add_ps(sOne, exp2fx), exp2fx);

	// exp2(ix)

	__m256 exp2ix = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_add_epi32(ix, _mm256_set1_epi32(0x7f)), 23));

	return _mm256_mul_ps(exp2fx, exp2ix);
}
#endif

// roughly same perf as SSE2 intrinsics, the asm version below is about 10% faster
template <bool useMultiplier, uint32_t avx>
void solveConstraints(float* __restrict posIt, const float* __restrict rIt, const float* __restrict stIt, const float* __restrict rEnd,
                      const uint16_t* __restrict iIt, const __m128& stiffnessEtc, const __m128& stiffnessExponent)
{
	__m256 stiffness, stretchLimit, compressionLimit, multiplier;

	if (useMultiplier)
	{
		//	  A0          A1            A2              A3            B0          B1            B2              B3
		// (stiffness, multiplier, compressionLimit, stretchLimit, stiffness, multiplier, compressionLimit, stretchLimit)
		//  float*[0], float*[1], etc..
		stiffness = _mm256_broadcast_ps(&stiffnessEtc);
		stretchLimit = _mm256_permute_ps(stiffness, 0xff);		//(A3, A3, A3, A3, B3, B3, B3, B3)
		compressionLimit = _mm256_permute_ps(stiffness, 0xaa);	//(A2, A2, A2, A2, B2, B2, B2, B2)
		multiplier = _mm256_permute_ps(stiffness, 0x55);		//(A1, A1, A1, A1, B1, B1, B1, B1)
		stiffness = _mm256_permute_ps(stiffness, 0x00);			//(A0, A0, A0, A0, B0, B0, B0, B0)
	}
	else
	{
		stiffness = _mm256_broadcast_ss((const float*)&stiffnessEtc); // (float*[0], float*[0], ..., float*[0]);
	}

	bool useStiffnessPerConstraint = stIt!=nullptr;

	for (; rIt < rEnd; rIt += 8, iIt += 16, stIt += 8)
	{
		float* p0i = posIt + iIt[0] * 4;
		float* p4i = posIt + iIt[8] * 4;
		float* p0j = posIt + iIt[1] * 4;
		float* p4j = posIt + iIt[9] * 4;
		float* p1i = posIt + iIt[2] * 4;
		float* p5i = posIt + iIt[10] * 4;
		float* p1j = posIt + iIt[3] * 4;
		float* p5j = posIt + iIt[11] * 4;

		__m128 v0i = _mm_load_ps(p0i);
		__m128 v4i = _mm_load_ps(p4i);
		__m128 v0j = _mm_load_ps(p0j);
		__m128 v4j = _mm_load_ps(p4j);
		__m128 v1i = _mm_load_ps(p1i);
		__m128 v5i = _mm_load_ps(p5i);
		__m128 v1j = _mm_load_ps(p1j);
		__m128 v5j = _mm_load_ps(p5j);

		__m256 v04i = _mm256_insertf128_ps(_mm256_castps128_ps256(v0i), v4i, 1);
		__m256 v04j = _mm256_insertf128_ps(_mm256_castps128_ps256(v0j), v4j, 1);
		__m256 v15i = _mm256_insertf128_ps(_mm256_castps128_ps256(v1i), v5i, 1);
		__m256 v15j = _mm256_insertf128_ps(_mm256_castps128_ps256(v1j), v5j, 1);

		__m256 h04ij = fmadd_ps<avx>(sMinusOneXYZOneW, v04i, v04j);
		__m256 h15ij = fmadd_ps<avx>(sMinusOneXYZOneW, v15i, v15j);

		float* p2i = posIt + iIt[4] * 4;
		float* p6i = posIt + iIt[12] * 4;
		float* p2j = posIt + iIt[5] * 4;
		float* p6j = posIt + iIt[13] * 4;
		float* p3i = posIt + iIt[6] * 4;
		float* p7i = posIt + iIt[14] * 4;
		float* p3j = posIt + iIt[7] * 4;
		float* p7j = posIt + iIt[15] * 4;

		__m128 v2i = _mm_load_ps(p2i);
		__m128 v6i = _mm_load_ps(p6i);
		__m128 v2j = _mm_load_ps(p2j);
		__m128 v6j = _mm_load_ps(p6j);
		__m128 v3i = _mm_load_ps(p3i);
		__m128 v7i = _mm_load_ps(p7i);
		__m128 v3j = _mm_load_ps(p3j);
		__m128 v7j = _mm_load_ps(p7j);

		__m256 v26i = _mm256_insertf128_ps(_mm256_castps128_ps256(v2i), v6i, 1);
		__m256 v26j = _mm256_insertf128_ps(_mm256_castps128_ps256(v2j), v6j, 1);
		__m256 v37i = _mm256_insertf128_ps(_mm256_castps128_ps256(v3i), v7i, 1);
		__m256 v37j = _mm256_insertf128_ps(_mm256_castps128_ps256(v3j), v7j, 1);

		__m256 h26ij = fmadd_ps<avx>(sMinusOneXYZOneW, v26i, v26j);
		__m256 h37ij = fmadd_ps<avx>(sMinusOneXYZOneW, v37i, v37j);

		__m256 a = _mm256_unpacklo_ps(h04ij, h26ij);
		__m256 b = _mm256_unpackhi_ps(h04ij, h26ij);
		__m256 c = _mm256_unpacklo_ps(h15ij, h37ij);
		__m256 d = _mm256_unpackhi_ps(h15ij, h37ij);

		__m256 hxij = _mm256_unpacklo_ps(a, c);
		__m256 hyij = _mm256_unpackhi_ps(a, c);
		__m256 hzij = _mm256_unpacklo_ps(b, d);
		__m256 vwij = _mm256_unpackhi_ps(b, d);

		__m256 e2ij = fmadd_ps<avx>(hxij, hxij, fmadd_ps<avx>(hyij, hyij, fmadd_ps<avx>(hzij, hzij, sEpsilon)));

		__m256 rij = _mm256_load_ps(rIt);
		__m256 stij = useStiffnessPerConstraint?_mm256_sub_ps(sOne, exp2<avx>(_mm256_mul_ps(_mm256_load_ps(stIt),_mm256_broadcast_ps(&stiffnessExponent)))):stiffness;
		__m256 mask = _mm256_cmp_ps(rij, sEpsilon, _CMP_GT_OQ);
		__m256 erij = _mm256_and_ps(fnmadd_ps<avx>(rij, _mm256_rsqrt_ps(e2ij), sOne), mask);

		if (useMultiplier)
		{
			erij = fnmadd_ps<avx>(multiplier, _mm256_max_ps(compressionLimit, _mm256_min_ps(erij, stretchLimit)), erij);
		}

		__m256 exij = _mm256_mul_ps(erij, _mm256_mul_ps(stij, _mm256_rcp_ps(_mm256_add_ps(sEpsilon, vwij))));

		// replace these two instructions with _mm_maskstore_ps below?
		__m256 exlo = _mm256_and_ps(sMaskXY, exij);
		__m256 exhi = _mm256_andnot_ps(sMaskXY, exij);

		__m256 f04ij = _mm256_mul_ps(h04ij, _mm256_permute_ps(exlo, 0xc0));
		__m256 u04i = fmadd_ps<avx>(f04ij, _mm256_permute_ps(v04i, 0xff), v04i);
		__m256 u04j = fnmadd_ps<avx>(f04ij, _mm256_permute_ps(v04j, 0xff), v04j);

		_mm_store_ps(p0i, _mm256_extractf128_ps(u04i, 0));
		_mm_store_ps(p0j, _mm256_extractf128_ps(u04j, 0));
		_mm_store_ps(p4i, _mm256_extractf128_ps(u04i, 1));
		_mm_store_ps(p4j, _mm256_extractf128_ps(u04j, 1));

		__m256 f15ij = _mm256_mul_ps(h15ij, _mm256_permute_ps(exlo, 0xd5));
		__m256 u15i = fmadd_ps<avx>(f15ij, _mm256_permute_ps(v15i, 0xff), v15i);
		__m256 u15j = fnmadd_ps<avx>(f15ij, _mm256_permute_ps(v15j, 0xff), v15j);

		_mm_store_ps(p1i, _mm256_extractf128_ps(u15i, 0));
		_mm_store_ps(p1j, _mm256_extractf128_ps(u15j, 0));
		_mm_store_ps(p5i, _mm256_extractf128_ps(u15i, 1));
		_mm_store_ps(p5j, _mm256_extractf128_ps(u15j, 1));

		__m256 f26ij = _mm256_mul_ps(h26ij, _mm256_permute_ps(exhi, 0x2a));
		__m256 u26i = fmadd_ps<avx>(f26ij, _mm256_permute_ps(v26i, 0xff), v26i);
		__m256 u26j = fnmadd_ps<avx>(f26ij, _mm256_permute_ps(v26j, 0xff), v26j);

		_mm_store_ps(p2i, _mm256_extractf128_ps(u26i, 0));
		_mm_store_ps(p2j, _mm256_extractf128_ps(u26j, 0));
		_mm_store_ps(p6i, _mm256_extractf128_ps(u26i, 1));
		_mm_store_ps(p6j, _mm256_extractf128_ps(u26j, 1));

		__m256 f37ij = _mm256_mul_ps(h37ij, _mm256_permute_ps(exhi, 0x3f));
		__m256 u37i = fmadd_ps<avx>(f37ij, _mm256_permute_ps(v37i, 0xff), v37i);
		__m256 u37j = fnmadd_ps<avx>(f37ij, _mm256_permute_ps(v37j, 0xff), v37j);

		_mm_store_ps(p3i, _mm256_extractf128_ps(u37i, 0));
		_mm_store_ps(p3j, _mm256_extractf128_ps(u37j, 0));
		_mm_store_ps(p7i, _mm256_extractf128_ps(u37i, 1));
		_mm_store_ps(p7j, _mm256_extractf128_ps(u37j, 1));
	}

	_mm256_zeroupper();
}


template void solveConstraints<false, 1>(float* __restrict, const float* __restrict, const float* __restrict, const float* __restrict,
                                         const uint16_t* __restrict, const __m128&, const __m128&);

template void solveConstraints<true, 1>(float* __restrict, const float* __restrict, const float* __restrict, const float* __restrict,
                                         const uint16_t* __restrict, const __m128&, const __m128&);

template void solveConstraints<false, 2>(float* __restrict, const float* __restrict, const float* __restrict, const float* __restrict,
                                         const uint16_t* __restrict, const __m128&, const __m128&);

template void solveConstraints<true, 2>(float* __restrict, const float* __restrict, const float* __restrict, const float* __restrict,
                                         const uint16_t* __restrict, const __m128&, const __m128&);


} // namespace avx
