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

#include "NvCloth/Allocator.h"
#include <algorithm>
#include "NvCloth/Callbacks.h"

namespace nv
{
namespace cloth
{

struct MovingAverage
{
  public:
	MovingAverage(uint32_t n = 1) : mBegin(0), mCount(0), mSize(n)
	{
		mData = reinterpret_cast<float*>(NV_CLOTH_ALLOC(mSize *sizeof(float), "MovingAverage"));
	}
	MovingAverage(const MovingAverage& other): mData(nullptr), mBegin(0), mCount(0), mSize(0)
	{
		*this = other;
	}
	MovingAverage& operator=(const MovingAverage& other)
	{
		mBegin = other.mBegin;
		mCount = other.mCount;
		mSize = other.mSize;
		NV_CLOTH_FREE(mData);
		mData = reinterpret_cast<float*>(NV_CLOTH_ALLOC(mSize * sizeof(float), "MovingAverage"));
		memcpy(mData, other.mData, mSize * sizeof(float));
		return *this;
	}
	~MovingAverage() { NV_CLOTH_FREE(mData); }

	bool empty() const
	{
		return mCount == 0;
	}

	uint32_t size() const
	{
		return mSize;
	}
	
	void resize(uint32_t n)
	{
		float* newData = reinterpret_cast<float*>(NV_CLOTH_ALLOC(n * sizeof(float), "MovingAverage"));

		const int cutOffFront = std::max(mCount - static_cast<int32_t>(n), 0);
		int index = (mBegin + cutOffFront) % mSize;
		for(int i = 0; i < static_cast<int>(n); i++)
		{
			newData[i] = mData[index];
			index = (index + 1) % mSize;
		}

		mCount -= cutOffFront;
			
		NV_CLOTH_FREE(mData);

		mSize = n;
		mData = newData;
		mBegin = 0;
	}
	
	void reset()
	{
		mCount = 0;
		mBegin = 0;
	}

	void push(uint32_t n, float value)
	{
		n = std::min(n, static_cast<uint32_t>(mSize));
		const int start = (mBegin + mCount) % mSize;
		const int end = start + n;
		const int end1 = std::min(end, mSize);
		const int end2 = std::max(end - end1, 0);
		for(int i = start; i < end1; i++)
		{
			mData[i] = value;
		}
		for(int i = 0; i < end2; i++)
		{
			mData[i] = value;
		}

		int newCount = std::min(mCount + static_cast<int32_t>(n), mSize);
		mBegin = (mBegin + n-(newCount-mCount))%mSize; //move mBegin by the amount of replaced elements
		mCount = newCount;
	}

	float average() const
	{
		NV_CLOTH_ASSERT(!empty());

		float sum = 0.0f;
		int totalWeight = 0;
		{
			int count = 0;
			int end = std::min(mBegin + mCount, mSize);
			int rampSize = std::max(1,mCount / 8);
			for(int i = mBegin; i < end; i++)
			{
				//ramp weight /''''''\ . 
				int weight = std::min(
					std::min(count+1, rampSize), //left ramp /'''
					std::min(mCount-(count), rampSize)); //right ramp  '''\ .
				sum += mData[i] * weight;
				totalWeight += weight;
				count++;
			}
			int leftOver = mCount-(end - mBegin);
			for(int i = 0; i < leftOver; i++)
			{
				int weight = std::min(std::min(count + 1, rampSize), std::min(mCount - (count), rampSize));
				sum += mData[i] * weight;
				totalWeight += weight;
				count++;
			}
			NV_CLOTH_ASSERT(count == mCount);
		}

		return sum / static_cast<float>(totalWeight);
	}

private:
	float* mData; //Ring buffer
	int32_t mBegin; //Index to first element
	int32_t mCount; //current number of elements 
	int32_t mSize; //max ringbuffer size
};
}
}
