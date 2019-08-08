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


#include "PsTime.h"
#include "nn/os/os_Tick.h"


namespace physx
{
namespace shdfnd
{

namespace
{
	static double sRecipFrequency = 1.0 / nn::os::GetSystemTickFrequency();
}

static const CounterFrequencyToTensOfNanos gCounterFreq = Time::getCounterFrequency();

const CounterFrequencyToTensOfNanos& Time::getBootCounterFrequency()
{
	return gCounterFreq;
}

CounterFrequencyToTensOfNanos Time::getCounterFrequency()
{
	return CounterFrequencyToTensOfNanos( Time::sNumTensOfNanoSecondsInASecond, static_cast<PxU64>(nn::os::GetSystemTickFrequency()));
}


PxU64 Time::getCurrentCounterValue()
{
	nn::os::Tick tick = nn::os::GetSystemTick();
	return static_cast<PxU64>(tick.GetInt64Value());
}

Time::Time(): mTickCount(0)
{
	getElapsedSeconds();
}

Time::Second Time::getElapsedSeconds()
{
	PxI64 lastTickCount = mTickCount;
	mTickCount = static_cast<PxI64>(nn::os::GetSystemTick().GetInt64Value());
	return (mTickCount - lastTickCount) * sRecipFrequency;
}

Time::Second Time::peekElapsedSeconds()
{
	return (static_cast<PxI64>(nn::os::GetSystemTick().GetInt64Value()) - mTickCount) * sRecipFrequency;
}

Time::Second Time::getLastTime() const
{
	return mTickCount * sRecipFrequency;
}

} // namespace shdfnd
} // namespace physx
