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

#include "CuDevicePointer.h"
#include "PsArray.h"
#include "PsUtilities.h"
#include <algorithm>

namespace nv
{
#if PX_VC
#pragma warning(push)
#pragma warning(disable : 4365) // 'action' : conversion from 'type_1' to 'type_2', signed/unsigned mismatch
#endif

namespace cloth
{

// STL-style vector that holds POD types in CUDA device memory. The interface
// is not complete, add whatever you need from the std::vector interface.
template <typename T>
class CuDeviceVector
{
  public:
	typedef CuDevicePointer<T> iterator;
	typedef CuDevicePointer<const T> const_iterator;

	CuDeviceVector(CUcontext ctx) : mContext(ctx)
	{
		NV_CLOTH_ASSERT(mContext);
	}

	CuDeviceVector(const CuDeviceVector& other) 
		: mContext(other.mContext)
	{
		NV_CLOTH_ASSERT(mContext);
		operator=(other);
	}

	CuDeviceVector(CUcontext ctx, const T* first, const T* last) 
		: mContext(ctx)
	{
		NV_CLOTH_ASSERT(mContext);
		assign(first, last);
	}

	template <typename Alloc>
	CuDeviceVector(const physx::shdfnd::Array<T, Alloc>& other)
	{
		operator=(other);
	}

	~CuDeviceVector()
	{
		checkSuccess(cuMemFree(mFirst.dev()));
	}

	CuDeviceVector& operator = (const CuDeviceVector& other)
	{
		resize(other.size());
		checkSuccess(cuMemcpyDtoD(mFirst.dev(), other.mFirst.dev(), other.size() * sizeof(T)));
		return *this;
	}

	template <typename Alloc>
	CuDeviceVector& operator = (const physx::shdfnd::Array<T, Alloc>& other)
	{
		const T* first = other.empty() ? 0 : &other.front();
		assign(first, first + other.size());
		return *this;
	}

	bool empty() const
	{
		return mLast == mFirst;
	}
	size_t size() const
	{
		return size_t(mLast - mFirst);
	}
	size_t capacity() const
	{
		return mEnd - mFirst;
	}

	iterator begin()
	{
		return mFirst;
	}
	iterator end()
	{
		return mLast;
	}
	const_iterator begin() const
	{
		return mFirst;
	}
	const_iterator end() const
	{
		return mLast;
	}

	void push_back(const T& v)
	{
		if (mLast == mEnd)
			reserve(std::max<size_t>(1, capacity() * 2));

		*mLast++ = v;
	}

	void push_back(const T* first, const T* last)
	{
		if (mEnd - mLast < last - first)
			reserve(std::max<size_t>(2 * capacity(), mLast - mFirst + last - first));

		if (first != last)
			checkSuccess(cuMemcpyHtoD(mLast.dev(), first, sizeof(T) * (last - first)));

		mLast += last - first;
	}

	void erase(iterator it)
	{
		size_t byteSize = (mLast - it - 1) * sizeof(T);
		if (byteSize)
		{
			CUdeviceptr tmp = 0, dst = it.dev();
			checkSuccess(cuMemAlloc(&tmp, byteSize));
			checkSuccess(cuMemcpyDtoD(tmp, dst + sizeof(T), byteSize));
			checkSuccess(cuMemcpyDtoD(dst, tmp, byteSize));
			checkSuccess(cuMemFree(tmp));
		}
		--mLast;
	}

	void reserve(size_t n)
	{
		if (n <= capacity())
			return;

		CUdeviceptr newFirst = 0, oldFirst = mFirst.dev();

		checkSuccess(cuMemAlloc(&newFirst, n * sizeof(T)));
		checkSuccess(cuMemcpyDtoD(newFirst, oldFirst, size() * sizeof(T)));
		checkSuccess(cuMemFree(oldFirst));

		iterator first(reinterpret_cast<T*>(newFirst));
		mEnd = first + n;
		mLast = first + size();
		mFirst = first;
	}

	void resize(size_t n)
	{
		if (capacity() < n)
			reserve(std::max(n, capacity() * 2));

		mLast = mFirst + n;
	}

	template <typename Iter>
	void assign(Iter first, Iter last)
	{
		size_t n = last - first;
		resize(n);
		if (n)
			checkSuccess(cuMemcpyHtoD(mFirst.dev(), &*first, n * sizeof(T)));
	}

	void swap(CuDeviceVector& other)
	{
		std::swap(mFirst, other.mFirst);
		std::swap(mLast, other.mLast);
		std::swap(mEnd, other.mEnd);
	}

  private:
	CUcontext const mContext;
	iterator mFirst, mLast, mEnd;
};

} // namespace cloth
} // namespace nv

#if PX_VC
#pragma warning(pop)
#endif

namespace std
{
template <typename T>
void swap(nv::cloth::CuDeviceVector<T>& first, nv::cloth::CuDeviceVector<T>& second)
{
	first.swap(second);
}
}
