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

#include "task/PxTaskDefine.h"

#if PX_SUPPORT_GPU_PHYSX

#include "foundation/PxErrorCallback.h"

#include "PhysXDeviceSettings.h"
#include "PhysXDevice.h"

#if PX_VC
#pragma warning(disable: 4191)	//'operator/operation' : unsafe conversion from 'type of expression' to 'type required'
#endif

namespace
{
#if PX_WIN32 || PX_WIN64
	/** funcs for the dynamic loading of the PhysXDevice.dll file */
	typedef PHYSX_DEV_STATUS __cdecl physxDevInit_t();
	typedef PHYSX_DEV_STATUS __cdecl physxDevClose_t();
	typedef PHYSX_DEV_STATUS __cdecl physxDevGet_t(PhysXDevHandle* devHandle);
	typedef PHYSX_DEV_STATUS __cdecl physxDevGetCudaOrdinal_t(int* cudaDevOrdinal, PhysXDevHandle devHandle);
	typedef bool __cdecl physxDevUsingDedicatedGPU_t();
	typedef bool __cdecl physxDevSLIEnabled_t(void* graphicsDevice);

	/** globals for cuda functions */
	static physxDevInit_t* physxDevInit_f;
	static physxDevClose_t* physxDevClose_f;
	static physxDevGetCudaOrdinal_t* physxDevGetCudaOrdinal_f;
	static physxDevGet_t* physxDevGet_f;
	static physxDevUsingDedicatedGPU_t* physxDevUsingDedicatedGPU_f;
	static physxDevSLIEnabled_t* physxDevSLIEnabled_f;

	/** globals */
	static HMODULE gPhysXDevModuleH;

	PHYSX_DEV_STATUS initPhysXDeviceLib()
	{
		PHYSX_DEV_STATUS status;
#if PX_X86
		gPhysXDevModuleH = LoadLibrary("PhysXDevice.dll");
#else
		gPhysXDevModuleH = LoadLibrary("PhysXDevice64.dll");
#endif
		if (!gPhysXDevModuleH)
		{
			return PHYSX_DEV_PHYSX_DEV_UNAVAILABLE;
		}

		physxDevInit_f = (physxDevInit_t*)GetProcAddress(gPhysXDevModuleH, "physxDevInit");
		physxDevClose_f = (physxDevClose_t*)GetProcAddress(gPhysXDevModuleH, "physxDevClose");
		physxDevGetCudaOrdinal_f = (physxDevGetCudaOrdinal_t*)GetProcAddress(gPhysXDevModuleH, "physxDevGetCudaOrdinal");
		physxDevGet_f = (physxDevGet_t*)GetProcAddress(gPhysXDevModuleH, "physxDevGet");
		physxDevUsingDedicatedGPU_f = (physxDevUsingDedicatedGPU_t*)GetProcAddress(gPhysXDevModuleH, "physxDevUsingDedicatedGPU");
		physxDevSLIEnabled_f = (physxDevSLIEnabled_t*)GetProcAddress(gPhysXDevModuleH, "physxDevSLIEnabled");

		if (!physxDevInit_f ||
			!physxDevClose_f ||
			!physxDevGetCudaOrdinal_f ||
			!physxDevGet_f)
		{
			FreeLibrary(gPhysXDevModuleH);
			return PHYSX_DEV_CUDA_UNAVAILABLE;
		}

		status = physxDevInit_f();
		if (PHYSX_DEV_OK != status)
		{
			FreeLibrary(gPhysXDevModuleH);
			return status;
		}

		return PHYSX_DEV_OK;
	}
#endif // PX_WIN32 || PX_WIN64

	PHYSX_DEV_STATUS getCudaOrdinal(int* cudaDevOrdinal)
	{
#if PX_WIN32 || PX_WIN64
		PHYSX_DEV_STATUS status;
		PhysXDevHandle selectedDev;

		status = initPhysXDeviceLib();

		if (PHYSX_DEV_OK != status)
		{
			return status;
		}

		status = physxDevGet_f(&selectedDev);
		physxDevGetCudaOrdinal_f(cudaDevOrdinal, selectedDev);

		physxDevClose_f();
		FreeLibrary(gPhysXDevModuleH);

		if (status == PHYSX_DEV_LEGACY_MODE_GPU_HANDLE) // R177 installed
		{
			return PHYSX_DEV_LEGACY_MODE_GPU_HANDLE;
		}
		else
		{
			return PHYSX_DEV_OK;
		}
#elif PX_LINUX
		const char* deviceOrdinalString = ::getenv("PHYSX_GPU_DEVICE");
		if (!deviceOrdinalString)
			*cudaDevOrdinal = 0;
		else
			*cudaDevOrdinal = atoi(deviceOrdinalString);
		return PHYSX_DEV_OK;
#endif
	}

}

namespace physx
{

	int PhysXDeviceSettings::getSuggestedCudaDeviceOrdinal(physx::PxErrorCallback& errc)
	{
		int cudaDevOrdinal = -1;
		switch (getCudaOrdinal(&cudaDevOrdinal))
		{
		case PHYSX_DEV_OK:
			break;

		case PHYSX_DEV_UNKNOWN_ERROR:
			errc.reportError(PxErrorCode::eDEBUG_WARNING, "unknown error during CUDA device detection\n", __FILE__, __LINE__);
			break;

		case PHYSX_DEV_NV_API_UNAVAILABLE:
			errc.reportError(PxErrorCode::eDEBUG_WARNING, "NVAPI is not available\n", __FILE__, __LINE__);
			break;

		case PHYSX_DEV_CUDA_UNAVAILABLE:
			errc.reportError(PxErrorCode::eDEBUG_WARNING, "CUDA is not available\n", __FILE__, __LINE__);
			break;

		case PHYSX_DEV_PHYSX_DEV_UNAVAILABLE:
#if PX_X86
			errc.reportError(PxErrorCode::eDEBUG_WARNING, "PhysXDevice.dll is not available\n", __FILE__, __LINE__);
#else
			errc.reportError(PxErrorCode::eDEBUG_WARNING, "PhysXDevice64.dll is not available\n", __FILE__, __LINE__);
#endif
			break;

		default:
			errc.reportError(PxErrorCode::eDEBUG_WARNING, "unknown error during CUDA device detection\n", __FILE__, __LINE__);
			break;
		}

		return cudaDevOrdinal;
	}

	int PhysXDeviceSettings::isUsingDedicatedGPU()
	{
#if PX_WIN32 || PX_WIN64
		PHYSX_DEV_STATUS status;
		bool dedicated = false;

		status = initPhysXDeviceLib();

		if (PHYSX_DEV_OK != status)
		{
			return 0;
		}

		if (physxDevUsingDedicatedGPU_f)
		{
			dedicated = physxDevUsingDedicatedGPU_f();
			physxDevClose_f();
			FreeLibrary(gPhysXDevModuleH);
			return(dedicated);
		}
		else
		{
			physxDevClose_f();
			FreeLibrary(gPhysXDevModuleH);
			return(-1);
		}
#elif PX_LINUX
		// need some way to set this
		return 0;
#endif
	}

	bool PhysXDeviceSettings::isSLIEnabled(void* graphicsDevice)
	{
#if PX_WIN32 || PX_WIN64
		PHYSX_DEV_STATUS status;
		status = initPhysXDeviceLib();

		if (PHYSX_DEV_OK != status)
		{
			return false;
		}

		if (physxDevSLIEnabled_f)
		{
			bool enabled = physxDevSLIEnabled_f(graphicsDevice);
			physxDevClose_f();
			FreeLibrary(gPhysXDevModuleH);
			return enabled;
		}
		else
		{
			physxDevClose_f();
			FreeLibrary(gPhysXDevModuleH);
			return false;
		}
#elif PX_LINUX
		// Unimplemented for Linux because we don't need it, not because it's really always false.
		PX_UNUSED(graphicsDevice);
		return false;
#endif
	}

} // end physx namespace

#endif // PX_SUPPORT_GPU_PHYSX


