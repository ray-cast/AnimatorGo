/*
 * Copyright (c) 2008-2015, NVIDIA CORPORATION.  All rights reserved.
 *
 * NVIDIA CORPORATION and its licensors retain all intellectual property
 * and proprietary rights in and to this software, related documentation
 * and any modifications thereto.  Any use, reproduction, disclosure or
 * distribution of this software and related documentation without an express
 * license agreement from NVIDIA CORPORATION is strictly prohibited.
 */
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  
#include "PsFPU.h"

#include <cfenv>

physx::shdfnd::FPUGuard::FPUGuard()
{
	PX_COMPILE_TIME_ASSERT(sizeof(fenv_t) <= sizeof(mControlWords));

	fegetenv(reinterpret_cast<fenv_t*>(mControlWords));
	fesetenv(FE_DFL_ENV);

	// NX does not seem to support fedisableexcept
	//fedisableexcept(FE_ALL_EXCEPT);

	fesetround(FE_TONEAREST);  // since this does not seem to be the default mode
}

physx::shdfnd::FPUGuard::~FPUGuard()
{
	fesetenv(reinterpret_cast<fenv_t*>(mControlWords));
}

PX_FOUNDATION_API void physx::shdfnd::enableFPExceptions()
{
	// NX does not seem to support feenableexcept
	//feclearexcept(FE_ALL_EXCEPT);
	//feenableexcept(FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);	
}

PX_FOUNDATION_API void physx::shdfnd::disableFPExceptions()
{
	// NX does not seem to support fedisableexcept
	//fedisableexcept(FE_ALL_EXCEPT);
}
