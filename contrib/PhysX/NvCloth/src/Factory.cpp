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

#ifndef NV_CLOTH_ENABLE_CUDA
#define NV_CLOTH_ENABLE_CUDA 1
#endif

#include "SwFactory.h"

using namespace physx;

namespace nv
{
namespace cloth
{
uint32_t getNextFabricId()
{
	static uint32_t sNextFabricId = 0;
	return sNextFabricId++;
}

}
}


nv::cloth::Factory* NvClothCreateFactoryCPU()
{
	return NV_CLOTH_NEW(nv::cloth::SwFactory)();
}

#if NV_CLOTH_ENABLE_CUDA
#include "cuda/CuFactory.h"

nv::cloth::Factory* NvClothCreateFactoryCUDA(CUcontext context)
{
	return NV_CLOTH_NEW(nv::cloth::CuFactory)(context);
}
#else
nv::cloth::Factory* NvClothCreateFactoryCUDA(CUcontext context)
{
	PX_UNUSED(context);
	return nullptr;
}
#endif

#if NV_CLOTH_ENABLE_DX11
#include "dx/dxFactory.h"
#include "NvCloth/DxContextManagerCallback.h"
nv::cloth::Factory* NvClothCreateFactoryDX11(nv::cloth::DxContextManagerCallback* graphicsContextManager)
{
	if (graphicsContextManager->getDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0)
		return NV_CLOTH_NEW(nv::cloth::DxFactory)(graphicsContextManager);
	return nullptr;
}
#else
nv::cloth::Factory* NvClothCreateFactoryDX11(nv::cloth::DxContextManagerCallback* graphicsContextManager)
{
	PX_UNUSED(graphicsContextManager);
	return nullptr;
}
#endif

NV_CLOTH_API(void) NvClothDestroyFactory(nv::cloth::Factory* factory)
{
	NV_CLOTH_DELETE(factory);
}

NV_CLOTH_API(bool) NvClothCompiledWithCudaSupport()
{
	return NV_CLOTH_ENABLE_CUDA == 1;
}
NV_CLOTH_API(bool) NvClothCompiledWithDxSupport()
{
	return NV_CLOTH_ENABLE_DX11 == 1;
}
