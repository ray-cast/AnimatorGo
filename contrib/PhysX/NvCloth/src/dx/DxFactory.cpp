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

#include "NvCloth/DxContextManagerCallback.h"
#include "DxFactory.h"
#include "DxFabric.h"
#include "DxCloth.h"
#include "DxSolver.h"
#include "DxCheckSuccess.h"
#include "DxContextLock.h"
#include "../ClothImpl.h"
#include <PsFoundation.h>

#if NV_CLOTH_ENABLE_DX11

#include "DxSolverKernelBlob.h"

using namespace physx;
using namespace nv;

namespace nv
{
namespace cloth
{
// defined in Factory.cpp
uint32_t getNextFabricId();

typedef Vec4T<uint32_t> Vec4u;
}
}

	void cloth::checkSuccessImpl(HRESULT err, const char* file, const int line)
	{
		if (err != S_OK)
		{
			NV_CLOTH_LOG_ERROR("direct compute error: %u at %s:%d", err, file, line);
		}
	}

	namespace
	{
		// returns max threads as specified by launch bounds in DxSolverKernel.hlsl
		uint32_t getMaxThreadsPerBlock()
		{
			return 1024;
		}
	}

	cloth::DxFactory::DxFactory(DxContextManagerCallback* contextManager)
		: mContextManager(contextManager)
		, mStagingBuffer(0)
		, mSolverKernelComputeShader(nullptr)
		, mNumThreadsPerBlock(getMaxThreadsPerBlock())
		, mMaxThreadsPerBlock(mNumThreadsPerBlock)
		, mConstraints(mContextManager)
		, mConstraintsHostCopy(mContextManager, DxStagingBufferPolicy())
		, mStiffnessValues(mContextManager)
		, mTethers(mContextManager)
		, mParticles(mContextManager)
		, mParticlesHostCopy(mContextManager, DxStagingBufferPolicy())
		, mParticleAccelerations(mContextManager)
		, mParticleAccelerationsHostCopy(mContextManager, DxStagingBufferPolicy())
		, mPhaseConfigs(mContextManager)
		, mCapsuleIndices(mContextManager, DxStagingBufferPolicy())
		, mCapsuleIndicesDeviceCopy(mContextManager)
		, mCollisionSpheres(mContextManager, DxStagingBufferPolicy())
		, mCollisionSpheresDeviceCopy(mContextManager)
		, mConvexMasks(mContextManager, DxStagingBufferPolicy())
		, mConvexMasksDeviceCopy(mContextManager)
		, mCollisionPlanes(mContextManager, DxStagingBufferPolicy())
		, mCollisionPlanesDeviceCopy(mContextManager)
		, mCollisionTriangles(mContextManager, DxStagingBufferPolicy())
		, mCollisionTrianglesDeviceCopy(mContextManager)
		, mMotionConstraints(mContextManager)
		, mSeparationConstraints(mContextManager)
		, mRestPositions(mContextManager, DxStagingBufferPolicy())
		, mRestPositionsDeviceCopy(mContextManager)
		, mSelfCollisionIndices(mContextManager)
		, mSelfCollisionParticles(mContextManager)
		, mSelfCollisionData(mContextManager)
	{
		if (mContextManager->synchronizeResources())
		{
			// allow particle interop with other device
			mParticles.mBuffer.mMiscFlag =
				D3D11_RESOURCE_MISC_FLAG(mParticles.mBuffer.mMiscFlag | D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX);
		}
	}

	cloth::DxFactory::~DxFactory()
	{
		DxContextLock(*this);
		NV_CLOTH_ASSERT(("All fabrics created by this factory need to be deleted before this factory is destroyed.",mFabrics.size() == 0));
		if (mStagingBuffer)
			mStagingBuffer->Release();

		if (mSolverKernelComputeShader)
			mSolverKernelComputeShader->Release();
	}

cloth::Fabric* cloth::DxFactory::createFabric(uint32_t numParticles, Range<const uint32_t> phaseIndices,
	Range<const uint32_t> sets, Range<const float> restvalues, Range<const float> stiffnessValues,
	Range<const uint32_t> indices, Range<const uint32_t> anchors,
	Range<const float> tetherLengths, Range<const uint32_t> triangles)
{
	return NV_CLOTH_NEW(DxFabric)(*this, numParticles, phaseIndices, sets, restvalues, stiffnessValues, indices, anchors, tetherLengths, triangles,
		getNextFabricId());
}

cloth::Cloth* cloth::DxFactory::createCloth(Range<const PxVec4> particles, Fabric& fabric)
{
	return NV_CLOTH_NEW(DxClothImpl)(*this, fabric, particles);
}

cloth::Solver* cloth::DxFactory::createSolver()
{
	CompileComputeShaders(); //Make sure our compute shaders are ready
	DxSolver* solver = NV_CLOTH_NEW(DxSolver)(*this);

	if (solver->hasError())
	{
		NV_CLOTH_DELETE(solver);
		return NULL;
	}

	return solver;
}

// DxFactory::clone() implemented in DxClothClone.cpp

void cloth::DxFactory::copyToHost(void* dst, ID3D11Buffer* srcBuffer, uint32_t offset, uint32_t size) const
{
	if (!size)
		return;

	DxContextLock contextLock(*this);

	const_cast<DxFactory*>(this)->reserveStagingBuffer(size);
	CD3D11_BOX box(offset, 0, 0, offset + size, 1, 1);
	mContextManager->getContext()->CopySubresourceRegion(mStagingBuffer, 0, 0, 0, 0, srcBuffer, 0, &box);
	void* mapIt = mapStagingBuffer(D3D11_MAP_READ);
	memcpy(dst, mapIt, size);
	unmapStagingBuffer();
}

void cloth::DxFactory::extractFabricData(const Fabric& fabric, Range<uint32_t> phaseIndices, Range<uint32_t> sets,
	Range<float> restvalues, Range<float> stiffnessValues, Range<uint32_t> indices, Range<uint32_t> anchors,
	Range<float> tetherLengths, Range<uint32_t> triangles) const
{
	DxContextLock contextLock(*this);

	const DxFabric& dxFabric = static_cast<const DxFabric&>(fabric);

	if (!phaseIndices.empty())
	{
		NV_CLOTH_ASSERT(phaseIndices.size() == dxFabric.mPhases.size());
		memcpy(phaseIndices.begin(), dxFabric.mPhases.begin(), phaseIndices.size() * sizeof(uint32_t));
	}

	if (!restvalues.empty())
	{
		NV_CLOTH_ASSERT(restvalues.size() == dxFabric.mConstraints.size());
		Vector<DxConstraint>::Type hostConstraints(restvalues.size());
		copyToHost(hostConstraints.begin(), dxFabric.mConstraints.buffer(), dxFabric.mConstraints.mOffset * sizeof(DxConstraint),
			uint32_t(hostConstraints.size() * sizeof(DxConstraint)));
		for (uint32_t i = 0, n = restvalues.size(); i < n; ++i)
			restvalues[i] = hostConstraints[i].mRestvalue;
	}

	if (!stiffnessValues.empty())
	{
		NV_CLOTH_ASSERT(stiffnessValues.size() == dxFabric.mStiffnessValues.size());
		Vector<float>::Type hostStiffnessValues(stiffnessValues.size());
		copyToHost(hostStiffnessValues.begin(), dxFabric.mStiffnessValues.buffer(), dxFabric.mStiffnessValues.mOffset * sizeof(float),
			uint32_t(hostStiffnessValues.size() * sizeof(float)));
		for (uint32_t i = 0, n = stiffnessValues.size(); i < n; ++i)
			stiffnessValues[i] = hostStiffnessValues[i];
	}

	if (!sets.empty())
	{
		// need to skip copying the first element
		NV_CLOTH_ASSERT(sets.size() == dxFabric.mSets.size());
		memcpy(sets.begin(), dxFabric.mSets.begin(), sets.size() * sizeof(uint32_t));
	}

	if (!indices.empty())
	{
		NV_CLOTH_ASSERT(indices.size() == dxFabric.mConstraints.size()*2);
		Vector<DxConstraint>::Type hostConstraints(dxFabric.mConstraints.size());
		copyToHost(hostConstraints.begin(), dxFabric.mConstraints.buffer(), dxFabric.mConstraints.mOffset * sizeof(DxConstraint),
			uint32_t(hostConstraints.size() * sizeof(DxConstraint)));

		auto cIt = hostConstraints.begin(), cEnd = hostConstraints.end();
		for (uint32_t* iIt = indices.begin(); cIt != cEnd; ++cIt)
		{
			*iIt++ = cIt->mFirstIndex;
			*iIt++ = cIt->mSecondIndex;
		}
	}

	if (!anchors.empty() || !tetherLengths.empty())
	{
		uint32_t numTethers = uint32_t(dxFabric.mTethers.size());
		Vector<DxTether>::Type tethers(numTethers, DxTether(0, 0));
		copyToHost(tethers.begin(), dxFabric.mTethers.buffer(), dxFabric.mTethers.mOffset  * sizeof(DxTether),
			uint32_t(tethers.size() * sizeof(DxTether)));

		NV_CLOTH_ASSERT(anchors.empty() || anchors.size() == tethers.size());
		for (uint32_t i = 0; !anchors.empty(); ++i, anchors.popFront())
			anchors.front() = tethers[i].mAnchor;

		NV_CLOTH_ASSERT(tetherLengths.empty() || tetherLengths.size() == tethers.size());
		for (uint32_t i = 0; !tetherLengths.empty(); ++i, tetherLengths.popFront())
			tetherLengths.front() = tethers[i].mLength * dxFabric.mTetherLengthScale;
	}

	if (!triangles.empty())
	{
		// todo triangles
	}
}


	void cloth::DxFactory::extractCollisionData(const Cloth& cloth, Range<PxVec4> spheres, Range<uint32_t> capsules,
		Range<PxVec4> planes, Range<uint32_t> convexes, Range<PxVec3> triangles) const
	{
		PX_ASSERT(&cloth.getFactory() == this);

		const DxCloth& dxCloth = static_cast<const DxClothImpl&>(cloth).mCloth;

		PX_ASSERT(spheres.empty() || spheres.size() == dxCloth.mStartCollisionSpheres.size());
		PX_ASSERT(capsules.empty() || capsules.size() == dxCloth.mCapsuleIndices.size() * 2);
		PX_ASSERT(planes.empty() || planes.size() == dxCloth.mStartCollisionPlanes.size());
		PX_ASSERT(convexes.empty() || convexes.size() == dxCloth.mConvexMasks.size());
		PX_ASSERT(triangles.empty() || triangles.size() == dxCloth.mStartCollisionTriangles.size());

		// collision spheres are in pinned memory, so memcpy directly
		if (!dxCloth.mStartCollisionSpheres.empty() && !spheres.empty())
		{
			memcpy(spheres.begin(),
				DxCloth::MappedVec4fVectorType(const_cast<DxCloth&>(dxCloth).mStartCollisionSpheres).begin(),
				spheres.size() * sizeof(PxVec4));
		}

		if (!dxCloth.mCapsuleIndices.empty() && !capsules.empty())
		{
			memcpy(capsules.begin(), DxCloth::MappedIndexVectorType(const_cast<DxCloth&>(dxCloth).mCapsuleIndices).begin(),
				capsules.size() * sizeof(uint32_t));
		}

		if (!dxCloth.mStartCollisionPlanes.empty() && !planes.empty())
		{
			memcpy(planes.begin(), DxCloth::MappedVec4fVectorType(const_cast<DxCloth&>(dxCloth).mStartCollisionPlanes).begin(),
				dxCloth.mStartCollisionPlanes.size() * sizeof(PxVec4));
		}

		if (!dxCloth.mConvexMasks.empty() && !convexes.empty())
		{
			memcpy(convexes.begin(), DxCloth::MappedMaskVectorType(const_cast<DxCloth&>(dxCloth).mConvexMasks).begin(),
				dxCloth.mConvexMasks.size() * sizeof(uint32_t));
		}

		if (!dxCloth.mStartCollisionTriangles.empty() && !triangles.empty())
		{
			memcpy(triangles.begin(), DxCloth::MappedVec3fVectorType(const_cast<DxCloth&>(dxCloth).mStartCollisionTriangles).begin(),
				dxCloth.mStartCollisionTriangles.size() * sizeof(PxVec3));
		}
	}

	void cloth::DxFactory::extractMotionConstraints(const Cloth& cloth, Range<PxVec4> destConstraints) const
	{
		PX_ASSERT(&cloth.getFactory() == this);

		const DxCloth& dxCloth = static_cast<const DxClothImpl&>(cloth).mCloth;

		if (dxCloth.mMotionConstraints.mHostCopy.size())
		{
			PX_ASSERT(destConstraints.size() == dxCloth.mMotionConstraints.mHostCopy.size());

			memcpy(destConstraints.begin(), dxCloth.mMotionConstraints.mHostCopy.begin(),
				sizeof(PxVec4) * dxCloth.mMotionConstraints.mHostCopy.size());
		}
		else
		{
			DxContextLock contextLock(*this);

			DxBatchedVector<PxVec4> const& srcConstraints = !dxCloth.mMotionConstraints.mTarget.empty()
				? dxCloth.mMotionConstraints.mTarget
				: dxCloth.mMotionConstraints.mStart;

			PX_ASSERT(destConstraints.size() == srcConstraints.size());
			copyToHost(destConstraints.begin(), srcConstraints.buffer(), 0, destConstraints.size() * sizeof(PxVec4));
		}
	}

	void cloth::DxFactory::extractSeparationConstraints(const Cloth& cloth, Range<PxVec4> destConstraints) const
	{
		PX_ASSERT(&cloth.getFactory() == this);

		const DxCloth& dxCloth = static_cast<const DxClothImpl&>(cloth).mCloth;

		if (dxCloth.mSeparationConstraints.mHostCopy.size())
		{
			PX_ASSERT(destConstraints.size() == dxCloth.mSeparationConstraints.mHostCopy.size());

			memcpy(destConstraints.begin(), dxCloth.mSeparationConstraints.mHostCopy.begin(),
				sizeof(PxVec4) * dxCloth.mSeparationConstraints.mHostCopy.size());
		}
		else
		{
			DxContextLock contextLock(*this);

			DxBatchedVector<PxVec4> const& srcConstraints = !dxCloth.mSeparationConstraints.mTarget.empty()
				? dxCloth.mSeparationConstraints.mTarget
				: dxCloth.mSeparationConstraints.mStart;

			PX_ASSERT(destConstraints.size() == srcConstraints.size());

			copyToHost(destConstraints.begin(), srcConstraints.buffer(), 0, destConstraints.size() * sizeof(PxVec4));
		}
	}

	void cloth::DxFactory::extractParticleAccelerations(const Cloth& cloth, Range<PxVec4> destAccelerations) const
	{
		/*
		PX_ASSERT(&cloth.getFactory() == this);
		const DxCloth& dxCloth = static_cast<const DxClothImpl&>(cloth).mCloth;

		if (dxCloth.mParticleAccelerationsHostCopy.size())
		{
			PX_ASSERT(dxCloth.mParticleAccelerationsHostCopy.size());

			memcpy(destAccelerations.begin(), dxCloth.mParticleAccelerationsHostCopy.begin(),
				sizeof(PxVec4) * dxCloth.mParticleAccelerationsHostCopy.size());
		}
		else
		{
			DxContextLock contextLock(*this);

			DxBatchedVector<PxVec4> const& srcAccelerations = dxCloth.mParticleAccelerations;

			PX_ASSERT(destAccelerations.size() == srcAccelerations.size());

			copyToHost(destAccelerations.begin(), srcAccelerations.buffer(), 0, destAccelerations.size() * sizeof(PxVec4));
		}
		*/
		PX_UNUSED(&cloth);
		PX_UNUSED(&destAccelerations);
		PX_ASSERT(0);
	}

	void cloth::DxFactory::extractVirtualParticles(const Cloth& cloth, Range<uint32_t[4]> destIndices,
		Range<PxVec3> destWeights) const
	{
		PX_ASSERT(&cloth.getFactory() == this);

		DxContextLock contextLock(*this);

		const DxCloth& dxCloth = static_cast<const DxClothImpl&>(cloth).mCloth;

		if (destWeights.size() > 0)
		{
			uint32_t numWeights = cloth.getNumVirtualParticleWeights();

			Vector<PxVec4>::Type hostWeights(numWeights, PxVec4(0.0f));
			copyToHost(hostWeights.begin(), dxCloth.mVirtualParticleWeights.mBuffer.mBuffer, 0,
				hostWeights.size() * sizeof(PxVec4));

			// convert weights to Vec3f
			PxVec3* destIt = reinterpret_cast<PxVec3*>(destWeights.begin());
			Vector<PxVec4>::Type::ConstIterator srcIt = hostWeights.begin();
			Vector<PxVec4>::Type::ConstIterator srcEnd = srcIt + numWeights;
			for (; srcIt != srcEnd; ++srcIt, ++destIt)
				*destIt = reinterpret_cast<const PxVec3&>(*srcIt);

			PX_ASSERT(destIt <= destWeights.end());
		}

		if (destIndices.size() > 0)
		{
			uint32_t numIndices = cloth.getNumVirtualParticles();

			Vector<Vec4us>::Type hostIndices(numIndices);
			copyToHost(hostIndices.begin(), dxCloth.mVirtualParticleIndices.mBuffer.mBuffer, 0,
				hostIndices.size() * sizeof(Vec4us));

			// convert indices to 32 bit
			Vec4u* destIt = reinterpret_cast<Vec4u*>(destIndices.begin());
			Vector<Vec4us>::Type::ConstIterator srcIt = hostIndices.begin();
			Vector<Vec4us>::Type::ConstIterator srcEnd = srcIt + numIndices;
			for (; srcIt != srcEnd; ++srcIt, ++destIt)
				*destIt = Vec4u(*srcIt);

			PX_ASSERT(&array(*destIt) <= destIndices.end());
		}
	}

	void cloth::DxFactory::extractSelfCollisionIndices(const Cloth& cloth, Range<uint32_t> destIndices) const
	{
		const DxCloth& dxCloth = static_cast<const DxClothImpl&>(cloth).mCloth;
		PX_ASSERT(destIndices.size() == dxCloth.mSelfCollisionIndices.size());
		intrinsics::memCopy(destIndices.begin(), dxCloth.mSelfCollisionIndicesHost.begin(),
			destIndices.size() * sizeof(uint32_t));
	}

	void cloth::DxFactory::extractRestPositions(const Cloth& cloth, Range<PxVec4> destRestPositions) const
	{
		const DxCloth& dxCloth = static_cast<const DxClothImpl&>(cloth).mCloth;
		PX_ASSERT(destRestPositions.size() == dxCloth.mRestPositions.size());
		intrinsics::memCopy(destRestPositions.begin(), DxCloth::MappedVec4fVectorType(const_cast<DxCloth&>(dxCloth).mRestPositions).begin(),
			destRestPositions.size() * sizeof(PxVec4));
	}

	void cloth::DxFactory::reserveStagingBuffer(uint32_t size)
	{
		if (mStagingBuffer)
		{
			D3D11_BUFFER_DESC desc;
			mStagingBuffer->GetDesc(&desc);
			if (desc.ByteWidth >= size)
				return;
			mStagingBuffer->Release();
		}

		CD3D11_BUFFER_DESC desc(size, 0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE);
		mContextManager->getDevice()->CreateBuffer(&desc, 0, &mStagingBuffer);
	}

	void* cloth::DxFactory::mapStagingBuffer(D3D11_MAP mapType) const
	{
		D3D11_MAPPED_SUBRESOURCE mapped;
		mContextManager->getContext()->Map(mStagingBuffer, 0, mapType, 0, &mapped);
		return mapped.pData;
	}

	void cloth::DxFactory::unmapStagingBuffer() const
	{
		mContextManager->getContext()->Unmap(mStagingBuffer, 0);
	}

	void cloth::DxFactory::CompileComputeShaders()
	{
		if (mSolverKernelComputeShader == nullptr)
		{
			DxContextLock(*this);
			ID3D11Device* device = mContextManager->getDevice();
			device->CreateComputeShader(gDxSolverKernel, sizeof(gDxSolverKernel), NULL, &mSolverKernelComputeShader);
		}
	}

#endif // NV_CLOTH_ENABLE_DX11
