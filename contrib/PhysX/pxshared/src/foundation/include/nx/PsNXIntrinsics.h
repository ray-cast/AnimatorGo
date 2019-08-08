/*
 * Copyright (c) 2008-2015, NVIDIA CORPORATION.  All rights reserved.
 *
 * NVIDIA CORPORATION and its licensors retain all intellectual property
 * and proprietary rights in and to this software, related documentation
 * and any modifications thereto.  Any use, reproduction, disclosure or
 * distribution of this software and related documentation without an express
 * license agreement from NVIDIA CORPORATION is strictly prohibited.
 */
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  


#ifndef PX_FOUNDATION_PS_NX_INTRINSICS_H
#define PX_FOUNDATION_PS_NX_INTRINSICS_H

#include "Ps.h"
#include "foundation/PxAssert.h"

// this file is for internal intrinsics - that is, intrinsics that are used in
// cross platform code but do not appear in the API

#if !PX_NX
	#error "This file should only be included by NX builds!!"
#endif

#include <math.h>

namespace physx
{
namespace shdfnd
{
	/*
	 * Implements a memory barrier
	 */
	PX_FORCE_INLINE void memoryBarrier()
	{
		__sync_synchronize();
	}

	/*!
	Returns the index of the highest set bit. Not valid for zero arg.
	*/
	PX_FORCE_INLINE PxU32 highestSetBitUnsafe(PxU32 v)
	{
		// http://graphics.stanford.edu/~seander/bithacks.html
		static const PxU32 MultiplyDeBruijnBitPosition[32] = 
		{
			0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
			8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
		};

		v |= v >> 1; // first round up to one less than a power of 2 
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;

		return MultiplyDeBruijnBitPosition[(PxU32)(v * 0x07C4ACDDU) >> 27];
	}

	/*!
	Returns the index of the highest set bit. Undefined for zero arg.
	*/
	PX_FORCE_INLINE PxU32 lowestSetBitUnsafe(PxU32 v)
	{
		// http://graphics.stanford.edu/~seander/bithacks.html
		static const PxU32 MultiplyDeBruijnBitPosition[32] = 
		{
			0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
			31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
		};
		PxI32 w = v;
		return MultiplyDeBruijnBitPosition[(PxU32)((w & -w) * 0x077CB531U) >> 27];
	}

	/*!
	Returns the number of leading zeros in v. Returns 32 for v=0.
	*/
	PX_FORCE_INLINE PxU32 countLeadingZeros(PxU32 v)
	{
		PxI32 result = 0;
		PxU32 testBit = (1<<31);
		while ((v & testBit) == 0 && testBit != 0)
			result ++, testBit >>= 1;
		return result;
	}

	/*!
	Prefetch aligned cache size around \c ptr+offset.
	*/
	PX_FORCE_INLINE void prefetchLine(const void* ptr, PxU32 offset = 0)
	{
		__builtin_prefetch((char* PX_RESTRICT)(ptr) + offset, 0, 3);
	}

	/*!
	Prefetch \c count bytes starting at \c ptr.
	*/
	PX_FORCE_INLINE void prefetch(const void* ptr, PxU32 count = 1)
	{
		const char* cp = (char*)ptr;
		PxU64 p = size_t(ptr);
		PxU64 startLine = p>>6, endLine = (p+count-1)>>6;
		PxU64 lines = endLine - startLine + 1;
		do
		{
			prefetchLine(cp);
			cp+=64;
		} while(--lines);
	}

	//! \brief platform-specific reciprocal
	PX_CUDA_CALLABLE PX_FORCE_INLINE float recipFast(float a)				{	return 1.0f/a;			}

	//! \brief platform-specific fast reciprocal square root
	PX_CUDA_CALLABLE PX_FORCE_INLINE float recipSqrtFast(float a)			{   return 1.0f/::sqrtf(a); }

	//! \brief platform-specific floor
	PX_CUDA_CALLABLE PX_FORCE_INLINE float floatFloor(float x)
	{
		return ::floorf(x);
	}

	#define PX_PRINTF printf
	#define PX_EXPECT_TRUE(x) x
	#define PX_EXPECT_FALSE(x) x

} // namespace shdfnd
} // namespace physx

#define PX_EXPECT_TRUE(x) x
#define PX_EXPECT_FALSE(x) x

#endif
