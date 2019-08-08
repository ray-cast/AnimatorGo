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

#ifndef ___PHYS_X_DEVICE_
#define ___PHYS_X_DEVICE_

#include "foundation/PxPreprocessor.h"

#if PX_WINDOWS
#	pragma warning (push)
#	pragma warning (disable : 4668) //'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
#	include "windows/PsWindowsInclude.h"
#	pragma warning (pop)
#endif

#if PX_WINDOWS
	#define PHYSX_DEV_DLL_API extern "C" __declspec(dllexport)
	#ifdef _DEBUG
	#   define PHYSX_DEV_DLL_PRIVATE_API extern "C" __declspec(dllexport)
	#else
	#   define PHYSX_DEV_DLL_PRIVATE_API
	#endif
#else
	#define PHYSX_DEV_DLL_API
	#define PHYSX_DEV_DLL_PRIVATE_API
#endif

/** typedefs */
typedef int PHYSX_DEV_STATUS;
typedef unsigned int PhysXDevHandle;

/** PHYSX_DEV_STATUS values */
enum
{
	PHYSX_DEV_OK = 0,
	PHYSX_DEV_UNKNOWN_ERROR,
	PHYSX_DEV_INVALID_HANDLE,
	PHYSX_DEV_UNINITIALIZED,
	PHYSX_DEV_NV_API_UNAVAILABLE,
	PHYSX_DEV_CUDA_UNAVAILABLE,
	PHYSX_DEV_CUDA_MEMORY_ALLOC_FAILURE,
	PHYSX_DEV_LEGACY_MODE_GPU_HANDLE,
	PHYSX_DEV_PHYSX_DEV_UNAVAILABLE,
};


/**
 *  physxDevInit
 *  Initialize the PhysX Device information functions.
 *  Must be called before using any other API functions.
 */
PHYSX_DEV_DLL_API PHYSX_DEV_STATUS physxDevInit();

/**
 *  physxDevClose
 *  Call this when finished with the PhysX Device API, it
 *  frees memory that is allocated in physxDevInit
 */
PHYSX_DEV_DLL_API PHYSX_DEV_STATUS physxDevClose();

/**
 *  physxDevGetCudaOrdinal
 *  Returns the CUDA device ordinal for the given PhysX GPU device
 */
PHYSX_DEV_DLL_API PHYSX_DEV_STATUS physxDevGetCudaOrdinal(int* cudaDevOrdinal, PhysXDevHandle devHandle);
PHYSX_DEV_STATUS physxDevGetCudaOrdinalWrapper(int* cudaDevOrdinal);

/**
 *  physxDevGet
 *  Returns the PhysX GPU device that the PhysX Engine
 *  will use.  If the device is -1, the engine will
 *  automatically choose which GPU to use.
 *
 *  This function handles the R177/R180 detection first, then decides accordingly
 *
 *  if(180+)
 *      if(GPU Enabled) ? get NVAPI sel : -1
 *  else (177)
 *      if regkey ? regkey value : -1 (PHYSX_DEV_LEGACY_MODE_GPU_HANDLE returned)
 */
PHYSX_DEV_DLL_API PHYSX_DEV_STATUS physxDevGet(PhysXDevHandle* devHandle);

/**
 *  physxDevUsingDedicatedGPU
 *  Returns whether or not PhysX has a dedicated GPU (set by the user in the NV CPL)
 */
PHYSX_DEV_DLL_API bool physxDevUsingDedicatedGPU();

/**
 *  physxDevSLIEnabled
 *  Returns whether or not the device pointer specified (D3D device) is in an SLI group
 */
PHYSX_DEV_DLL_API bool physxDevSLIEnabled(void* graphicsDevice);

#endif
