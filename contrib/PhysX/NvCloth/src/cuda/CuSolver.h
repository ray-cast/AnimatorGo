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
#include "CuClothData.h"
#include "CuPinnedAllocator.h"
#include "CuContextLock.h"
#include "CuDeviceVector.h"
#include "../SwInterCollision.h"
#include "CuSolverKernel.h"

namespace nv
{

namespace cloth
{

class CuCloth;
class CuFabric;
struct PhaseConfig;
struct CuKernelData;

class CuSolver : private CuContextLock, public Solver
{
	CuSolver(const CuSolver&); // not implemented
	CuSolver& operator = (const CuSolver&); // not implemented

public:
	CuSolver(CuFactory&);
	~CuSolver();

	virtual void addCloth(Cloth*);
	virtual void removeCloth(Cloth*);

	// functions executing the simulation work.
	virtual bool beginSimulation(float dt);
	virtual void simulateChunk(int idx);
	virtual void endSimulation();
	virtual int getSimulationChunkCount() const override;

	virtual bool hasError() const
	{
		return mCudaError;
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
	void updateKernelData(); // context needs to be acquired

	// simulate helper functions
	void beginFrame();
	static void CUDA_CB KernelFinished(CUstream stream, CUresult status, void *userData);
	void executeKernel();
	void endFrame();

	void interCollision();

  private:
	CuFactory& mFactory;

	typedef Vector<CuCloth*>::Type ClothVector;
	ClothVector mCloths;

	CuDeviceVector<CuClothData> mClothData;
	CuHostVector<CuClothData, CU_MEMHOSTALLOC_WRITECOMBINED>::Type mClothDataHostCopy;
	bool mClothDataDirty;

	CuHostVector<CuFrameData, CU_MEMHOSTALLOC_DEVICEMAP>::Type mFrameData;

	CuHostVector<CuIterationData, CU_MEMHOSTALLOC_DEVICEMAP>::Type mIterationData;
	CuIterationData* mIterationDataBegin; // corresponding device ptr

	float mFrameDt;

	uint32_t mSharedMemorySize;
	uint32_t mSharedMemoryLimit;

	CUstream mStream;
	CUfunction mKernelFunction;
	int mKernelSharedMemorySize;
	CuKernelData mKernelDataHost;
	CuDevicePointer<uint32_t> mClothIndex;

	float mInterCollisionDistance;
	float mInterCollisionStiffness;
	uint32_t mInterCollisionIterations;
	InterCollisionFilter mInterCollisionFilter;
	void* mInterCollisionScratchMem;
	uint32_t mInterCollisionScratchMemSize;
	Vector<SwInterCollisionData>::Type mInterCollisionInstances;

	uint64_t mSimulateNvtxRangeId;

	void* mProfileBuffer;
	uint32_t mProfileBaseId;

	bool mCudaError;

	void* mCuptiEventProfiler;

	friend void record(const CuSolver&);

	void* mSimulateProfileEventData;
};
}
}
