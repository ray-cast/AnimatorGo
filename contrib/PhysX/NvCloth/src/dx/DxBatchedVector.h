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

#include "DxDeviceVector.h"
#include "NvCloth/Allocator.h"

namespace nv
{
namespace cloth
{

class DxContextManagerCallback;

// forward declarations
template <typename>
class DxBatchedVector;

// array of vectors stored in one dx device buffer
template <typename T>
class DxBatchedStorage
{
	static const uint32_t SizeOfT = sizeof(T);

	friend class DxBatchedVector<T>;

  public:
	DxBatchedStorage(DxContextManagerCallback* manager, const DxBufferFlags& flags = DxDefaultBufferPolicy())
	: mBuffer(manager, flags), mSize(0), mMapRefCount(0), mMapPointer(0)
	{
	}

	~DxBatchedStorage()
	{
		NV_CLOTH_ASSERT(mViews.empty());
		NV_CLOTH_ASSERT(!mMapRefCount);
	}

  private: // functions used by DxBatchedVector
	void add(DxBatchedVector<T>* view)
	{
		mViews.pushBack(view);
		view->mOffset = mSize;
	}

	void reserve(DxBatchedVector<T>* view, uint32_t capacity)
	{
		if (view->mCapacity >= capacity)
			return;
		uint32_t index = view->mOffset + view->mSize;
		uint32_t delta = capacity - view->mCapacity;
		replace(index, index, nullptr, (T*)nullptr + delta);
		update(view, delta);
		view->mCapacity = capacity;
	}

	void assign(DxBatchedVector<T>* view, const T* data, uint32_t newSize)
	{
		uint32_t offset = view->mOffset;
		uint32_t oldSize = std::min(newSize, view->mCapacity);
		replace(offset, offset + oldSize, data, data + newSize);
		update(view, newSize - oldSize);
		view->mSize = newSize;
		if (newSize > view->mCapacity)
			view->mCapacity = newSize;
	}

	void remove(DxBatchedVector<T>* view)
	{
		uint32_t offset = view->mOffset;
		uint32_t capacity = view->mCapacity;
		replace(offset, offset + capacity, nullptr, nullptr);
		update(view, -int32_t(capacity));
		DxBatchedVector<T>** it = mViews.find(view);
		mViews.remove(uint32_t(it - mViews.begin()));
	}

	T* map(D3D11_MAP mapType = D3D11_MAP_READ_WRITE)
	{
		if (!mMapRefCount++)
			mMapPointer = mBuffer.map(mapType);
		return mMapPointer;
	}

	void unmap()
	{
		if (!--mMapRefCount)
		{
			mBuffer.unmap();
			mMapPointer = 0;
		}
	}

  private: // internal functions
	// not updating mSize!
	void replace(uint32_t first, uint32_t last, const T* begin, const T* end)
	{
		uint32_t tail = first + uint32_t(end - begin);
		if (uint32_t newSize = tail == last ? 0 : mSize + tail - last)
		{
			// Optimization: dx11.1 would allow in place copies
			// with ID3D11DeviceContext1::CopySubresourceRegion1

			DxBuffer<T> buffer = DxBuffer<T>(mBuffer.mManager, mBuffer);
			buffer.reserve(newSize);

			if (0 < first)
			{
				NV_CLOTH_ASSERT(!mMapRefCount);
				CD3D11_BOX box(0, 0, 0, first * SizeOfT, 1, 1);
				mBuffer.context()->CopySubresourceRegion(buffer.mBuffer, 0, 0, 0, 0, mBuffer.mBuffer, 0, &box);
			}

			if (last < mSize)
			{
				NV_CLOTH_ASSERT(!mMapRefCount);
				CD3D11_BOX box(last * SizeOfT, 0, 0, mSize * SizeOfT, 1, 1);
				mBuffer.context()->CopySubresourceRegion(buffer.mBuffer, 0, tail * SizeOfT, 0, 0, mBuffer.mBuffer, 0,
				                                         &box);
			}

			physx::shdfnd::swap(mBuffer, buffer);
		}

		if (begin && end > begin)
		{
			if (mBuffer.mUsage == D3D11_USAGE_DEFAULT)
			{
				NV_CLOTH_ASSERT(!mMapRefCount);
				CD3D11_BOX box(first * SizeOfT, 0, 0, tail * SizeOfT, 1, 1);
				mBuffer.context()->UpdateSubresource(mBuffer.mBuffer, 0, &box, begin, 0, 0);
			}
			else
			{
				memcpy(map(D3D11_MAP_WRITE) + first, begin, uint32_t(end - begin) * SizeOfT);
				unmap();
			}
		}
	}

	void update(DxBatchedVector<T>* view, int32_t delta)
	{
		const uint32_t offset = view->mOffset;
		DxBatchedVector<T>** it = mViews.begin();
		for (uint32_t i = mViews.size(); 0 < i--;)
		{
			if (it[i] != view && it[i]->mOffset >= offset)
				it[i]->mOffset += delta;
		}
		mSize += delta;
	}

  public:
	DxBuffer<T> mBuffer;
	uint32_t mSize;
	typename Vector<DxBatchedVector<T>*>::Type mViews;
	uint32_t mMapRefCount;
	T* mMapPointer;
};

template <typename T>
class DxBatchedVector
{
	friend class DxBatchedStorage<T>;

  public:
	typedef T ValueType;

	DxBatchedVector(DxBatchedStorage<T>& array) : mStorage(array), mSize(0), mCapacity(0)
	{
		mStorage.add(this);
	}

	~DxBatchedVector()
	{
		mStorage.remove(this);
	}

	DxBatchedVector(const DxBatchedVector& other) : mStorage(other.mStorage), mSize(0), mCapacity(0)
	{
		mStorage.add(this);
		mStorage.reserve(this, other.mCapacity);

		CD3D11_BOX box(other.mOffset * sizeof(T), 0, 0, (other.mOffset + other.size()) * sizeof(T), 1, 1);
		mStorage.mBuffer.context()->CopySubresourceRegion(buffer(), 0, mOffset * sizeof(T), 0, 0, other.buffer(), 0,
			&box);

		mSize = other.size();
	}

	template <typename Alloc>
	DxBatchedVector& operator = (const physx::shdfnd::Array<T, Alloc>& other)
	{
		assign(other.begin(), other.end());
		return *this;
	}

	DxBatchedVector& operator = (const DxBatchedVector& other)
	{
		NV_CLOTH_ASSERT(mSize == other.size()); // current limitation
		NV_CLOTH_ASSERT(!mStorage.mMapRefCount);

		CD3D11_BOX box(other.mOffset * sizeof(T), 0, 0, (other.mOffset + other.size()) * sizeof(T), 1, 1);
		mStorage.mBuffer.context()->CopySubresourceRegion(buffer(), 0, mOffset * sizeof(T), 0, 0, other.buffer(), 0,
		                                                  &box);

		return *this;
	}

	DxBatchedVector& operator = (const DxDeviceVector<T>& other)
	{
		NV_CLOTH_ASSERT(mSize == other.size()); // current limitation
		NV_CLOTH_ASSERT(!mStorage.mMapRefCount);

		mStorage.mBuffer.context()->CopySubresourceRegion(buffer(), 0, mOffset * sizeof(T), 0, 0, other.buffer(), 0,
		                                                  nullptr);

		return *this;
	}

	uint32_t capacity() const
	{
		return mCapacity;
	}
	bool empty() const
	{
		return !mSize;
	}
	uint32_t size() const
	{
		return mSize;
	}

	void reserve(uint32_t capacity)
	{
		mStorage.reserve(this, capacity);
	}

	void resize(uint32_t size)
	{
		mStorage.assign(this, nullptr, size);
	}

	void assign(const T* first, const T* last)
	{
		mStorage.assign(this, first, uint32_t(last - first));
	}

	// attention: data of this vector starts at mOffset
	ID3D11Buffer* buffer() const
	{
		return mStorage.mBuffer.mBuffer;
	}

	T* map(D3D11_MAP mapType = D3D11_MAP_READ_WRITE)
	{
		return buffer() ? mStorage.map(mapType) + mOffset : 0;
	}

	void unmap()
	{
		mStorage.unmap();
	}

	// common interface with DxDeviceVector for DxVectorMap
	DxContextManagerCallback* manager() const
	{
		return mStorage.mBuffer.mManager;
	}

	void swap(DxBatchedVector<T>& other)
	{
		PX_ASSERT(&mStorage == &other.mStorage);
		physx::shdfnd::swap(mOffset, other.mOffset);
		physx::shdfnd::swap(mSize, other.mSize);
		physx::shdfnd::swap(mCapacity, other.mCapacity);
		// alternative to running through all elements in DxBatchedStorage::update()
		// however, swap should be O(1) and is used more frequently than reserve/add/remove
		// nvidia::swap(*mStorage.mViews.find(&left), *other.mStorage.mViews.find(&right));
	}

	void clear()
	{
		//TODO: make more efficient impl.
		DxBatchedVector<T> temp(mStorage);
		this->swap(temp);
	}

  public:
	DxBatchedStorage<T>& mStorage;
	uint32_t mOffset, mSize, mCapacity;
};

} // namespace cloth

} // namespace nv

namespace physx
{
namespace shdfnd
{
template <typename T>
void swap(nv::cloth::DxBatchedVector<T>& left, nv::cloth::DxBatchedVector<T>& right)
{
	left.swap(right);
}
}
}
