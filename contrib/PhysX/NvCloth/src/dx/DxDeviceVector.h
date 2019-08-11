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

#include "NvCloth/DxContextManagerCallback.h"
#include "DxCheckSuccess.h"
#include "NvCloth/Allocator.h"

namespace nv
{
namespace cloth
{

struct DxBufferFlags
{
	D3D11_USAGE mUsage;
	D3D11_BIND_FLAG mBindFlag;
	D3D11_RESOURCE_MISC_FLAG mMiscFlag;
	D3D11_CPU_ACCESS_FLAG mCpuAccessFlag;

	bool isRawBuffer() { return (mMiscFlag & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS) != 0; }
};

inline DxBufferFlags DxDefaultBufferPolicy()
{
	DxBufferFlags result = { D3D11_USAGE_DEFAULT,
		                     D3D11_BIND_FLAG(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS),
		                     D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		                     D3D11_CPU_ACCESS_FLAG(0) };
	return result;
};

inline DxBufferFlags DxDefaultRawBufferPolicy()
{
	DxBufferFlags result = {D3D11_USAGE_DEFAULT,
		D3D11_BIND_FLAG(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS),
		D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS,
		D3D11_CPU_ACCESS_FLAG(0)};
	return result;
};

inline DxBufferFlags DxDynamicBufferPolicy()
{
	DxBufferFlags result = { D3D11_USAGE_DYNAMIC,                   D3D11_BIND_SHADER_RESOURCE,
		                     D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, D3D11_CPU_ACCESS_WRITE };
	return result;
};

inline DxBufferFlags DxStagingBufferPolicy()
{
	DxBufferFlags result = { D3D11_USAGE_STAGING,
		                     D3D11_BIND_FLAG(0),
		                     D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		                     D3D11_CPU_ACCESS_FLAG(D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ) };
	return result;
};

template <typename T>
class DxBuffer : public DxBufferFlags
{
	static const uint32_t SizeOfT = sizeof(T);

  public:
	DxBuffer(DxContextManagerCallback* manager, const DxBufferFlags& flags = DxDefaultBufferPolicy())
	: DxBufferFlags(flags), mCapacity(0), mBuffer(0), mManager(manager), mResourceView(nullptr), mAccessView(nullptr), mAccessRawView(nullptr)
	{
	}

	DxBuffer(const T* first, const T* last, DxContextManagerCallback* manager,
	         const DxBufferFlags& flags = DxDefaultBufferPolicy())
	: DxBufferFlags(flags), mCapacity(0), mBuffer(0), mManager(manager), mResourceView(nullptr), mAccessView(nullptr), mAccessRawView(nullptr)
	{
		D3D11_SUBRESOURCE_DATA data = { first };
		create(uint32_t(last - first), &data);
	}

	DxBuffer(const DxBuffer& other)
	: DxBufferFlags(other), mCapacity(0), mBuffer(0), mManager(other.mManager), mResourceView(nullptr), mAccessView(nullptr), mAccessRawView(nullptr)
	{
		operator=(other);
	}

	~DxBuffer()
	{
		if (mAccessView)
			mAccessView->Release();
		if(mAccessRawView)
			mAccessRawView->Release();
		if (mResourceView)
			mResourceView->Release();
		if (mBuffer)
			mBuffer->Release();
	}

	DxBuffer& operator = (const DxBuffer& other)
	{
		if (mCapacity < other.mCapacity)
		{
			if (mBuffer)
				mBuffer->Release();

			create(other.mCapacity);
		}

		if (other.mCapacity)
		{
			CD3D11_BOX box(0, 0, 0, other.mCapacity * SizeOfT, 1, 1);
			context()->CopySubresourceRegion(mBuffer, 0, 0, 0, 0, other.mBuffer, 0, &box);
		}

		return *this;
	}

	ID3D11DeviceContext* context() const
	{
		return mManager->getContext();
	}

	void reserve(uint32_t n)
	{
		if (n <= mCapacity)
			return;

		ID3D11Buffer* oldBuffer = mBuffer;
		CD3D11_BOX box(0, 0, 0, mCapacity * SizeOfT, 1, 1);

		create(n);

		if (oldBuffer)
		{
			context()->CopySubresourceRegion(mBuffer, 0, 0, 0, 0, oldBuffer, 0, &box);
			oldBuffer->Release();
		}
	}

	T* map(D3D11_MAP mapType)
	{
		D3D11_MAPPED_SUBRESOURCE map;
		checkSuccess(context()->Map(mBuffer, 0, mapType, 0, &map));
		return (T*)map.pData;
	}

	void unmap()
	{
		context()->Unmap(mBuffer, 0);
	}

	ID3D11ShaderResourceView* resourceView()
	{
		if (!mResourceView && mBuffer)
			mManager->getDevice()->CreateShaderResourceView(mBuffer, NULL, &mResourceView);
		return mResourceView;
	}

	ID3D11UnorderedAccessView* accessView()
	{
		if (!mAccessView && mBuffer)
			mManager->getDevice()->CreateUnorderedAccessView(mBuffer, NULL, &mAccessView);
		return mAccessView;
	}

	ID3D11UnorderedAccessView* accessViewRaw()
	{
		if(!mAccessRawView && mBuffer)
		{
			D3D11_BUFFER_UAV buffuav;
			buffuav.FirstElement = 0;
			buffuav.NumElements = (mCapacity * SizeOfT)>>2;
			buffuav.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

			D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
			desc.Format = DXGI_FORMAT_R32_TYPELESS;
			desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			desc.Buffer = buffuav;
			mManager->getDevice()->CreateUnorderedAccessView(mBuffer, &desc, &mAccessRawView);
		}
		return mAccessRawView;
	}

  private:
	void create(uint32_t capacity, D3D11_SUBRESOURCE_DATA* data = 0)
	{
		CD3D11_BUFFER_DESC desc(capacity * SizeOfT, mBindFlag, mUsage, mCpuAccessFlag, mMiscFlag, isRawBuffer()?0:SizeOfT);
		checkSuccess(mManager->getDevice()->CreateBuffer(&desc, data, &mBuffer));
		mCapacity = capacity;

		if (mResourceView)
			mResourceView->Release();
		mResourceView = nullptr;

		if (mAccessView)
			mAccessView->Release();
		mAccessView = nullptr;

		if(mAccessRawView)
			mAccessRawView->Release();
		mAccessRawView = nullptr;
	}

  public:
	uint32_t mCapacity;
	ID3D11Buffer* mBuffer;
	DxContextManagerCallback* mManager;
	ID3D11ShaderResourceView* mResourceView;
	ID3D11UnorderedAccessView* mAccessView;
	ID3D11UnorderedAccessView* mAccessRawView;
};

// STL-style vector that holds POD types in DX device memory. The interface
// is not complete, add whatever you need from the std::vector interface.
template <typename T>
class DxDeviceVector
{
	static const uint32_t SizeOfT = sizeof(T);

  public:
	DxDeviceVector(DxContextManagerCallback* manager, const DxBufferFlags& flags = DxDefaultBufferPolicy())
	: mBuffer(manager, flags), mSize(0)
	{
	}

	DxDeviceVector(const T* first, const T* last, DxContextManagerCallback* manager,
	               const DxBufferFlags& flags = DxDefaultBufferPolicy())
	: mBuffer(first, last, manager, flags)
	{
	}

	DxDeviceVector(const DxDeviceVector& other) : mBuffer(other.mBuffer), mSize(other.mSize)
	{
	}

	template <typename Alloc>
	DxDeviceVector& operator = (const physx::shdfnd::Array<T, Alloc>& other)
	{
		assign(other.begin(), other.end());
		return *this;
	}

	uint32_t capacity() const
	{
		return mBuffer.mCapacity;
	}
	bool empty() const
	{
		return !mSize;
	}
	uint32_t size() const
	{
		return mSize;
	}

	void reserve(uint32_t n)
	{
		mBuffer.reserve(n);
	}

	void resize(uint32_t n)
	{
		if (mBuffer.mCapacity < n)
			reserve(std::max(n, mBuffer.mCapacity * 2));
		mSize = n;
	}

	void assign(const T* first, const T* last)
	{
		mSize = uint32_t(last - first);

		if (!mSize)
			return;

		if (mSize > mBuffer.mCapacity)
		{
			mBuffer = DxBuffer<T>(first, last, mBuffer.mManager, mBuffer);
		}
		else
		{
			if (mBuffer.mUsage == D3D11_USAGE_DEFAULT)
			{
				CD3D11_BOX box(0, 0, 0, mSize * SizeOfT, 1, 1);
				mBuffer.context()->UpdateSubresource(mBuffer.mBuffer, 0, &box, first, 0, 0);
			}
			else
			{
				memcpy(map(D3D11_MAP_WRITE_DISCARD), first, mSize * SizeOfT);
				unmap();
			}
		}
	}

	void swap(DxDeviceVector& other)
	{
		physx::shdfnd::swap(mBuffer, other.mBuffer);
		physx::shdfnd::swap(mSize, other.mSize);
	}

	T* map(D3D11_MAP mapType)
	{
		return mBuffer.map(mapType);
	}

	void unmap()
	{
		mBuffer.unmap();
	}

	// common interface with DxBatchedVector for DxVectorMap
	DxContextManagerCallback* manager() const
	{
		return mBuffer.mManager;
	}

  public:
	DxBuffer<T> mBuffer;
	uint32_t mSize;
};

template <typename Vector>
class DxVectorMap : DxContextLock
{
	DxVectorMap& operator = (const DxVectorMap&);

  public:
	typedef typename Vector::ValueType ValueType;

	DxVectorMap(Vector& vector, D3D11_MAP mapType = D3D11_MAP_READ_WRITE)
	: DxContextLock(vector.manager()), mVector(vector), mData(mVector.map(mapType))
	{
	}

	~DxVectorMap()
	{
		if (mData)
			mVector.unmap();
	}

	// not actually initializing values!
	void resize(uint32_t size, const ValueType& = ValueType())
	{
		NV_CLOTH_ASSERT(size <= mVector.capacity());
		return mVector.resize(size);
	}

	uint32_t size() const
	{
		return mVector.size();
	}

	ValueType* begin()
	{
		return mData;
	}

	ValueType* end()
	{
		return mData + mVector.mSize;
	}

	ValueType& operator[](uint32_t i)
	{
		return mData[i];
	}

	void pushBack(const ValueType& value)
	{
		NV_CLOTH_ASSERT(mVector.mCapacity > mVector.mSize);
		mData[mVector.mSize++] = value;
	}

	void replaceWithLast(ValueType* it)
	{
		*it = mData[--mVector.mSize];
	}

  private:
	Vector& mVector;
	ValueType* const mData;
};

} // namespace cloth

} // namespace nv

namespace physx
{
namespace shdfnd
{
template <typename T>
void swap(nv::cloth::DxBuffer<T>& left, nv::cloth::DxBuffer<T>& right)
{
	swap(left.mCapacity, right.mCapacity);
	swap(left.mBuffer, right.mBuffer);
	swap(left.mManager, right.mManager);
	swap(left.mResourceView, right.mResourceView);
	swap(left.mAccessView, right.mAccessView);
	swap(left.mAccessRawView, right.mAccessRawView);
}
}
}
