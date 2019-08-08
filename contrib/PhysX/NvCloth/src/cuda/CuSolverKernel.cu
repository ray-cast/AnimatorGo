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

#include "CuSolverKernel.h"
#include "CuClothData.h"
#include "CuPhaseConfig.h"

#include <new> // placement new

/*
	For detailed comments about the algorithm check SwSolverKernel.cpp (or the documentation)
	The CPU implementation is generally easier to read, and comments are not duplicated in other implementations.
	Only CUDA implementation specific comments are left in this implementation.
*/

#ifndef FLT_EPSILON
#define FLT_EPSILON 1.192092896e-07F
#endif
#ifndef FLT_MAX
#define FLT_MAX 3.402823466e+38F
#endif

// Converting pointers to shared/global addresses is faster than doing generic loads on SM50
#define CONVERT_ADDRESSES (__CUDA_ARCH__ >= 500)

#if !defined(_WIN64) && !defined(__x86_64__)
#define POINTER_CONSTRAINT "r"
#define POINTER_TYPE "u32"
#else
#define POINTER_CONSTRAINT "l"
#define POINTER_TYPE "u64"
#endif

#ifndef __CUDA_ARCH__
#define assert(x)
#endif

extern "C" {
extern _CRTIMP __host__ __device__ int __cdecl printf(const char*, ...);
}

using namespace nv;

// shared memory copy (instead of relying on constant cache)
__shared__ cloth::CuClothData gClothData;
__shared__ cloth::CuFrameData gFrameData;
__shared__ cloth::CuIterationData gIterData;

// Our way to create stream local variables
__shared__ void* gProfileBuffer;
__shared__ uint32_t gProfileBaseId;

static const uint32_t gCuClothDataSize = sizeof(cloth::CuClothData) / sizeof(float);
static const uint32_t gCuFrameDataSize = sizeof(cloth::CuFrameData) / sizeof(float);
static const uint32_t gCuIterationDataSize = sizeof(cloth::CuIterationData) / sizeof(float);
static const uint32_t gCuPhaseConfigSize = sizeof(cloth::CuPhaseConfig) / sizeof(float);

/*
Memory block for all temporary data in shared memory (in 'allocation' order).
The numbers indicate the allocation slot if used a stack allocator.
0) simulate*()::configs (numPhases*sizeof(CuPhaseConfig))
1) simulate*()::particles ({0,1,2}*4*numParticles floats)
2) CuCollision::mCapsuleIndices, mCapsuleMasks, mConvexMasks (numCapsules*4+numConvexes ints)
3) CuCollision::mPrevData (4*numSpheres+10*numCones floats)
4) CuCollision::collideConvexes() (4*numPlanes floats)
4) CuCollision::collideTriangles() (19*numTriangles floats)
4) CuCollision::mCurData::Spheres (4*numSpheres floats)
5) computeParticleBounds()::dst (192 floats written, 208 float read)
5) computeSphereBounds()::dst (192 floats written, 208 floats read)
5) CuCollision::mCurData::Cones (10*numCones floats)
6) CuCollision::mShapeGrid (2*6*sGridSize=96 floats)
4) CuSelfCollision::buildAcceleration()::buffer (34*16=544 ints)
*/
extern __shared__ float gSharedMemory[];
extern __shared__ int32_t gSharedSigned[];
extern __shared__ uint32_t gSharedUnsigned[];

/***************** Pointer Wrappers **********************/
enum AddressSpace
{
	Shared,
	Global
};

template <AddressSpace, typename T>
__device__ T load(const T* ptr);
template <AddressSpace, typename T>
__device__ void store(T* ptr, const T& value);

#if !CONVERT_ADDRESSES
template <AddressSpace, typename T>
__device__ T load(const T* ptr)
{
	return *ptr;
}
template <AddressSpace, typename T>
__device__ void store(T* ptr, const T& value)
{
	*ptr = value;
}
#else
template <>
__device__ float load<Shared>(const float* ptr)
{
	float value;
	asm("ld.shared.f32 %0, [%1];" : "=f"(value) : POINTER_CONSTRAINT(ptr));
	return value;
}
template <>
__device__ int32_t load<Shared>(const int32_t* ptr)
{
	int32_t value;
	asm("ld.shared.s32 %0, [%1];" : "=r"(value) : POINTER_CONSTRAINT(ptr));
	return value;
}
template <>
__device__ uint32_t load<Shared>(const uint32_t* ptr)
{
	uint32_t value;
	asm("ld.shared.u32 %0, [%1];" : "=r"(value) : POINTER_CONSTRAINT(ptr));
	return value;
}
template <>
__device__ void store<Shared>(int32_t* ptr, const int32_t& value)
{
	asm("st.shared.s32 [%0], %1;" : : POINTER_CONSTRAINT(ptr), "r"(value) : "memory");
}
template <>
__device__ void store<Shared>(float* ptr, const float& value)
{
	asm("st.shared.f32 [%0], %1;" : : POINTER_CONSTRAINT(ptr), "f"(value) : "memory");
}
template <>
__device__ void store<Shared>(uint32_t* ptr, const uint32_t& value)
{
	asm("st.shared.u32 [%0], %1;" : : POINTER_CONSTRAINT(ptr), "r"(value) : "memory");
}
template <>
__device__ float load<Global>(const float* ptr)
{
	float value;
	asm("ld.global.f32 %0, [%1];" : "=f"(value) : POINTER_CONSTRAINT(ptr));
	return value;
}
template <>
__device__ int32_t load<Global>(const int32_t* ptr)
{
	int32_t value;
	asm("ld.global.s32 %0, [%1];" : "=r"(value) : POINTER_CONSTRAINT(ptr));
	return value;
}
template <>
__device__ uint32_t load<Global>(const uint32_t* ptr)
{
	uint32_t value;
	asm("ld.global.u32 %0, [%1];" : "=r"(value) : POINTER_CONSTRAINT(ptr));
	return value;
}
template <>
__device__ void store<Global>(int32_t* ptr, const int32_t& value)
{
	asm("st.global.s32 [%0], %1;" : : POINTER_CONSTRAINT(ptr), "r"(value) : "memory");
}
template <>
__device__ void store<Global>(float* ptr, const float& value)
{
	asm("st.global.f32 [%0], %1;" : : POINTER_CONSTRAINT(ptr), "f"(value) : "memory");
}
template <>
__device__ void store<Global>(uint32_t* ptr, const uint32_t& value)
{
	asm("st.global.u32 [%0], %1;" : : POINTER_CONSTRAINT(ptr), "r"(value) : "memory");
}
#endif

template <AddressSpace, typename>
struct Pointer;

template <AddressSpace S, typename T>
struct Reference
{
	template <AddressSpace, typename>
	friend struct Reference;
	friend struct Pointer<S, T>;

	__device__ Reference()
	{
	}
	__device__ Reference(const Reference& other) : mPtr(other.mPtr)
	{
	}
	template <typename U>
	__device__ Reference(const Reference<S, U>& other)
	: mPtr(other.mPtr)
	{
	}

	__device__ Reference& operator = (const Reference& other)
	{
		return *this = static_cast<T>(other);
	}
	template <typename U>
	__device__ Reference& operator = (const Reference<S, U>& other)
	{
		return *this = static_cast<U>(other);
	}

	__device__ Reference& operator += (const T& value)
	{
		return *this = *this + value;
	}
	__device__ Reference& operator |= (const T& value)
	{
		return *this = *this | value;
	}
	__device__ Reference& operator &= (const T& value)
	{
		return *this = *this & value;
	}
	__device__ Reference& operator *= (const T& value)
	{
		return *this = *this * value;
	}

	__device__ operator T() const
	{
		return load<S>(mPtr);
	}
	__device__ Reference& operator = (const T& value)
	{
		store<S>(mPtr, value);
		return *this;
	}

  //private:
	T* mPtr;

	__device__ explicit Reference(T& ref) : mPtr(&ref)
	{
	}

	template <typename U>
	friend __device__ void atomicAdd(Reference& ref, U value)
	{
		::atomicAdd(ref.mPtr, value);
	}
};

template <AddressSpace S, typename T>
struct Convert
{
	static __device__ T* from(T* ptr)
	{
		return ptr;
	}
	static __device__ T* to(T* ptr)
	{
		return ptr;
	}
};

#if CONVERT_ADDRESSES
template <typename T>
struct Convert<Shared, T>
{
	static __device__ T* from(T* ptr)
	{
		asm("cvta.shared." POINTER_TYPE " %0, %0;" : "+" POINTER_CONSTRAINT(ptr));
		return ptr;
	}
	static __device__ T* to(T* ptr)
	{
		asm("cvta.to.shared." POINTER_TYPE " %0, %0;" : "+" POINTER_CONSTRAINT(ptr));
		return ptr;
	}
};
template <typename T>
struct Convert<Global, T>
{
	static __device__ T* from(T* ptr)
	{
		asm("cvta.global." POINTER_TYPE " %0, %0;" : "+" POINTER_CONSTRAINT(ptr));
		return ptr;
	}
	static __device__ T* to(T* ptr)
	{
		asm("cvta.to.global." POINTER_TYPE " %0, %0;" : "+" POINTER_CONSTRAINT(ptr));
		return ptr;
	}
};
#endif

template <AddressSpace S, typename T>
__device__ T* generic(const Pointer<S, T>&);

// pointer forced to point to shared memory (only works for sizeof(T) <= 4)
template <AddressSpace S, typename T>
struct Pointer
{
	template <AddressSpace, typename> friend struct Pointer;
	friend __device__ T* generic<S, T>(const Pointer<S, T>&);
	friend struct GlobalParticleData;

	__device__ Pointer()
	{
	}
	__device__ Pointer(const Pointer& other) 
		: mPtr(other.mPtr)
	{
	}
	template <typename U>
	__device__ Pointer(const Pointer<S, U>& other)
	: mPtr(other.mPtr)
	{
	}

	// construct from generic pointer
	__device__ explicit Pointer(T* ptr) 
		: mPtr(Convert<S, T>::to(ptr))
	{
	}

	__device__ bool operator!=(const Pointer& other) const
	{
		return mPtr != other.mPtr;
	}
	__device__ bool operator<(const Pointer& other) const
	{
		return mPtr < other.mPtr;
	}

	__device__ Pointer operator + (ptrdiff_t i) const
	{
		return Pointer(*this) += i;
	}
	__device__ Pointer& operator += (ptrdiff_t i)
	{
		mPtr += i * stride();
		return *this;
	}
	__device__ Pointer operator - (ptrdiff_t i) const
	{
		return Pointer(*this) -= i;
	}
	__device__ Pointer& operator -= (ptrdiff_t i)
	{
		mPtr -= i * stride();
		return *this;
	}

	__device__ Pointer& operator ++ ()
	{
		mPtr += stride();
		return *this;
	}
	__device__ Pointer& operator -- ()
	{
		mPtr -= stride();
		return *this;
	}

	__device__ Reference<S, T> operator*() const
	{
		return Reference<S, T>(*mPtr);
	}
	__device__ Reference<S, T> operator[](int32_t i) const
	{
		return Reference<S, T>(mPtr[i * stride()]);
	}

  private:
	// convert back to generic pointer, private for safety, use generic() instead
	__device__ operator T*() const
	{
		return Convert<S, T>::from(mPtr);
	}

	__device__ static size_t stride() { return 1; }

	template <typename U>
	__device__ Pointer(const Pointer<S, U>& other, ptrdiff_t stridedOffset)
		: mPtr(other.mPtr + stridedOffset)
	{
	}

	T* mPtr;
};

// pointers to global memory are all referring to particle data
// stored as array of structs, so they have a stride of 4.
template<> __device__ size_t Pointer<Global, float>::stride() { return 4; }
template<> __device__ size_t Pointer<Global, const float>::stride() { return 4; }
template <AddressSpace S, typename T>
__device__ T* generic(const Pointer<S, T>& ptr)
{
	return ptr;
}

#if !CONVERT_ADDRESSES
template <typename T>
__device__ T* generic(T* ptr)
{
	return ptr;
}
#endif

/***************** Particle Data **********************/

template <typename T>
struct SharedParticleReference
{
	__device__ operator float3() const
	{
		float3 result;
		result.x = mReferences[0];
		result.y = mReferences[1];
		result.z = mReferences[2];
		return result;
	}

	__device__ SharedParticleReference& operator = (const float3& vec)
	{
		mReferences[0] = vec.x;
		mReferences[1] = vec.y;
		mReferences[2] = vec.z;
		return *this;
	}

	__device__ operator float4() const
	{
		float4 result;
		result.x = mReferences[0];
		result.y = mReferences[1];
		result.z = mReferences[2];
		result.w = mReferences[3];
		return result;
	}

	__device__ SharedParticleReference& operator = (const float4& vec)
	{
		mReferences[0] = vec.x;
		mReferences[1] = vec.y;
		mReferences[2] = vec.z;
		mReferences[3] = vec.w;
		return *this;
	}

	Reference<Shared, T> mReferences[4];
};

struct SharedParticleData
{
	typedef float3 VectorType;

	typedef Pointer<Shared, float> PointerType;
	typedef Pointer<Shared, const float> ConstPointerType;
	typedef Reference<Shared, float> ReferenceType;
	typedef Reference<Shared, const float> ConstReferenceType;

	typedef SharedParticleReference<float> ParticleReferenceType;
	typedef SharedParticleReference<const float> ParticleConstReferenceType;

	__device__ ReferenceType operator()(int32_t index, int32_t element)
	{
		return mPointers[element][index];
	}
	__device__ ConstReferenceType operator()(int32_t index, int32_t element) const
	{
		return mPointers[element][index];
	}

	__device__ ParticleReferenceType operator()(int32_t index)
	{
		ParticleReferenceType result = { mPointers[0][index], mPointers[1][index],
			                             mPointers[2][index], mPointers[3][index] };
		return result;
	}
	__device__ ParticleConstReferenceType operator()(int32_t index) const
	{
		ParticleConstReferenceType result = { mPointers[0][index], mPointers[1][index],
			                                  mPointers[2][index], mPointers[3][index] };
		return result;
	}

	__device__ const PointerType& operator[](int32_t element)
	{
		return mPointers[element];
	}
	__device__ ConstPointerType operator[](int32_t element) const
	{
		return mPointers[element];
	}

	PointerType mPointers[4];
};

template <typename T>
struct GlobalParticleReference
{
	__device__ GlobalParticleReference(Pointer<Global, T> ref) : mPtr(reinterpret_cast<T* const&>(ref))
	{
	}

#if CONVERT_ADDRESSES
	__device__ operator float4() const
	{
		float4 vec;
		asm("ld.global.v4.f32 {%0, %1, %2, %3}, [%4];"
		    : "=f"(vec.x), "=f"(vec.y), "=f"(vec.z), "=f"(vec.w)
		    : POINTER_CONSTRAINT(mPtr));
		return vec;
	}

	__device__ GlobalParticleReference& operator = (const float4& vec)
	{
		asm("st.global.v4.f32 [%0], {%1, %2, %3, %4};" ::POINTER_CONSTRAINT(mPtr), "f"(vec.x), "f"(vec.y), "f"(vec.z),
		    "f"(vec.w)
		    : "memory");
		return *this;
	}

	__device__ operator float3() const
	{
		float4 vec = *this;
		return make_float3(vec.x, vec.y, vec.z);
	}
#else

	__device__ operator float4() const
	{
		return *reinterpret_cast<const float4*>(mPtr);
	}

	__device__ GlobalParticleReference& operator = (const float4& vec)
	{
		*reinterpret_cast<float4*>(mPtr) = vec;
		return *this;
	}

	__device__ operator float3() const
	{
		return *reinterpret_cast<const float3*>(mPtr);
	}

	__device__ GlobalParticleReference& operator = (const float3& vec)
	{
		*reinterpret_cast<float3*>(mPtr) = vec;
		return *this;
	}
#endif

	T* mPtr; // pointer to global address
};

struct GlobalParticleData
{
#if CONVERT_ADDRESSES
	// ld.global.v4 saturates memory bandwidth better than 3x ld.global
	typedef float4 VectorType;
#else
	// the same isn't true for ld without state space
	typedef float3 VectorType;
#endif

	typedef Pointer<Global, float> PointerType;
	typedef Pointer<Global, const float> ConstPointerType;
	typedef Reference<Global, float> ReferenceType;
	typedef Reference<Global, const float> ConstReferenceType;

	typedef GlobalParticleReference<float> ParticleReferenceType;
	typedef GlobalParticleReference<const float> ParticleConstReferenceType;

	__device__ ReferenceType operator()(int32_t index, int32_t element)
	{
		return *PointerType(mPtr, index * 4 + element);
	}
	__device__ ConstReferenceType operator()(int32_t index, int32_t element) const
	{
		return *ConstPointerType(mPtr, index * 4 + element);
	}

	__device__ ParticleReferenceType operator()(int32_t index)
	{
		return PointerType(mPtr, index * 4);
	}

	__device__ ParticleConstReferenceType operator()(int32_t index) const
	{
		return ConstPointerType(mPtr, index * 4);
	}

	__device__ PointerType operator[](int32_t element)
	{
		return PointerType(mPtr, element);
	}
	__device__ ConstPointerType operator[](int32_t element) const
	{
		return ConstPointerType(mPtr, element);
	}

	PointerType mPtr;
};

/***************** Profiling **********************/
struct ProfileDisabledZone
{
	__device__ ProfileDisabledZone(cloth::CuProfileZoneIds::Enum)
	{
	}
};

#if defined(__CUDA_ARCH__) && defined(PX_PROFILE) // profile zones enabled for profile build

// code below is copied from GPUProfile.h and needs to be kept in sync.

#define NUM_WARPS_PER_PROFILE_BUFFER (4 * 1024 * 1024)

struct __align__(16) WarpProfileEvent
{
	__device__ WarpProfileEvent(uint16_t id)
	: block(blockIdx.x + gridDim.x * blockIdx.y), warp(threadIdx.x >> 5), userData(0), eventId(id)
	{
		uint32_t smid32, warpid32;
		asm volatile("mov.u32 %0, %smid;" : "=r"(smid32));
		asm volatile("mov.u32 %0, %warpid;" : "=r"(warpid32));
		asm volatile("mov.u32 %0, %clock;" : "=r"(startTime));
		smid = smid32;
		warpid = warpid32;
		endTime = startTime;
	}

	uint16_t block;
	uint8_t warp;
	uint8_t smid;
	uint8_t warpid;
	uint8_t userData;
	uint16_t eventId;
	uint32_t startTime;
	uint32_t endTime;
};

struct ProfileZone
{
	__device__ ProfileZone(cloth::CuProfileZoneIds::Enum id) : mEvent(0)
	{
		if (!gProfileBuffer || threadIdx.x & 0x1f)
			return;

		// +1: first entry reserved for counter
		uint32_t index = atomicAdd(reinterpret_cast<uint32_t*>(gProfileBuffer), 1) + 1;

		if (index >= NUM_WARPS_PER_PROFILE_BUFFER)
			return;

		mEvent = reinterpret_cast<WarpProfileEvent*>(gProfileBuffer) + index;

		new (mEvent) WarpProfileEvent(gProfileBaseId + id);
	}

	__device__ ~ProfileZone()
	{
		if (mEvent)
			mEvent->endTime = clock();
	}

	WarpProfileEvent* mEvent;
};

#else
typedef ProfileDisabledZone ProfileZone;
#endif

#if 1 // set to 1 to enable detailed profile zones
typedef ProfileZone ProfileDetailZone;
#else
typedef ProfileDisabledZone ProfileDetailZone;
#endif

namespace
{
// cut down version of thrust::uninitialized
// avoids warning about non-empty c'tor
template <typename T>
struct uninitialized
{
	__device__ inline T& get()
	{
		return *reinterpret_cast<T*>(data);
	}

	// maximum alignment required by device code is 16
	__align__(16) unsigned char data[sizeof(T)];
};
}

#if __CUDA_ARCH__ < 320
namespace
{
template <typename T>
__device__ T __ldg(const T* __restrict ptr)
{
	return *ptr;
}
}
#endif

#define CU_SOLVER_KERNEL_CU
#include "CuCollision.h"
#include "CuSelfCollision.h"

namespace
{
__device__ void loadIterData(const cloth::CuIterationData* __restrict iterData)
{
	if (threadIdx.x < gCuIterationDataSize)
	{
		gIterData.mIntegrationTrafo[threadIdx.x] = __ldg(iterData->mIntegrationTrafo + threadIdx.x);
	}
}

// integrate particle positions and store transposed
template <bool IsTurning, typename CurrentT, typename PreviousT>
__device__ void integrateParticles(CurrentT& current, PreviousT& previous)
{
	ProfileDetailZone zone(cloth::CuProfileZoneIds::INTEGRATE);

	const float* __restrict trafo = gIterData.mIntegrationTrafo;

	for (int32_t i = threadIdx.x; i < gClothData.mNumParticles; i += blockDim.x)
	{
		float4 prev = previous(i);
		float4 next = current(i);
		float4 cur = { next.x, next.y, next.z, prev.w };

		if (next.w == 0.0f)
			next.w = prev.w;

		if (next.w > 0.0f)
		{
			if (IsTurning)
			{
				next.x = next.x + trafo[3] + cur.x * trafo[15] + prev.x * trafo[6] + cur.y * trafo[16] +
				         prev.y * trafo[7] + cur.z * trafo[17] + prev.z * trafo[8];

				next.y = next.y + trafo[4] + cur.x * trafo[18] + prev.x * trafo[9] + cur.y * trafo[19] +
				         prev.y * trafo[10] + cur.z * trafo[20] + prev.z * trafo[11];

				next.z = next.z + trafo[5] + cur.x * trafo[21] + prev.x * trafo[12] + cur.y * trafo[22] +
				         prev.y * trafo[13] + cur.z * trafo[23] + prev.z * trafo[14];
			}
			else
			{
				next.x += (cur.x - prev.x) * trafo[6] + trafo[3];
				next.y += (cur.y - prev.y) * trafo[9] + trafo[4];
				next.z += (cur.z - prev.z) * trafo[12] + trafo[5];
			}

			cur.x += trafo[0];
			cur.y += trafo[1];
			cur.z += trafo[2];
		}

		current(i) = next;
		previous(i) = cur;
	}
}

template <typename CurrentT, typename PreviousT>
__device__ void integrateParticles(CurrentT& current, PreviousT& previous)
{
	if (gIterData.mIsTurning)
		integrateParticles<true>(current, previous);
	else
		integrateParticles<false>(current, previous);
}

template <typename CurrentT>
__device__ void accelerateParticles(CurrentT& current)
{
	// might be better to move this into integrate particles
	const float* __restrict accelerations = gFrameData.mParticleAccelerations;

	if (!accelerations)
		return;

	ProfileDetailZone zone(cloth::CuProfileZoneIds::ACCELERATE);

	__syncthreads(); // looping with 4 instead of 1 thread per particle

	float sqrIterDt = ~threadIdx.x & 0x3 ? gFrameData.mIterDt * gFrameData.mIterDt : 0.0f;
	typename CurrentT::PointerType sharedCurPos = current[threadIdx.x % 4];

	for (int32_t i = threadIdx.x; i < gClothData.mNumParticles * 4; i += blockDim.x)
	{
		// turning this into __ldg slows kernel down even without particle accelerations (!)
		if (current(i / 4, 3) > 0.0f)
			sharedCurPos[i / 4] += accelerations[i] * sqrIterDt;
	}

	__syncthreads();
}

__device__ float3 operator + (const float3& u, const float3& v)
{
	return make_float3(u.x + v.x, u.y + v.y, u.z + v.z);
}
__device__ float3 operator - (const float3& u, const float3& v)
{
	return make_float3(u.x - v.x, u.y - v.y, u.z - v.z);
}
__device__ float3 operator*(float s, const float3& v)
{
	return make_float3(v.x * s, v.y * s, v.z * s);
}
__device__ float dot3(const float3& u, const float3& v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}
__device__ float3 cross3(const float3& u, const float3& v)
{
	return make_float3(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}
__device__ void applyImpulse(SharedParticleData::ParticleReferenceType pos, const float3& impulse)
{
	float scale = -pos.mReferences[3];
	//Use this instead of atomicAdd function to work around compiler issue treating the pointer as global memory instead of shared memory
	asm("red.shared.add.f32 [%0], %1;" :: POINTER_CONSTRAINT(pos.mReferences[0].mPtr), "f"(impulse.x * scale));
	asm("red.shared.add.f32 [%0], %1;" :: POINTER_CONSTRAINT(pos.mReferences[1].mPtr), "f"(impulse.y * scale));
	asm("red.shared.add.f32 [%0], %1;" :: POINTER_CONSTRAINT(pos.mReferences[2].mPtr), "f"(impulse.z * scale));
}
__device__ void applyImpulse(GlobalParticleData::ParticleReferenceType pos, const float3& impulse)
{
	float scale = -pos.mPtr[3];
	atomicAdd(pos.mPtr + 0, impulse.x * scale);
	atomicAdd(pos.mPtr + 1, impulse.y * scale);
	atomicAdd(pos.mPtr + 2, impulse.z * scale);
}

template <bool IsTurning, typename CurrentT, typename PreviousT>
__device__ void applyWind(CurrentT& current, PreviousT& previous)
{
	const float dragCoefficient = gFrameData.mDragCoefficient;
	const float liftCoefficient = gFrameData.mLiftCoefficient;

	if (dragCoefficient == 0.0f && liftCoefficient == 0.0f)
		return;

	ProfileDetailZone zone(cloth::CuProfileZoneIds::WIND);

	const float oneThird = 1 / 3.0f;
	float3 wind = make_float3(gIterData.mWind[0], gIterData.mWind[1], gIterData.mWind[2]);

	const uint16_t* tIt = gClothData.mTriangles;
	for (int32_t i = threadIdx.x; i < gClothData.mNumTriangles; i += blockDim.x)
	{
		uint16_t i0 = tIt[i * 3 + 0];
		uint16_t i1 = tIt[i * 3 + 1];
		uint16_t i2 = tIt[i * 3 + 2];

		float3 c0 = current(i0);
		float3 c1 = current(i1);
		float3 c2 = current(i2);

	//	float w1 = current(i0, 3);
	//	float w2 = current(i1, 3);
	//	float w2 = current(i2, 3);
	//
	//	float wMult = w1 * w2 * w3;
	//	float invMass = wMult < FLT_EPSILON ? 0.f : w1 * w2 * w3 / (w1 * w2 + w1 * w3 + w2 * w3);

		float3 p0 = previous(i0);
		float3 p1 = previous(i1);
		float3 p2 = previous(i2);

		float3 cur = oneThird * (c0 + c1 + c2);
		float3 prev = oneThird * (p0 + p1 + p2);

		float3 delta = cur - prev + wind;

		if (IsTurning)
		{
			const float3* rot = reinterpret_cast<const float3*>(gFrameData.mRotation);
			float3 d = wind - prev;
			delta = cur + d.x * rot[0] + d.y * rot[1] + d.z * rot[2];
		}

		float3 normal = cross3(c2 - c0, c1 - c0);

		float doubleArea = sqrtf(dot3(normal, normal));

		float invSqrScale = dot3(delta, delta);
		float scale = rsqrtf(invSqrScale);

		float cosTheta = dot3(normal, delta) * scale / doubleArea;
		float sinTheta = sqrtf(max(0.0f, 1.0f - cosTheta * cosTheta));

		float3 liftDir = cross3(cross3(delta, normal), scale * delta);

		float3 lift = liftCoefficient * cosTheta * sinTheta * liftDir;
		float3 drag = dragCoefficient * abs(cosTheta) * doubleArea * delta;

		float3 impulse = invSqrScale < FLT_EPSILON ? make_float3(0.0f, 0.0f, 0.0f) : lift + drag;

		applyImpulse(current(i0), impulse);
		applyImpulse(current(i1), impulse);
		applyImpulse(current(i2), impulse);
	}

	__syncthreads();
}

template <typename CurrentT, typename PreviousT>
__device__ void applyWind(CurrentT& current, PreviousT& previous)
{
	if (gIterData.mIsTurning)
		applyWind<true>(current, previous);
	else
		applyWind<false>(current, previous);
}

template <typename CurrentT>
__device__ void constrainTether(CurrentT& current)
{
	if (0.0f == gFrameData.mTetherConstraintStiffness || !gClothData.mNumTethers)
		return;

	ProfileDetailZone zone(cloth::CuProfileZoneIds::TETHER);

	int32_t numParticles = gClothData.mNumParticles;
	int32_t numTethers = gClothData.mNumTethers;
	assert(0 == numTethers % numParticles);

	float stiffness = numParticles * __fdividef(gFrameData.mTetherConstraintStiffness, numTethers);
	float scale = gClothData.mTetherConstraintScale;

	const uint32_t* __restrict tIt = reinterpret_cast<const uint32_t*>(gClothData.mTethers);

	for (int32_t i = threadIdx.x; i < numParticles; i += blockDim.x)
	{
		float posX = current(i, 0);
		float posY = current(i, 1);
		float posZ = current(i, 2);

		float offsetX = 0.0f;
		float offsetY = 0.0f;
		float offsetZ = 0.0f;

		for (int32_t j = i; j < numTethers; j += gClothData.mNumParticles)
		{
			uint32_t tether = __ldg(tIt + j);

			int32_t anchor = tether & 0xffff;
			float deltaX = current(anchor, 0) - posX;
			float deltaY = current(anchor, 1) - posY;
			float deltaZ = current(anchor, 2) - posZ;

			float sqrLength = FLT_EPSILON + deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;

			float radius = (tether >> 16) * scale;
			float slack = 1.0f - radius * rsqrtf(sqrLength);

			if (slack > 0.0f)
			{
				offsetX += deltaX * slack;
				offsetY += deltaY * slack;
				offsetZ += deltaZ * slack;
			}
		}

		current(i, 0) = posX + offsetX * stiffness;
		current(i, 1) = posY + offsetY * stiffness;
		current(i, 2) = posZ + offsetZ * stiffness;
	}
}

template <typename CurrentT>
__device__ void solveFabric(CurrentT& current)
{
	ProfileDetailZone zone(cloth::CuProfileZoneIds::FABRIC);

	const cloth::CuPhaseConfig* __restrict cIt = (cloth::CuPhaseConfig*)gSharedMemory;
	const cloth::CuPhaseConfig* cEnd = cIt + gClothData.mNumPhases;

	for (; cIt != cEnd; ++cIt)
	{
		__syncthreads();

		ProfileDetailZone zone(cloth::CuProfileZoneIds::CONSTRAINT_SET);

		int32_t numConstraints = cIt->mNumConstraints;
		if (threadIdx.x >= numConstraints)
			continue;

		const uint32_t* __restrict iIt = reinterpret_cast<const uint32_t*>(cIt->mIndices) + threadIdx.x;
		const float* restvalues = cIt->mRestvalues;
		const float* rIt = restvalues + threadIdx.x;
		const float* rEnd = restvalues + numConstraints;

		const float* stIt = cIt->mStiffnessValues + threadIdx.x;
		bool useStiffnessPerConstraint = cIt->mStiffnessValues!=nullptr;

		uint32_t vpijPrefetch = __ldg(iIt);
		float rijPrefetch = __ldg(rIt);
		float stijPrefetch;
		if (useStiffnessPerConstraint)
			stijPrefetch = __ldg(stIt);

		float stiffness = cIt->mStiffness;
		float stiffnessMultiplier = cIt->mStiffnessMultiplier;
		float compressionLimit = cIt->mCompressionLimit;
		float stretchLimit = cIt->mStretchLimit;

		do
		{
			rIt += blockDim.x;
			iIt += blockDim.x;
			stIt += blockDim.x;

			int32_t vpi = USHRT_MAX & vpijPrefetch;
			int32_t vpj = USHRT_MAX & vpijPrefetch >> 16;
			float rij = rijPrefetch;
			float stij = useStiffnessPerConstraint?1.0f - exp2f(stijPrefetch * gFrameData.mStiffnessExponent):stiffness;

			if (rIt < rEnd)
			{
				vpijPrefetch = __ldg(iIt);
				rijPrefetch = __ldg(rIt);
				if (useStiffnessPerConstraint)
					stijPrefetch = __ldg(stIt);
			}

			float vxi = current(vpi, 0);
			float vyi = current(vpi, 1);
			float vzi = current(vpi, 2);
			float vwi = current(vpi, 3);

			float vxj = current(vpj, 0);
			float vyj = current(vpj, 1);
			float vzj = current(vpj, 2);
			float vwj = current(vpj, 3);

			float hxij = vxj - vxi;
			float hyij = vyj - vyi;
			float hzij = vzj - vzi;

			float e2ij = FLT_EPSILON + hxij * hxij + hyij * hyij + hzij * hzij;
			float negErij = rij > FLT_EPSILON ? -1.0f + rij * rsqrtf(e2ij) : 0.0f;

			negErij = negErij + stiffnessMultiplier * max(compressionLimit, min(-negErij, stretchLimit));

			float negExij = __fdividef(negErij * stij, FLT_EPSILON + vwi + vwj);

			float vmi = -vwi * negExij;
			current(vpi, 0) = vxi + vmi * hxij;
			current(vpi, 1) = vyi + vmi * hyij;
			current(vpi, 2) = vzi + vmi * hzij;

			float vmj = +vwj * negExij;
			current(vpj, 0) = vxj + vmj * hxij;
			current(vpj, 1) = vyj + vmj * hyij;
			current(vpj, 2) = vzj + vmj * hzij;

		} while (rIt < rEnd);
	}

	__syncthreads();
}

template <typename CurrentT>
__device__ void constrainMotion(CurrentT& current, float alpha)
{
	if (!gFrameData.mStartMotionConstraints)
		return;

	ProfileDetailZone zone(cloth::CuProfileZoneIds::MOTION);

	// negative because of fused multiply-add optimization
	float negativeScale = -gClothData.mMotionConstraintScale;
	float negativeBias = -gClothData.mMotionConstraintBias;

	const float4* startIt = reinterpret_cast<const float4*>(gFrameData.mStartMotionConstraints);
	const float4* targetIt = reinterpret_cast<const float4*>(gFrameData.mTargetMotionConstraints);

	for (int32_t i = threadIdx.x; i < gClothData.mNumParticles; i += blockDim.x)
	{
		float4 startPos = __ldg(startIt + i);
		float4 targetPos = __ldg(targetIt + i);

		float sphereX = startPos.x + (targetPos.x - startPos.x) * alpha;
		float sphereY = startPos.y + (targetPos.y - startPos.y) * alpha;
		float sphereZ = startPos.z + (targetPos.z - startPos.z) * alpha;
		float sphereW = startPos.w + (targetPos.w - startPos.w) * alpha;

		float dx = sphereX - current(i, 0);
		float dy = sphereY - current(i, 1);
		float dz = sphereZ - current(i, 2);

		float sqrLength = FLT_EPSILON + dx * dx + dy * dy + dz * dz;
		float negativeRadius = min(0.0f, sphereW * negativeScale + negativeBias);

		float slack = max(negativeRadius * rsqrtf(sqrLength) + 1.0f, 0.0f) * gFrameData.mMotionConstraintStiffness;

		current(i, 0) += slack * dx;
		current(i, 1) += slack * dy;
		current(i, 2) += slack * dz;

		// set invMass to zero if radius is zero
		if (negativeRadius >= 0.0f)
			current(i, 3) = 0.0f;
	}
}

template <typename T>
__device__ void constrainSeparation(T& current, float alpha)
{
	if (!gFrameData.mStartSeparationConstraints)
		return;

	ProfileDetailZone zone(cloth::CuProfileZoneIds::SEPARATION);

	const float4* startIt = reinterpret_cast<const float4*>(gFrameData.mStartSeparationConstraints);
	const float4* targetIt = reinterpret_cast<const float4*>(gFrameData.mTargetSeparationConstraints);

	for (int32_t i = threadIdx.x; i < gClothData.mNumParticles; i += blockDim.x)
	{
		float4 startPos = __ldg(startIt + i);
		float4 targetPos = __ldg(targetIt + i);

		float sphereX = startPos.x + (targetPos.x - startPos.x) * alpha;
		float sphereY = startPos.y + (targetPos.y - startPos.y) * alpha;
		float sphereZ = startPos.z + (targetPos.z - startPos.z) * alpha;
		float sphereW = startPos.w + (targetPos.w - startPos.w) * alpha;

		float dx = sphereX - current(i, 0);
		float dy = sphereY - current(i, 1);
		float dz = sphereZ - current(i, 2);

		float sqrLength = FLT_EPSILON + dx * dx + dy * dy + dz * dz;

		float slack = min(0.0f, 1.0f - sphereW * rsqrtf(sqrLength));

		current(i, 0) += slack * dx;
		current(i, 1) += slack * dy;
		current(i, 2) += slack * dz;
	}
}

template <typename CurrentT, typename PreviousT>
__device__ void updateSleepState(const CurrentT& current, const PreviousT& previous)
{
	ProfileDetailZone zone(cloth::CuProfileZoneIds::SLEEP);

	if (!threadIdx.x)
		gFrameData.mSleepTestCounter += max(1, uint32_t(gFrameData.mIterDt * 1000));

	__syncthreads();

	if (gFrameData.mSleepTestCounter < gClothData.mSleepTestInterval)
		return;

	float maxDelta = 0.0f;
	for (int32_t i = threadIdx.x; i < gClothData.mNumParticles; i += blockDim.x)
	{
		float4 prev = previous(i);
		maxDelta = max(fabsf(current(i, 0) - prev.x), maxDelta);
		maxDelta = max(fabsf(current(i, 1) - prev.y), maxDelta);
		maxDelta = max(fabsf(current(i, 2) - prev.z), maxDelta);
	}

	if (!threadIdx.x)
	{
		++gFrameData.mSleepPassCounter;
		gFrameData.mSleepTestCounter -= gClothData.mSleepTestInterval;
	}

	__syncthreads();

	if (maxDelta > gClothData.mSleepThreshold * gFrameData.mIterDt)
		gFrameData.mSleepPassCounter = 0;
}

template <typename CurrentT, typename PreviousT>
__device__ void simulateCloth(CurrentT& current, PreviousT& previous)
{
	// apply exponent to phase configs
	assert(blockDim.x >= gClothData.mNumPhases);
	if (threadIdx.x < gClothData.mNumPhases)
	{
		float exponent = gFrameData.mStiffnessExponent;
		float* ptr = gSharedMemory + threadIdx.x * gCuPhaseConfigSize;
		ptr[0] = 1.0f - exp2f(ptr[0] * exponent);
		ptr[1] = 1.0f - exp2f(ptr[1] * exponent);
	}

	uint32_t numIterations = gFrameData.mNumIterations;
	float invNumIterations = __fdividef(1.0f, numIterations);

	const cloth::CuIterationData* iterData = gFrameData.mIterationData;
	const cloth::CuIterationData* iterEnd = iterData + numIterations;

	loadIterData(iterData);

	__syncthreads();

	for (float alpha = invNumIterations; iterData++ != iterEnd; alpha += invNumIterations)
	{
		integrateParticles(current, previous);
		accelerateParticles(current);
		applyWind(current, previous);
		constrainMotion(current, alpha);
		constrainTether(current);
		solveFabric(current);
		loadIterData(iterData);
		constrainSeparation(current, alpha);
		gCollideParticles.get()(current, previous, alpha);
		gSelfCollideParticles.get()(current);
		updateSleepState(current, previous);
	}

	__syncthreads();
}

__device__ void simulateShared()
{
	ProfileZone zone(cloth::CuProfileZoneIds::SIMULATE_SHARED);

	__shared__ uninitialized<SharedParticleData> current;
	__shared__ uninitialized<SharedParticleData> previous;

	int32_t configDataSize = gClothData.mNumPhases * gCuPhaseConfigSize;
	int32_t particlesDataSize = 4 * gClothData.mNumParticles;

	Pointer<Shared, float> sharedCurPos =
	    Pointer<Shared, float>(gSharedMemory + configDataSize + threadIdx.x % 4 * gClothData.mNumParticles);
	Pointer<Shared, float> sharedPrevPos = sharedCurPos + particlesDataSize;

	if (threadIdx.x < 4)
	{
		current.get().mPointers[threadIdx.x] = sharedCurPos;
		previous.get().mPointers[threadIdx.x] = sharedPrevPos;
	}

	float* globalCurPos = gClothData.mParticles;
	float* globalPrevPos = gClothData.mParticles + particlesDataSize;

	// copy particles from device memory to shared memory and transpose
	for (int32_t i = threadIdx.x; i < particlesDataSize; i += blockDim.x)
	{
		sharedCurPos[i / 4] = globalCurPos[i];
		sharedPrevPos[i / 4] = globalPrevPos[i];
	}

	simulateCloth(current.get(), previous.get());

	// copy particles from shared memory to device memory and transpose
	for (int32_t i = threadIdx.x; i < particlesDataSize; i += blockDim.x)
	{
		globalCurPos[i] = sharedCurPos[i / 4];
		globalPrevPos[i] = sharedPrevPos[i / 4];
	}

	__syncthreads();
}

__device__ void simulateStreamed()
{
	ProfileZone zone(cloth::CuProfileZoneIds::SIMULATE_STREAMED);

	__shared__ uninitialized<SharedParticleData> current;
	__shared__ uninitialized<GlobalParticleData> previous;

	int32_t configDataSize = gClothData.mNumPhases * gCuPhaseConfigSize;
	int32_t particlesDataSize = 4 * gClothData.mNumParticles;

	float* globalCurPos = gClothData.mParticles;
	Pointer<Shared, float> sharedCurPos =
	    Pointer<Shared, float>(gSharedMemory + configDataSize + threadIdx.x % 4 * gClothData.mNumParticles);

	if (threadIdx.x < 4)
		current.get().mPointers[threadIdx.x] = sharedCurPos;
	if (!threadIdx.x)
		previous.get().mPtr = GlobalParticleData::PointerType(globalCurPos + particlesDataSize);

	// copy particles from device memory to shared memory and transpose
	for (int32_t i = threadIdx.x; i < particlesDataSize; i += blockDim.x)
		sharedCurPos[i / 4] = globalCurPos[i];

	simulateCloth(current.get(), previous.get());

	// copy particles from shared memory to device memory and transpose
	for (int32_t i = threadIdx.x; i < particlesDataSize; i += blockDim.x)
		globalCurPos[i] = sharedCurPos[i / 4];

	__syncthreads();
}

__device__ void simulateGlobal()
{
	ProfileZone zone(cloth::CuProfileZoneIds::SIMULATE_GLOBAL);

	__shared__ uninitialized<GlobalParticleData> current;
	__shared__ uninitialized<GlobalParticleData> previous;

	if (!threadIdx.x)
	{
		GlobalParticleData::PointerType globalCurPos(gClothData.mParticles);
		current.get().mPtr = globalCurPos;
		previous.get().mPtr = globalCurPos + gClothData.mNumParticles;
	}

	simulateCloth(current.get(), previous.get());
}

} // anonymous namespace

extern "C" __global__ void
#if __CUDA_ARCH__ >= 300
__launch_bounds__(1024, 1)
#else
__launch_bounds__(512, 1)
#endif
    simulateCloths(cloth::CuKernelData kernelData)
{
	gProfileBuffer = kernelData.mProfileBuffer;
	gProfileBaseId = kernelData.mProfileBaseId;

	ProfileZone zone(cloth::CuProfileZoneIds::SIMULATE);

	// check that http://nvbugs/1038473 is fixed
	assert(gSharedMemory > (float*)&gFrameData);
	assert(gSharedMemory > (float*)&gClothData);

	// fetch cloth index from queue
	__shared__ uint32_t clothIdx;
	if (!threadIdx.x)
		clothIdx = atomicInc(kernelData.mClothIndex, gridDim.x - 1);
	__syncthreads();
	assert(clothIdx < gridDim.x);

	// copy cloth data to shared memory
	const uint32_t* clothData = reinterpret_cast<const uint32_t*>(kernelData.mClothData + clothIdx);
	if (threadIdx.x < gCuClothDataSize)
		reinterpret_cast<uint32_t*>(&gClothData)[threadIdx.x] = clothData[threadIdx.x];

	// copy frame data to shared memory
	uint32_t* frameData = reinterpret_cast<uint32_t*>(kernelData.mFrameData + clothIdx);
	if (threadIdx.x < gCuFrameDataSize)
		reinterpret_cast<uint32_t*>(&gFrameData)[threadIdx.x] = frameData[threadIdx.x];

	__syncthreads();

	if (gFrameData.mSleepPassCounter >= gClothData.mSleepAfterCount)
		return; // cloth is sleeping, exit

	// copy phase configs to shared memory
	int32_t configDataSize = gClothData.mNumPhases * gCuPhaseConfigSize;
	for (int32_t i = threadIdx.x; i < configDataSize; i += blockDim.x)
		gSharedUnsigned[i] = reinterpret_cast<const uint32_t*>(gClothData.mPhaseConfigs)[i];

	Pointer<Shared, uint32_t> scratchPtr = Pointer<Shared, uint32_t>(
	    gSharedUnsigned + configDataSize + 4 * gFrameData.mNumSharedPositions * gClothData.mNumParticles);

	// initialize with placement new
	new (gCollideParticles.data) CuCollision(scratchPtr);
	new (gSelfCollideParticles.data) CuSelfCollision();

	// copy particles and constraints to device
	if (gFrameData.mDeviceParticlesDirty)
	{
		for (int32_t i = threadIdx.x; i < gClothData.mNumParticles * 8; i += blockDim.x)
			gClothData.mParticles[i] = gClothData.mParticlesHostCopy[i];
	}
	if (gFrameData.mHostMotionConstraints)
	{
		for (int32_t i = threadIdx.x; i < gClothData.mNumParticles * 4; i += blockDim.x)
			gFrameData.mTargetMotionConstraints[i] = gFrameData.mHostMotionConstraints[i];
	}
	if (gFrameData.mHostSeparationConstraints)
	{
		for (int32_t i = threadIdx.x; i < gClothData.mNumParticles * 4; i += blockDim.x)
			gFrameData.mTargetSeparationConstraints[i] = gFrameData.mHostSeparationConstraints[i];
	}
	if (gFrameData.mHostParticleAccelerations)
	{
		for (int32_t i = threadIdx.x; i < gClothData.mNumParticles * 4; i += blockDim.x)
			gFrameData.mParticleAccelerations[i] = gFrameData.mHostParticleAccelerations[i];
	}

	// necessary to ensure phase configs are fully loaded before setup in simulateCloth()
	__syncthreads();

	switch(gFrameData.mNumSharedPositions)
	{
	case 0:
		simulateGlobal();
		break;
	case 1:
		simulateStreamed();
		break;
	case 2:
		simulateShared();
		break;
	}

	// write back frame data
	if (threadIdx.x < gCuFrameDataSize)
		frameData[threadIdx.x] = reinterpret_cast<const uint32_t*>(&gFrameData)[threadIdx.x];

	// copy particles to host
	for (int32_t i = threadIdx.x; i < gClothData.mNumParticles * 8; i += blockDim.x)
		gClothData.mParticlesHostCopy[i] = gClothData.mParticles[i];
}

const char* cloth::getKernelFunctionName()
{
	return "simulateCloths";
}
