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

#include "CuCheckSuccess.h"
#include "NvCloth/Allocator.h"

// todo: rename file to CuHostAllocator.h

namespace nv
{
namespace cloth
{

template <typename T, unsigned Flags = 0>
class CuHostAllocator
{
public:
	typedef T value_type;

	CuHostAllocator(CUcontext ctx = nullptr)
		: mContext(ctx), mDevicePtr(0)
	{}

	template <class U>
	CuHostAllocator(const CuHostAllocator<U, Flags>& other)
		: mContext(other.mContext), mDevicePtr(0)
	{}

	template <class U>
	struct rebind
	{
		typedef CuHostAllocator<U, Flags> other;
	};

	T* allocate(size_t num, const char*, int)
	{
		void* ptr = nullptr;
		checkSuccess(cuMemHostAlloc(&ptr, num * sizeof(T), Flags));
#if PX_VC
#pragma warning(push)
#pragma warning(disable : 4127) // conditional expression is constant
#endif
		if (Flags & CU_MEMHOSTALLOC_DEVICEMAP)
			checkSuccess(cuMemHostGetDevicePointer(&mDevicePtr, ptr, 0));
#if PX_VC
#pragma warning(pop)
#endif
		return static_cast<T*>(ptr);
	}

	void deallocate(void* ptr)
	{
		checkSuccess(cuMemFreeHost(ptr));
	}

#if PX_VC
	template <class U>
	void construct(U* p, T&& val)
	{
		new (static_cast<void*>(p)) U(std::forward<T>(val));
	}
#else
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;

	template <class U, class... Args>
	void construct(U* p, Args&&... args)
	{
		::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
	}
#endif

	void destroy(T* ptr)
	{
		core::unused(ptr);
		ptr->~T();
	}

	CUcontext mContext;
	CUdeviceptr mDevicePtr; // device pointer of last allocation
};

template <class T1, unsigned Flag1, class T2, unsigned Flag2>
bool operator == (const CuHostAllocator<T1, Flag1>&, const CuHostAllocator<T2, Flag2>&)
{
	return true;
}

template <class T1, unsigned Flag1, class T2, unsigned Flag2>
bool operator!=(const CuHostAllocator<T1, Flag1>&, const CuHostAllocator<T2, Flag2>&)
{
	return false;
}

//Use CuHostVectorImpl instead of physx::shdfnd::Array<T, typename CuHostAllocator<T, Flags>> 
//This entire class is just to make sure that the mDevicePtr from the CuHostAllocator is properly swapped together with mData
template <typename T, unsigned Flags = 0>
class CuHostVectorImpl : public physx::shdfnd::Array<T, typename CuHostAllocator<T, Flags>>
{
	typedef physx::shdfnd::Array<T, typename CuHostAllocator<T, Flags>> Super;
	typedef typename CuHostAllocator<T, Flags> Alloc;
public:
	explicit CuHostVectorImpl(const physx::PxEMPTY v):Super(v){}
	PX_INLINE explicit CuHostVectorImpl(const Alloc& alloc = Alloc()):Super(alloc){}

	PX_INLINE explicit CuHostVectorImpl(uint32_t size, const T& a = T(), const Alloc& alloc = Alloc()):Super(size,a,alloc){}

	template <class A>
	PX_INLINE explicit CuHostVectorImpl(const physx::shdfnd::Array<T, A>& other, const Alloc& alloc = Alloc()):Super(other,alloc){}

	PX_INLINE CuHostVectorImpl(const CuHostVectorImpl& other, const Alloc& alloc = Alloc()):Super(other,alloc){}

	PX_INLINE explicit CuHostVectorImpl(const T* first, const T* last, const Alloc& alloc = Alloc()):Super(first,last,alloc){}

	void swap(physx::shdfnd::Array<T, typename CuHostAllocator<T, Flags>>& other)
	{
		PX_ASSERT(mContext == other.mContext);
		physx::shdfnd::swap(mDevicePtr, other.mDevicePtr);
		Super::swap(other);
	}
};


template <typename T, unsigned Flags = 0>
struct CuHostVector
{
	typedef CuHostVectorImpl<T,Flags> Type;
};

template <typename T, unsigned Flags /*= 0*/>
T* getDevicePointer(nv::cloth::CuHostVectorImpl<T, Flags>& vector)
{
	// cached device pointer only valid if non-empty
	return vector.empty() ? 0 : reinterpret_cast<T*>(vector.getAllocator().mDevicePtr);
}

} // namespace cloth

} // namespace nv

namespace physx
{
namespace shdfnd
{
	//Make sure we call the correct swap function when using the free function swap
	template <class T, unsigned Flags>
	PX_INLINE void swap(nv::cloth::CuHostVectorImpl<T, Flags>& x, nv::cloth::CuHostVectorImpl<T, Flags>& y)
	{
		x.swap(y);
	}
}
}
