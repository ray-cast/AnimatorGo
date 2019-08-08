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

#include "task/PxGpuDispatcher.h"
#include "cudamanager/PxCudaContextManager.h"

#include "foundation/PxAssert.h"
#include "foundation/PxErrorCallback.h"

#include "PsString.h"

#include "CudaKernelWrangler.h"

#include <cuda.h>
#include <texture_types.h>

/**
 * Workaround hacks for using nvcc --compiler output object files
 * without linking with CUDART.  We must implement our own versions
 * of these functions that the object files are hard-coded to call into.
 */

#define CUT_SAFE_CALL(call)  { CUresult ret = call; if( CUDA_SUCCESS != ret ) { PX_ASSERT(!ret); } }

#define MAX_MODULES					64  // Max number of .cu files you will build
static void* gModuleTable[ MAX_MODULES ];
static int gNumModules = 0;

#define MAX_FUNCTIONS				256 // Max number of kernel of entry points
typedef struct
{
	int moduleIndex;
	const char* functionName;
} cuFuncDesc;
static cuFuncDesc gFunctionTable[ MAX_FUNCTIONS ];
static int gNumFunctions = 0;

using namespace physx::shdfnd;
using namespace physx;
		
KernelWrangler::KernelWrangler(PxGpuDispatcher& gd, PxErrorCallback& errorCallback, const char** funcNames, uint16_t numFuncs)
	: mError(false)
	, mCuFunctions(PX_DEBUG_EXP("CuFunctions"))
	, mCuModules(PX_DEBUG_EXP("CuModules"))
	, mGpuDispatcher(gd)
	, mErrorCallback(errorCallback)
{
	PxScopedCudaLock _lock_(*gd.getCudaContextManager());

	/* Formally load the CUDA modules, get CUmodule handles */
	mCuModules.resize((uint32_t)gNumModules);
	for (int i = 0 ; i < gNumModules ; ++i)
	{
		CUresult ret = cuModuleLoadDataEx(&mCuModules[(uint32_t)i], gModuleTable[i], 0, NULL, NULL);
		if (ret != CUDA_SUCCESS && ret != CUDA_ERROR_NO_BINARY_FOR_GPU)
		{
			mErrorCallback.reportError(PxErrorCode::eINTERNAL_ERROR, "Failed to load CUDA module data.", __FILE__, __LINE__);
			mError = true;
			return;
		}
	}

	/* matchup funcNames to CUDA modules, get CUfunction handles */
	mCuFunctions.resize(numFuncs);
	mCuFuncModIndex.resize(numFuncs);
	for (uint32_t i = 0 ; i < numFuncs ; ++i)
	{
		for (int j = 0; ; ++j)
		{
			if(j == gNumFunctions)
			{
				char buffer[256];
				physx::shdfnd::snprintf(buffer, 256, "Could not find registered CUDA function '%s'.", funcNames[i]);
				mErrorCallback.reportError(PxErrorCode::eINTERNAL_ERROR, buffer, __FILE__, __LINE__);
				mError = true;
				return;
			}

			if (!physx::shdfnd::strcmp(gFunctionTable[j].functionName, funcNames[i]))
			{
				mCuFuncModIndex[i] = (uint16_t)gFunctionTable[j].moduleIndex;
				CUresult ret = cuModuleGetFunction(&mCuFunctions[i], mCuModules[mCuFuncModIndex[i]], funcNames[i]);
				if (ret != CUDA_SUCCESS)
				{
					char buffer[256];
					physx::shdfnd::snprintf(buffer, 256, "Could not find CUDA module containing function '%s'.", funcNames[i]);
					mErrorCallback.reportError(PxErrorCode::eINTERNAL_ERROR, buffer, __FILE__, __LINE__);
					mError = true;
					return;
				}
				break;
			}
		}
	}
}

KernelWrangler::~KernelWrangler()
{
	if (mCuModules.size())
	{
		PxScopedCudaLock _lock_(*mGpuDispatcher.getCudaContextManager());

		for (uint32_t i = 0 ; i < mCuModules.size() ; i++)
			if(mCuModules[i])
				CUT_SAFE_CALL(cuModuleUnload(mCuModules[i]));
	}
}

void const* const* KernelWrangler::getImages()
{
	return gModuleTable;
}

int KernelWrangler::getNumImages()
{
	return gNumModules;
}

/*
 * These calls are all made _before_ main() during static initialization
 * of this DLL.
 */

#include <driver_types.h>

#if PX_WINDOWS_FAMILY
#define CUDARTAPI __stdcall
#endif

struct uint3;
struct dim3;

extern "C"
void** CUDARTAPI __cudaRegisterFatBinary(void* fatBin)
{
	//HACK to get real fatbin in CUDA 4.0
	struct CUIfatbinStruct
	{
		int magic;
		int version;
		void *fatbinArray;
		char *fatbinFile;
	};
	const CUIfatbinStruct *fatbinStruct = (const CUIfatbinStruct *)fatBin;
	if (fatbinStruct->magic == 0x466243B1)
	{
		fatBin = fatbinStruct->fatbinArray;
	}

	if (gNumModules < MAX_MODULES)
	{
		gModuleTable[ gNumModules ] = fatBin;
		return (void**)(size_t) gNumModules++;
	}
	return NULL;
}

extern "C"
void CUDARTAPI __cudaUnregisterFatBinary(void** fatCubinHandle)
{
	gModuleTable[(int)(size_t) fatCubinHandle ] = 0;
}

extern "C"
void CUDARTAPI __cudaRegisterTexture(void**, const struct textureReference*, const void**, const char*, int, int, int)
{
}

extern "C" void CUDARTAPI __cudaRegisterVar(void**, char*, char*, const char*, int, int, int, int)
{
}


extern "C" void CUDARTAPI __cudaRegisterShared(void**, void**)
{
}

extern "C"
void CUDARTAPI __cudaRegisterFunction(void** fatCubinHandle, const char*, 
	char*, const char* deviceName, int, uint3*, uint3*, dim3*, dim3*, int*)
{
	if (gNumFunctions < MAX_FUNCTIONS)
	{
		// We need this association of function to module in order to find textures and globals
		gFunctionTable[ gNumFunctions ].moduleIndex = (int)(size_t) fatCubinHandle;
		gFunctionTable[ gNumFunctions ].functionName = deviceName;
		gNumFunctions++;
	}
}

/* These functions are implemented just to resolve link dependencies */

extern "C"
cudaError_t CUDARTAPI cudaLaunch(const char* entry)
{
	PX_UNUSED(entry);
	return cudaSuccess;
}

extern "C"
cudaError_t CUDARTAPI cudaSetupArgument(const void*, size_t, size_t)
{
	return cudaSuccess;
}

extern "C"
struct cudaChannelFormatDesc CUDARTAPI cudaCreateChannelDesc(
    int x, int y, int z, int w, enum cudaChannelFormatKind f)
{
	struct cudaChannelFormatDesc desc;
	desc.x = x;
	desc.y = y;
	desc.z = z;
	desc.w = w;
	desc.f = f;
	return desc;
}

