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

#include "Simd.h"

namespace nv
{

namespace cloth
{

template <typename T4f>
struct BoundingBox
{
	T4f mLower;
	T4f mUpper;
};

template <typename T4f>
inline BoundingBox<T4f> loadBounds(const float* ptr)
{
	BoundingBox<T4f> result;
	result.mLower = load(ptr);
	result.mUpper = load(ptr + 3);
	return result;
}

template <typename T4f>
inline BoundingBox<T4f> emptyBounds()
{
	BoundingBox<T4f> result;

	result.mLower = gSimd4fFloatMax;
	result.mUpper = -result.mLower;

	return result;
}

template <typename T4f>
inline BoundingBox<T4f> expandBounds(const BoundingBox<T4f>& bounds, const T4f* pIt, const T4f* pEnd)
{
	BoundingBox<T4f> result = bounds;
	for (; pIt != pEnd; ++pIt)
	{
		result.mLower = min(result.mLower, *pIt);
		result.mUpper = max(result.mUpper, *pIt);
	}
	return result;
}

template <typename T4f>
inline BoundingBox<T4f> expandBounds(const BoundingBox<T4f>& a, const BoundingBox<T4f>& b)
{
	BoundingBox<T4f> result;
	result.mLower = min(a.mLower, b.mLower);
	result.mUpper = max(a.mUpper, b.mUpper);
	return result;
}

template <typename T4f>
inline BoundingBox<T4f> intersectBounds(const BoundingBox<T4f>& a, const BoundingBox<T4f>& b)
{
	BoundingBox<T4f> result;
	result.mLower = max(a.mLower, b.mLower);
	result.mUpper = min(a.mUpper, b.mUpper);
	return result;
}

template <typename T4f>
inline bool isEmptyBounds(const BoundingBox<T4f>& a)
{
	return anyGreater(a.mLower, a.mUpper) != 0;
}
}
}
