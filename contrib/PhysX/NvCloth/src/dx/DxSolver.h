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

#include "NvCloth/Solver.h"
#include "DxClothData.h"
#include "DxContextLock.h"
#include "DxDeviceVector.h"
#include "../SwInterCollision.h"

struct ID3D11ShaderResourceView;

namespace nv
{
namespace cloth
{

class DxCloth;
class DxFabric;
struct PhaseConfig;
struct DxKernelData;

class DxSolver : private DxContextLock, public Solver
{
	PX_NOCOPY(DxSolver);

  public:
	DxSolver(DxFactory&);
	~DxSolver();

	virtual void addCloth(Cloth*);
	virtual void removeCloth(Cloth*);

	// functions executing the simulation work.
	virtual bool beginSimulation(float dt);
	virtual void simulateChunk(int idx);
	virtual void endSimulation();
	virtual int getSimulationChunkCount() const override;

	virtual bool hasError() const
	{
		return mComputeError;
	}

	virtual void setInterCollisionDistance(float distance)
	{
		mInterCollisionDistance = distance;
	}
	virtual float getInterCollisionDistance() const
	{
		return mInterCollisionDistance;
	}
	virtual void setInterCollisionStiffness(float stiffness)
	{
		mInterCollisionStiffness = stiffness;
	}
	virtual float getInterCollisionStiffness() const
	{
		return mInterCollisionStiffness;
	}
	virtual void setInterCollisionNbIterations(uint32_t nbIterations)
	{
		mInterCollisionIterations = nbIterations;
	}
	virtual uint32_t getInterCollisionNbIterations() const
	{
		return mInterCollisionIterations;
	}
	virtual void setInterCollisionFilter(InterCollisionFilter filter)
	{
		mInterCollisionFilter = filter;
	}

  private:
	// simulate helper functions
	void beginFrame();
	void executeKernel();
	void endFrame();

	void interCollision();

  private:
	DxFactory& mFactory;

	typedef Vector<DxCloth*>::Type ClothVector;
	ClothVector mCloths;

	DxDeviceVector<DxClothData> mClothData;
	Vector<DxClothData>::Type mClothDataHostCopy;
	bool mClothDataDirty;

	DxDeviceVector<DxFrameData> mFrameData;
	DxDeviceVector<DxFrameData> mFrameDataHostCopy;

	DxDeviceVector<DxIterationData> mIterationData;
	Vector<DxIterationData>::Type mIterationDataHostCopy;

	float mFrameDt;

	uint32_t mSharedMemorySize;
	uint32_t mSharedMemoryLimit;

	int mKernelSharedMemorySize;
	ID3D11Query* mSyncQuery;

	float mInterCollisionDistance;
	float mInterCollisionStiffness;
	uint32_t mInterCollisionIterations;
	InterCollisionFilter mInterCollisionFilter;
	void* mInterCollisionScratchMem;
	uint32_t mInterCollisionScratchMemSize;
	Vector<SwInterCollisionData>::Type mInterCollisionInstances;

	bool mComputeError;

	friend void record(const DxSolver&);

	void* mSimulateProfileEventData;
};
}
}
