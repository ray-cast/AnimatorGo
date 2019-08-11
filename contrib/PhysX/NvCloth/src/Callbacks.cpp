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

#include "NvCloth/Callbacks.h"
#include "NvCloth/Allocator.h"
#include <foundation/PxAllocatorCallback.h>
#include <foundation/PxErrorCallback.h>
#include <stdarg.h>
#include <stdio.h>

using namespace physx;

namespace nv
{
namespace cloth
{

struct NvClothContext
{
	PxAllocatorCallback* mAllocator;
	PxErrorCallback* mErrorCallback;
	PxAssertHandler* mAssertHandler;
	PxProfilerCallback* mProfilerCallback;
};

static NvClothContext sContext;

NV_CLOTH_API(void) InitializeNvCloth(PxAllocatorCallback* allocatorCallback, PxErrorCallback* errorCallback, PxAssertHandler* assertHandler, PxProfilerCallback* profilerCallback, int autoDllIDCheck)
{
	PX_UNUSED(autoDllIDCheck);
	NV_CLOTH_ASSERT_WITH_MESSAGE("NvCloth dll id mismatch, ensure you compile with matching headers/run with matching dll.", NV_CLOTH_DLL_ID == autoDllIDCheck);
	NV_CLOTH_ASSERT_WITH_MESSAGE("NvCloth initialized with invalid allocator", allocatorCallback != nullptr);
	sContext.mAllocator = allocatorCallback;
	sContext.mErrorCallback = errorCallback;
	sContext.mAssertHandler = assertHandler;
	sContext.mProfilerCallback = profilerCallback;
}

}
}

PxAllocatorCallback* GetNvClothAllocator()
{
	NV_CLOTH_ASSERT_WITH_MESSAGE("NvCloth used before calling InitializeNvCloth", nv::cloth::sContext.mAllocator != nullptr);
	return nv::cloth::sContext.mAllocator;
}


namespace nv
{
namespace cloth
{
void LogFn(PxErrorCode::Enum errorLevel, const char* fileName, int lineNumber, const char* msg, va_list additionalArguments)
{
	if (!sContext.mErrorCallback)
		return;
	char buf[2048];
#if PX_VC
	//Pre VC2015 doesn't support the standard library properly.
	_vsnprintf_s(buf, 2048, _TRUNCATE, msg, additionalArguments);
#else
	//This should also work with VC2015
	vsnprintf(buf, 2048, msg, additionalArguments);
#endif
	sContext.mErrorCallback->reportError(errorLevel, buf, fileName, lineNumber);
}
void LogErrorFn(const char* fileName, int lineNumber, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	LogFn(::PxErrorCode::eINTERNAL_ERROR, fileName, lineNumber, msg, args);
	va_end(args);
}
void LogInvalidParameterFn(const char* fileName, int lineNumber, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	LogFn(::PxErrorCode::eINVALID_PARAMETER, fileName, lineNumber, msg, args);
	va_end(args);
}
void LogWarningFn(const char* fileName, int lineNumber, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	LogFn(::PxErrorCode::eDEBUG_WARNING, fileName, lineNumber, msg, args);
	va_end(args);
}
void LogInfoFn(const char* fileName, int lineNumber, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	LogFn(::PxErrorCode::eDEBUG_INFO, fileName, lineNumber, msg, args);
	va_end(args);
}

NV_CLOTH_API(PxAssertHandler*) GetNvClothAssertHandler()
{
	return sContext.mAssertHandler;
}

PxProfilerCallback* GetNvClothProfiler()
{
	return sContext.mProfilerCallback;
}

}//cloth
}//nv
