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

#include "foundation/PxVec4.h"
#include "foundation/PxMemory.h"
#include "foundation/PxStrideIterator.h"
#include "NvClothExt/ClothTetherCooker.h"
#include "NvCloth/Allocator.h"

using namespace physx;

namespace nv
{
namespace cloth
{

struct ClothSimpleTetherCooker : public ClothTetherCooker
{
	virtual bool cook(const ClothMeshDesc& desc) override;

	virtual uint32_t	getCookerStatus() const override; //From APEX
	virtual void	getTetherData(PxU32* userTetherAnchors, PxReal* userTetherLengths) const override;
	virtual PxU32 getNbTethersPerParticle() const override{ return 1; }

public:
	// output
	nv::cloth::Vector<PxU32>::Type	mTetherAnchors;
	nv::cloth::Vector<PxReal>::Type	mTetherLengths;

protected:
	void	createTetherData(const ClothMeshDesc &desc);

	uint32_t	mCookerStatus; //From APEX
};


///////////////////////////////////////////////////////////////////////////////
bool ClothSimpleTetherCooker::cook(const ClothMeshDesc &desc)
{
	mCookerStatus = 1;
	createTetherData(desc);
	return getCookerStatus() == 0;
}

///////////////////////////////////////////////////////////////////////////////
void ClothSimpleTetherCooker::createTetherData(const ClothMeshDesc &desc)
{
	PxU32 numParticles = desc.points.count;

	if (!desc.invMasses.data)
		return;

	// assemble points
	nv::cloth::Vector<PxVec4>::Type particles;
	particles.reserve(numParticles);
	PxStrideIterator<const PxVec3> pIt(reinterpret_cast<const PxVec3*>(desc.points.data), desc.points.stride);
	PxStrideIterator<const PxReal> wIt(reinterpret_cast<const PxReal*>(desc.invMasses.data), desc.invMasses.stride);
	for(PxU32 i=0; i<numParticles; ++i)
		particles.pushBack(PxVec4(*pIt++, wIt.ptr() ? *wIt++ : 1.0f));

	// compute tether data
	nv::cloth::Vector<PxU32>::Type attachedIndices;
	for(PxU32 i=0; i < numParticles; ++i)
		if(particles[i].w == 0.0f)
			attachedIndices.pushBack(i);

	PxU32 n = attachedIndices.empty() ? 0 : numParticles;
	for(PxU32 i=0; i < n; ++i)
	{
		mTetherAnchors.reserve(numParticles);
		mTetherLengths.reserve(numParticles);

		PxVec3 position = reinterpret_cast<const PxVec3&>(particles[i]);
		float minSqrDist = FLT_MAX;
		PxU32 minIndex = numParticles;
		const PxU32 *aIt, *aEnd = attachedIndices.end();
		for(aIt = attachedIndices.begin(); aIt != aEnd; ++aIt)
		{
			float sqrDist = (reinterpret_cast<const PxVec3&>(
				particles[*aIt]) - position).magnitudeSquared();
			if(minSqrDist > sqrDist)
			{
				minSqrDist = sqrDist;
				minIndex = *aIt;
			}
		}

		mTetherAnchors.pushBack(minIndex);
		mTetherLengths.pushBack(PxSqrt(minSqrDist));
	}

	NV_CLOTH_ASSERT(mTetherAnchors.size() == mTetherLengths.size());

	if (numParticles == mTetherAnchors.size() && numParticles == mTetherLengths.size())
	{
		mCookerStatus = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////  
uint32_t ClothSimpleTetherCooker::getCookerStatus() const
{
	return mCookerStatus;
}

///////////////////////////////////////////////////////////////////////////////
void  
ClothSimpleTetherCooker::getTetherData(PxU32* userTetherAnchors, PxReal* userTetherLengths) const
{
	PxMemCopy(userTetherAnchors, mTetherAnchors.begin(), mTetherAnchors.size() * sizeof(PxU32));
	PxMemCopy(userTetherLengths, mTetherLengths.begin(), mTetherLengths.size() * sizeof(PxReal));
}


} // namespace cloth
} // namespace nv

NV_CLOTH_API(nv::cloth::ClothTetherCooker*) NvClothCreateSimpleTetherCooker()
{
	return NV_CLOTH_NEW(nv::cloth::ClothSimpleTetherCooker);
}
