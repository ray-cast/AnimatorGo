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

#include "ClothClone.h"

#include "DxFactory.h"
#include "DxFabric.h"
#include "DxCloth.h"

#if NV_CLOTH_ENABLE_DX11

using namespace physx;

namespace nv
{
namespace cloth
{
Range<const PhaseConfig> getPhaseConfigs(const DxCloth& cloth)
{
	return makeRange(cloth.mHostPhaseConfigs);
}
void setPhaseConfigs(DxCloth& cloth, Range<const PhaseConfig> phaseConfigs)
{
	cloth.setPhaseConfig(phaseConfigs);
}
Range<const PxVec4> getParticleAccelerations(const DxCloth& cloth)
{
	return makeRange(cloth.mParticleAccelerationsHostCopy);
}
Range<const uint32_t> getSelfCollisionIndices(const DxCloth& cloth)
{
	return makeRange(cloth.mSelfCollisionIndicesHost);
}

Cloth* DxCloth::clone(Factory& factory) const
{
	if (&mFactory == &factory)
		return NV_CLOTH_NEW(DxCloth)(mFactory, *this); // copy construct directly

	switch(factory.getPlatform())
	{
	case Platform::CPU:
		return convertCloth(static_cast<SwFactory&>(factory), *this);
	case Platform::DX11:
		return convertCloth(static_cast<DxFactory&>(factory), *this);
	case Platform::CUDA:
	default:
		return nullptr;
	}
}

Cloth* DxFactory::clone(const Cloth& cloth)
{
	if (cloth.getFactory().getPlatform() == Platform::CPU)
	{
		if (cloth.getNumParticles() > 2025 /* see DxSolverKernel.hlsl gCurParticles */)
			return nullptr; // can only fit 2025 particles in dx shared memory
		return convertCloth(*this, static_cast<const SwCloth&>(cloth));
	}

	return cloth.clone(*this);
}

} // namespace cloth
} // namespace nv

#endif // NV_CLOTH_ENABLE_DX11
