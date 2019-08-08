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

#include <foundation/Px.h>

namespace nv
{
namespace cloth
{
// Note: CuProfileZoneIds has a corresponding enum list (gCuProfileZoneNames) in CuSolver.cpp.
// Additions/deletions to CuProfileZoneIds requires a similar action to gCuProfileZoneNames.
struct CuProfileZoneIds
{
	enum Enum
	{
		SIMULATE,
		INTEGRATE,
		ACCELERATE,
		WIND,
		TETHER,
		FABRIC,
		MOTION,
		SEPARATION,
		COLLIDE,
		SELFCOLLIDE,
		SLEEP,
		SIMULATE_SHARED,
		SIMULATE_STREAMED,
		SIMULATE_GLOBAL,
		CONSTRAINT_SET,
		COLLIDE_ACCELERATION,
		COLLIDE_CAPSULES,
		COLLIDE_VIRTUAL_CAPSULES,
		COLLIDE_CONTINUOUS_CAPSULES,
		COLLIDE_CONVEXES,
		COLLIDE_TRIANGLES,
		SELFCOLLIDE_ACCELERATION,
		SELFCOLLIDE_PARTICLES,
		NUMZONES
	};
};

struct CuClothData;
struct CuFrameData;

// data of all cloth instances, one block per instance
struct CuKernelData
{
	// pointer to atomic variable
	uint32_t* mClothIndex;

	// array of cloths (length determined by grid dim)
	const CuClothData* mClothData;

	// frame data per cloth
	CuFrameData* mFrameData;

	void* mProfileBuffer;
	uint32_t mProfileBaseId;
};

const char* getKernelDataName();
const char* getKernelFunctionName();
}
}
