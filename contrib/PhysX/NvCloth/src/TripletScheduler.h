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

#include "NvCloth/Range.h"
#include "NvCloth/Allocator.h"
#include "Vec4T.h"

namespace nv
{

namespace cloth
{

struct TripletScheduler
{
	typedef Vector<Vec4u>::Type::ConstIterator ConstTripletIter;
	typedef Vector<Vec4u>::Type::Iterator TripletIter;
	typedef Vector<uint32_t>::Type::Iterator SetIter;

	TripletScheduler(Range<const uint32_t[4]>);
	void simd(uint32_t numParticles, uint32_t simdWidth);
	void warp(uint32_t numParticles, uint32_t warpWidth);

	Vector<Vec4u>::Type mTriplets;
	Vector<uint32_t>::Type mSetSizes;
	Vector<Vec4us>::Type mPaddedTriplets;
};
}
}

//Make TripletScheduler available for the unit tests.
namespace nv
{
namespace cloth
{
struct NV_CLOTH_IMPORT TripletSchedulerTestInterface
{
private:
	TripletScheduler* mScheduler;

public:
	TripletSchedulerTestInterface(Range<const uint32_t[4]> triplets);
	~TripletSchedulerTestInterface();
	void simd(uint32_t numParticles, uint32_t simdWidth);
	void warp(uint32_t numParticles, uint32_t warpWidth);

	Range<const uint32_t> GetTriplets();
	Range<const uint32_t> GetSetSizes();
};
}
}

NV_CLOTH_API(nv::cloth::TripletSchedulerTestInterface*) NvClothCreateTripletScheduler(nv::cloth::Range<const uint32_t[4]>);
NV_CLOTH_API(void) NvClothDestroyTripletScheduler(nv::cloth::TripletSchedulerTestInterface*);
