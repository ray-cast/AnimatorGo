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

#include "DxSolver.h"
#include "DxCloth.h"
#include "../ClothImpl.h"
#include "DxFabric.h"
#include "DxFactory.h"
#include "DxContextLock.h"
#include "../IterationState.h"
#include <PsSort.h>
#include <foundation/PxProfiler.h>
#include <PsFoundation.h>

#if NV_CLOTH_ENABLE_DX11

#include "DxSolverKernelBlob.h"

using namespace physx;
using namespace nv;

cloth::DxSolver::DxSolver(DxFactory& factory)
: DxContextLock(factory)
, mFactory(factory)
, mFrameDt(0.0f)
, mSharedMemorySize(0)
, mSharedMemoryLimit(0)
, mClothData(mFactory.mContextManager)
, mFrameData(mFactory.mContextManager)
, mFrameDataHostCopy(mFactory.mContextManager, DxStagingBufferPolicy())
, mIterationData(mFactory.mContextManager, DxDynamicBufferPolicy())
, mClothDataDirty(false)
, mKernelSharedMemorySize(0)
, mSyncQuery(0)
, mInterCollisionDistance(0.0f)
, mInterCollisionStiffness(1.0f)
, mInterCollisionIterations(0)
, mInterCollisionFilter(nullptr)
, mInterCollisionScratchMem(NULL)
, mInterCollisionScratchMemSize(0)
, mComputeError(false)
{
	ID3D11Device* device = mFactory.mContextManager->getDevice();
	if (device->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
	{
		D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts = { 0 };
		device->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));
		if (!hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
		{
			NV_CLOTH_LOG_WARNING("DirectCompute is not supported by this device\n");
			mComputeError = true;
		}
	}

	mSharedMemoryLimit = 32 * 1024 - mKernelSharedMemorySize;

	D3D11_QUERY_DESC queryDesc = { D3D11_QUERY_EVENT, 0 };
	device->CreateQuery(&queryDesc, &mSyncQuery);

	DxContextLock::release();

	mSimulateProfileEventData = nullptr;
}

cloth::DxSolver::~DxSolver()
{
	NV_CLOTH_ASSERT(mCloths.empty());

	DxContextLock::acquire();

	if (mSyncQuery)
		mSyncQuery->Release();

	if (mInterCollisionScratchMem)
		NV_CLOTH_FREE(mInterCollisionScratchMem);
}

namespace
{
struct ClothSimCostGreater
{
	bool operator()(const cloth::DxCloth* left, const cloth::DxCloth* right) const
	{
		return left->mNumParticles * left->mSolverFrequency > right->mNumParticles * right->mSolverFrequency;
	}
};
}

void cloth::DxSolver::addCloth(Cloth* cloth)
{
	DxCloth& dxCloth = static_cast<DxClothImpl&>(*cloth).mCloth;

	NV_CLOTH_ASSERT(mCloths.find(&dxCloth) == mCloths.end());

	mCloths.pushBack(&dxCloth);
	// trigger update of mClothData array
	dxCloth.notifyChanged();

	// sort cloth instances by size
	shdfnd::sort(mCloths.begin(), mCloths.size(), ClothSimCostGreater(), NonTrackingAllocator());

	DxContextLock contextLock(mFactory);

	// resize containers and update kernel data
	mClothDataHostCopy.resize(mCloths.size());
	mClothData.resize(mCloths.size());
	mFrameDataHostCopy.resize(mCloths.size());

	// lazy compilation of compute shader
	mComputeError |= mFactory.mSolverKernelComputeShader == nullptr;
#if 0
	if (!mSortComputeShader && !mComputeError)
	{
		ID3D11Device* device = mFactory.mContextManager->getDevice();
		device->CreateComputeShader(gDxSortKernel, sizeof(gDxSortKernel), NULL, &mSortComputeShader);
		mComputeError |= mSortComputeShader == NULL;
		if (mSortComputeShader)
		{
			const uint32_t SortMaxN = 10000;
			ID3D11DeviceContext* context = mFactory.mContextManager->getContext();


			DxDeviceVector<DxSortData> _SortData(mFactory.mContextManager);
			Vector<DxSortData>::Type _SortDataHostCopy;
			_SortData.resize(1);
			_SortDataHostCopy.resize(1);

			DxDeviceVector<uint32_t> _SortElems(mFactory.mContextManager);
			DxDeviceVector<uint32_t> _SortElemsHostCopy(mFactory.mContextManager, DxStagingBufferPolicy());
			Vector<uint32_t>::Type _SortElemsRef;
			_SortElems.reserve(SortMaxN * 2);
			_SortElemsHostCopy.reserve(SortMaxN * 2);
			_SortElemsRef.reserve(SortMaxN);

			context->CSSetShader(mSortComputeShader, NULL, 0);

			//for (uint32_t n = 1; n < SortMaxN; ++n)
			uint32_t n = 2000;
			{
				_SortDataHostCopy[0].mCount = n;
				_SortData = _SortDataHostCopy;

				_SortElems.resize(n * 2);
				_SortElemsHostCopy.resize(n * 2);
				_SortElemsRef.resize(n);

				srand(95123);
				uint32_t* sortElems = _SortElemsHostCopy.map(D3D11_MAP_WRITE);
				for (uint32_t i = 0; i < n; ++i)
				{
					_SortElemsRef[i] = ((uint32_t(rand()) ) << 16);
					sortElems[i] = _SortElemsRef[i] | i;
					sortElems[i + n] = 0x89abcdef;
				}
				_SortElemsHostCopy.unmap();
				std::sort(_SortElemsRef.begin(), _SortElemsRef.end());

				_SortElems = _SortElemsHostCopy;

				ID3D11ShaderResourceView* resourceViews[1] = { _SortData.mBuffer.resourceView() };
				context->CSSetShaderResources(0, 1, resourceViews);

				ID3D11UnorderedAccessView* accessViews[1] = { _SortElems.mBuffer.accessView() };
				context->CSSetUnorderedAccessViews(0, 1, accessViews, NULL);

				context->Dispatch(1, 1, 1);

				_SortElemsHostCopy = _SortElems;

				sortElems = _SortElemsHostCopy.map(D3D11_MAP_READ);
				for (uint32_t i = 0; i < n; ++i)
				{
					uint32_t key = sortElems[i] & ~0xffff;
					uint32_t keyRef = _SortElemsRef[i] & ~0xffff;
					PX_ASSERT(key == keyRef);
				}
				_SortElemsHostCopy.unmap();
			}

			context->CSSetShader(NULL, NULL, 0);
		}
	}
#endif
}

void cloth::DxSolver::removeCloth(Cloth* cloth)
{
	DxCloth& cuCloth = static_cast<DxClothImpl&>(*cloth).mCloth;

	ClothVector::Iterator begin = mCloths.begin(), end = mCloths.end();
	ClothVector::Iterator it = mCloths.find(&cuCloth);

	if (it == end)
		return; // not found

	uint32_t index = uint32_t(it - begin);

	mCloths.remove(index);
	mClothDataHostCopy.remove(index);
	mClothData.resize(mCloths.size());
	mClothDataDirty = true;
}

bool cloth::DxSolver::beginSimulation(float dt)
{
	if (mCloths.empty())
		return false;
	mFrameDt = dt;
	DxSolver::beginFrame();
	return true;
}
void cloth::DxSolver::simulateChunk(int idx)
{
	PX_UNUSED(idx);
	NV_CLOTH_ASSERT(!mCloths.empty());
	NV_CLOTH_ASSERT(idx == 0);
	DxSolver::executeKernel();
}
void cloth::DxSolver::endSimulation()
{
	NV_CLOTH_ASSERT(!mCloths.empty());
	DxSolver::endFrame();
}
int cloth::DxSolver::getSimulationChunkCount() const
{
	return 1;
}

void cloth::DxSolver::beginFrame()
{
	DxContextLock contextLock(mFactory);

	mSimulateProfileEventData = NV_CLOTH_PROFILE_START_CROSSTHREAD("cloth::DxSolver::simulate", 0);
	/*
	ID3DUserDefinedAnnotation* annotation;
	mFactory.mContextManager->getContext()->QueryInterface(&annotation);
	annotation->BeginEvent(L"cloth::DxSolver::simulate");
	annotation->Release();
	*/

	mIterationDataHostCopy.resize(0);

	// update cloth data
	ClothVector::Iterator cIt, cEnd = mCloths.end();
	Vector<DxClothData>::Type::Iterator dIt = mClothDataHostCopy.begin();
	for (cIt = mCloths.begin(); cIt != cEnd; ++cIt, ++dIt)
		mClothDataDirty |= (*cIt)->updateClothData(*dIt);

	uint32_t maxSharedMemorySize = 0;
	DxFrameData* frameDataIt = mFrameDataHostCopy.map(D3D11_MAP_WRITE);
	for (cIt = mCloths.begin(); cIt != cEnd; ++cIt)
	{
		DxCloth& cloth = **cIt;

		uint32_t sharedMemorySize = cloth.mSharedMemorySize;
		uint32_t positionsSize = cloth.mNumParticles * sizeof(PxVec4);

		uint32_t numSharedPositions = std::min(2u, (mSharedMemoryLimit - sharedMemorySize) / positionsSize);

		maxSharedMemorySize = std::max(maxSharedMemorySize, sharedMemorySize + numSharedPositions * positionsSize);

		IterationStateFactory factory(cloth, mFrameDt);
		IterationState<Simd4f> state = factory.create<Simd4f>(cloth);

		*(frameDataIt++) = DxFrameData(cloth, numSharedPositions, state, mIterationDataHostCopy.size());

		while (state.mRemainingIterations)
		{
			mIterationDataHostCopy.pushBack(DxIterationData(state));
			state.update();
		}

		if (cloth.mDeviceParticlesDirty)
			cloth.mParticles = cloth.mParticlesHostCopy;

		// copy to device
		cloth.mParticleAccelerations = cloth.mParticleAccelerationsHostCopy;
		if (!cloth.mMotionConstraints.mHostCopy.empty())
		{
			(cloth.mMotionConstraints.mTarget.empty() ? cloth.mMotionConstraints.mStart : cloth.mMotionConstraints.mTarget) = cloth.mMotionConstraints.mHostCopy;
		}
		if (!cloth.mSeparationConstraints.mHostCopy.empty())
		{
			(cloth.mSeparationConstraints.mTarget.empty() ? cloth.mSeparationConstraints.mStart : cloth.mSeparationConstraints.mTarget) = cloth.mSeparationConstraints.mHostCopy;
		}
	}
	mFrameDataHostCopy.unmap();
	mSharedMemorySize = maxSharedMemorySize;

	mFrameData = mFrameDataHostCopy;
	mIterationData = mIterationDataHostCopy;

	mFactory.mCapsuleIndicesDeviceCopy = mFactory.mCapsuleIndices.mBuffer;
	mFactory.mCollisionSpheresDeviceCopy = mFactory.mCollisionSpheres.mBuffer;
	mFactory.mConvexMasksDeviceCopy = mFactory.mConvexMasks.mBuffer;
	mFactory.mCollisionPlanesDeviceCopy = mFactory.mCollisionPlanes.mBuffer;
	mFactory.mCollisionTrianglesDeviceCopy = mFactory.mCollisionTriangles.mBuffer;
//	mFactory.mParticleAccelerations = mFactory.mParticleAccelerationsHostCopy;
	mFactory.mRestPositionsDeviceCopy = mFactory.mRestPositions.mBuffer;
}

void cloth::DxSolver::executeKernel()
{
	DxContextLock contextLock(mFactory);

	if (mClothDataDirty)
	{
		NV_CLOTH_ASSERT(mClothDataHostCopy.size() == mClothData.size());
		mClothData = mClothDataHostCopy;
		mClothDataDirty = false;
	}

	ID3D11DeviceContext* context = mFactory.mContextManager->getContext();
	{
		context->CSSetShader(mFactory.mSolverKernelComputeShader, NULL, 0);

		ID3D11ShaderResourceView* resourceViews[17] = {
			mClothData.mBuffer.resourceView(), /*mFrameData.mBuffer.resourceView()*/NULL,
			mIterationData.mBuffer.resourceView(), mFactory.mPhaseConfigs.mBuffer.resourceView(),
			mFactory.mConstraints.mBuffer.resourceView(), mFactory.mTethers.mBuffer.resourceView(),
			mFactory.mCapsuleIndicesDeviceCopy.resourceView(), mFactory.mCollisionSpheresDeviceCopy.resourceView(),
			mFactory.mConvexMasksDeviceCopy.resourceView(), mFactory.mCollisionPlanesDeviceCopy.resourceView(),
			mFactory.mCollisionTriangles.mBuffer.resourceView(),
			mFactory.mMotionConstraints.mBuffer.resourceView(),
			mFactory.mSeparationConstraints.mBuffer.resourceView(),
			mFactory.mParticleAccelerations.mBuffer.resourceView(),
			mFactory.mRestPositionsDeviceCopy.resourceView(),
			mFactory.mSelfCollisionIndices.mBuffer.resourceView(),
			mFactory.mStiffnessValues.mBuffer.resourceView()
		};
		context->CSSetShaderResources(0, 17, resourceViews);

		ID3D11UnorderedAccessView* accessViews[4] = {
			mFactory.mParticles.mBuffer.accessView(),
			mFactory.mSelfCollisionParticles.mBuffer.accessView(),
			mFactory.mSelfCollisionData.mBuffer.accessView(),
			mFrameData.mBuffer.accessView()
		};
		context->CSSetUnorderedAccessViews(0, 4, accessViews, NULL);

		context->Dispatch(mCloths.size(), 1, 1);

		context->CSSetShader(NULL, NULL, 0);

		ID3D11ShaderResourceView* resourceViewsNULL[17] = {
			NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
		};
		context->CSSetShaderResources(0, 17, resourceViewsNULL);
		ID3D11UnorderedAccessView* accessViewsNULL[4] = { NULL, NULL, NULL, NULL };
		context->CSSetUnorderedAccessViews(0, 4, accessViewsNULL, NULL);
#if 0
		if (!mCloths[0]->mSelfCollisionData.empty())
		{
			DxDeviceVector<PxVec4> _ParticlesHostCopy(mFactory.mContextManager, DxStagingBufferPolicy());
			DxDeviceVector<uint32_t> _SortDataHostCopy(mFactory.mContextManager, DxStagingBufferPolicy());
			_ParticlesHostCopy.resize(mCloths[0]->mParticles.mSize);
			_SortDataHostCopy.resize(mCloths[0]->mSelfCollisionData.mSize);
			{
				CD3D11_BOX box(mCloths[0]->mParticles.mOffset * sizeof(PxVec4), 0, 0, (mCloths[0]->mParticles.mOffset + mCloths[0]->mParticles.mSize) * sizeof(PxVec4), 1, 1);
				context->CopySubresourceRegion(_ParticlesHostCopy.mBuffer.mBuffer, 0, 0, 0, 0, mCloths[0]->mParticles.buffer(), 0, &box);
			}
			{
				CD3D11_BOX box(mCloths[0]->mSelfCollisionData.mOffset * sizeof(uint32_t), 0, 0, (mCloths[0]->mSelfCollisionData.mOffset + mCloths[0]->mSelfCollisionData.mSize) * sizeof(uint32_t), 1, 1);
				context->CopySubresourceRegion(_SortDataHostCopy.mBuffer.mBuffer, 0, 0, 0, 0, mCloths[0]->mSelfCollisionData.buffer(), 0, &box);
			}

			PxVec4* particles = mCloths[0]->mParticlesHostCopy.map(D3D11_MAP_READ);
			PX_UNUSED(particles);
			uint32_t* sortData = _SortDataHostCopy.map(D3D11_MAP_READ);
			PX_UNUSED(sortData);
			_SortDataHostCopy.unmap();
			mCloths[0]->mParticlesHostCopy.unmap();
		}
#endif
	}

	// copy particle data from device to host
	for (auto it : mCloths)
	{
		it->mParticlesHostCopy = it->mParticles;
	}

	mFrameDataHostCopy = mFrameData;

#if PX_DEBUG
	// cpu synchronization
	context->End(mSyncQuery);
	while (context->GetData(mSyncQuery, nullptr, 0, 0));
#endif
}

void cloth::DxSolver::endFrame()
{
	DxContextLock contextLock(mFactory);

	DxFrameData* fIt = mFrameDataHostCopy.map(D3D11_MAP_READ);
	ClothVector::Iterator cIt, cEnd = mCloths.end();
	for (cIt = mCloths.begin(); cIt != cEnd; ++cIt, ++fIt)
	{
		DxCloth& cloth = **cIt;

		cloth.mHostParticlesDirty = false;
		cloth.mDeviceParticlesDirty = false;

		cloth.mMotionConstraints.pop();
		// don't clear host copy because nothing is being uploaded yet
		// cloth.mMotionConstraints.mHostCopy.resize(0);

		cloth.mSeparationConstraints.pop();
		// don't clear host copy because nothing is being uploaded yet
		// cloth.mSeparationConstraints.mHostCopy.resize(0);

		if (!cloth.mTargetCollisionSpheres.empty())
		{
			shdfnd::swap(cloth.mStartCollisionSpheres, cloth.mTargetCollisionSpheres);
			cloth.mTargetCollisionSpheres.resize(0);
		}

		if (!cloth.mTargetCollisionPlanes.empty())
		{
			shdfnd::swap(cloth.mStartCollisionPlanes, cloth.mTargetCollisionPlanes);
			cloth.mTargetCollisionPlanes.resize(0);
		}

		if (!cloth.mTargetCollisionTriangles.empty())
		{
			shdfnd::swap(cloth.mStartCollisionTriangles, cloth.mTargetCollisionTriangles);
			cloth.mTargetCollisionTriangles.resize(0);
		}

		for (uint32_t i = 0; i < 3; ++i)
		{
			float upper = fIt->mParticleBounds[i * 2 + 0];
			float negativeLower = fIt->mParticleBounds[i * 2 + 1];
			cloth.mParticleBoundsCenter[i] = (upper - negativeLower) * 0.5f;
			cloth.mParticleBoundsHalfExtent[i] = (upper + negativeLower) * 0.5f;
		}

		cloth.mSleepPassCounter = fIt->mSleepPassCounter;
		cloth.mSleepTestCounter = fIt->mSleepTestCounter;
	}
	mFrameDataHostCopy.unmap();

	interCollision();

	/*
	ID3DUserDefinedAnnotation* annotation;
	mFactory.mContextManager->getContext()->QueryInterface(&annotation);
	annotation->EndEvent();
	annotation->Release();
	*/
	NV_CLOTH_PROFILE_STOP_CROSSTHREAD(mSimulateProfileEventData,"cloth::DxSolver::simulate", 0);
}

void cloth::DxSolver::interCollision()
{
	if (!mInterCollisionIterations || mInterCollisionDistance == 0.0f)
		return;
	if (mInterCollisionFilter == nullptr)
	{
		NV_CLOTH_LOG_WARNING("Inter collision will not work unless an inter collision filter is set using Solver::setInterCollisionFilter.");
		return;
	}

	typedef SwInterCollision<Simd4f> SwInterCollision;

	// rebuild cloth instance array
	mInterCollisionInstances.resize(0);
	DxFrameData* frameData = mFrameDataHostCopy.map(D3D11_MAP_READ);
	for (uint32_t i = 0, n = mCloths.size(); i < n; ++i)
	{
		DxCloth& cloth = *mCloths[i];

		cloth.mapParticles();
		float elasticity = 1.0f / frameData[i].mNumIterations;
		NV_CLOTH_ASSERT(!cloth.mHostParticlesDirty);
		PxVec4* particles = cloth.mParticlesMapPointer;
		uint32_t* indices = NULL, numIndices = cloth.mNumParticles;
		if (!cloth.mSelfCollisionIndices.empty())
		{
			indices = cloth.mSelfCollisionIndicesHost.begin();
			numIndices = uint32_t(cloth.mSelfCollisionIndices.size());
		}

		mInterCollisionInstances.pushBack(SwInterCollisionData(
		    particles, particles + cloth.mNumParticles, numIndices, indices, cloth.mTargetMotion,
		    cloth.mParticleBoundsCenter, cloth.mParticleBoundsHalfExtent, elasticity, cloth.mUserData));

		cloth.mDeviceParticlesDirty = true;
	}
	mFrameDataHostCopy.unmap();

	uint32_t requiredTempMemorySize = uint32_t(
	    SwInterCollision::estimateTemporaryMemory(&mInterCollisionInstances[0], mInterCollisionInstances.size()));

	// realloc temp memory if necessary
	if (mInterCollisionScratchMemSize < requiredTempMemorySize)
	{
		if (mInterCollisionScratchMem)
			NV_CLOTH_FREE(mInterCollisionScratchMem);

		mInterCollisionScratchMem = NV_CLOTH_ALLOC(requiredTempMemorySize, "cloth::SwSolver::mInterCollisionScratchMem");
		mInterCollisionScratchMemSize = requiredTempMemorySize;
	}

	SwKernelAllocator allocator(mInterCollisionScratchMem, mInterCollisionScratchMemSize);

	// run inter-collision
	SwInterCollision(mInterCollisionInstances.begin(), mInterCollisionInstances.size(), mInterCollisionDistance,
	                 mInterCollisionStiffness, mInterCollisionIterations, mInterCollisionFilter, allocator)();

	for (uint32_t i = 0, n = mCloths.size(); i < n; ++i)
		mCloths[i]->unmapParticles();
}

#endif // NV_CLOTH_ENABLE_DX11
