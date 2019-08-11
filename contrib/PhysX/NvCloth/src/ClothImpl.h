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

#include "NvCloth/Cloth.h"
#include "NvCloth/Fabric.h"
#include <foundation/PxVec4.h>
#include <foundation/PxVec3.h>
#include "IndexPair.h"
#include "MovingAverage.h"
#include "NvCloth/ps/PsMathUtils.h"
#include <cmath>

namespace nv
{
namespace cloth
{

template<typename T>
class ClothTraits
{
};

// SwCloth or CuCloth aggregate implementing the Cloth interface
// Member specializations are implemented in Sw/CuCloth.cpp
template <typename T>
class ClothImpl : public Cloth
{
	ClothImpl(const ClothImpl&);

  public:
	ClothImpl() {}
	typedef T ClothType;
	typedef typename ClothTraits<ClothType>::FactoryType FactoryType;
	typedef typename ClothTraits<ClothType>::FabricType FabricType;
	typedef typename ClothTraits<ClothType>::ContextLockType ContextLockType;

	ClothImpl& operator = (const ClothImpl&);

	ClothType* getChildCloth() { return static_cast<T*>(this); }
	ClothType const* getChildCloth() const { return static_cast<T const*>(this); }

	virtual Fabric& getFabric() const;
	virtual Factory& getFactory() const;

	virtual void setTranslation(const physx::PxVec3& trans);
	virtual void setRotation(const physx::PxQuat& rot);

	virtual const physx::PxVec3& getTranslation() const;
	virtual const physx::PxQuat& getRotation() const;

	virtual void clearInertia();

	virtual void teleport(const physx::PxVec3& delta);
	virtual void teleportToLocation(const physx::PxVec3& translation, const physx::PxQuat& rotation);

	virtual void ignoreVelocityDiscontinuity();

	virtual float getPreviousIterationDt() const;
	virtual void setGravity(const physx::PxVec3& gravity);
	virtual physx::PxVec3 getGravity() const;
	virtual void setDamping(const physx::PxVec3& damping);
	virtual physx::PxVec3 getDamping() const;
	virtual void setLinearDrag(const physx::PxVec3& drag);
	virtual physx::PxVec3 getLinearDrag() const;
	virtual void setAngularDrag(const physx::PxVec3& drag);
	virtual physx::PxVec3 getAngularDrag() const;
	virtual void setLinearInertia(const physx::PxVec3& inertia);
	virtual physx::PxVec3 getLinearInertia() const;
	virtual void setAngularInertia(const physx::PxVec3& inertia);
	virtual physx::PxVec3 getAngularInertia() const;
	virtual void setCentrifugalInertia(const physx::PxVec3& inertia);
	virtual physx::PxVec3 getCentrifugalInertia() const;

	virtual void setSolverFrequency(float frequency);
	virtual float getSolverFrequency() const;

	virtual void setStiffnessFrequency(float frequency);
	virtual float getStiffnessFrequency() const;

	virtual void setAcceleationFilterWidth(uint32_t);
	virtual uint32_t getAccelerationFilterWidth() const;

	virtual void setSpheres(Range<const physx::PxVec4>, uint32_t first, uint32_t last);
	virtual void setSpheres(Range<const physx::PxVec4> startSpheres, Range<const physx::PxVec4> targetSpheres);
	virtual uint32_t getNumSpheres() const;

	virtual void setCapsules(Range<const uint32_t>, uint32_t first, uint32_t last);
	virtual uint32_t getNumCapsules() const;

	virtual void setPlanes(Range<const physx::PxVec4>, uint32_t first, uint32_t last);
	virtual void setPlanes(Range<const physx::PxVec4> startPlanes, Range<const physx::PxVec4> targetPlanes);
	virtual uint32_t getNumPlanes() const;

	virtual void setConvexes(Range<const uint32_t>, uint32_t first, uint32_t last);
	virtual uint32_t getNumConvexes() const;

	virtual void setTriangles(Range<const physx::PxVec3>, uint32_t first, uint32_t last);
	virtual void setTriangles(Range<const physx::PxVec3>, Range<const physx::PxVec3>, uint32_t first);
	virtual uint32_t getNumTriangles() const;

	virtual bool isContinuousCollisionEnabled() const;
	virtual void enableContinuousCollision(bool);

	virtual float getCollisionMassScale() const;
	virtual void setCollisionMassScale(float);
	virtual void setFriction(float friction);
	virtual float getFriction() const;

	virtual uint32_t getNumVirtualParticleWeights() const;

	virtual void setTetherConstraintScale(float scale);
	virtual float getTetherConstraintScale() const;
	virtual void setTetherConstraintStiffness(float stiffness);
	virtual float getTetherConstraintStiffness() const;

	virtual Range<physx::PxVec4> getMotionConstraints();
	virtual void clearMotionConstraints();
	virtual uint32_t getNumMotionConstraints() const;
	virtual void setMotionConstraintScaleBias(float scale, float bias);
	virtual float getMotionConstraintScale() const;
	virtual float getMotionConstraintBias() const;
	virtual void setMotionConstraintStiffness(float stiffness);
	virtual float getMotionConstraintStiffness() const;

	virtual Range<physx::PxVec4> getSeparationConstraints();
	virtual void clearSeparationConstraints();
	virtual uint32_t getNumSeparationConstraints() const;

	virtual void clearInterpolation();

	virtual uint32_t getNumParticleAccelerations() const;

	virtual void setWindVelocity(physx::PxVec3);
	virtual physx::PxVec3 getWindVelocity() const;
	virtual void setDragCoefficient(float);
	virtual float getDragCoefficient() const;
	virtual void setLiftCoefficient(float);
	virtual float getLiftCoefficient() const;
	virtual void setFluidDensity(float);
	virtual float getFluidDensity() const;

	virtual void setSelfCollisionDistance(float);
	virtual float getSelfCollisionDistance() const;
	virtual void setSelfCollisionStiffness(float);
	virtual float getSelfCollisionStiffness() const;

	virtual uint32_t getNumSelfCollisionIndices() const;

	virtual void setRestPositions(Range<const physx::PxVec4>);
	virtual uint32_t getNumRestPositions() const;

	virtual const physx::PxVec3& getBoundingBoxCenter() const;
	virtual const physx::PxVec3& getBoundingBoxScale() const;

	virtual void setSleepThreshold(float);
	virtual float getSleepThreshold() const;
	virtual void setSleepTestInterval(uint32_t);
	virtual uint32_t getSleepTestInterval() const;
	virtual void setSleepAfterCount(uint32_t);
	virtual uint32_t getSleepAfterCount() const;
	virtual uint32_t getSleepPassCount() const;
	virtual bool isAsleep() const;
	virtual void putToSleep();
	virtual bool isSleeping() const;
	virtual void wakeUp();

	virtual void setUserData(void*);
	virtual void* getUserData() const;

	// helper function
	template <typename U>
	MappedRange<U> getMappedParticles(U* data) const;

public: //Fields shared between all cloth classes
	physx::PxVec3 mParticleBoundsCenter;
	physx::PxVec3 mParticleBoundsHalfExtent;

	physx::PxVec3 mGravity;
	physx::PxVec3 mLogDamping;
	physx::PxVec3 mLinearLogDrag;
	physx::PxVec3 mAngularLogDrag;
	physx::PxVec3 mLinearInertia;
	physx::PxVec3 mAngularInertia;
	physx::PxVec3 mCentrifugalInertia;
	float mSolverFrequency;
	float mStiffnessFrequency;

	physx::PxTransform mTargetMotion;
	physx::PxTransform mCurrentMotion;
	physx::PxVec3 mLinearVelocity;
	physx::PxVec3 mAngularVelocity;
	bool mIgnoreVelocityDiscontinuityNextFrame;

	float mPrevIterDt;
	MovingAverage mIterDtAvg;

	// wind
	physx::PxVec3 mWind;
	float mDragLogCoefficient;
	float mLiftLogCoefficient;
	float mFluidDensity;

	// sleeping
	uint32_t mSleepTestInterval; // how often to test for movement
	uint32_t mSleepAfterCount;   // number of tests to pass before sleep
	float mSleepThreshold;       // max movement delta to pass test
	uint32_t mSleepPassCounter;  // how many tests passed
	uint32_t mSleepTestCounter;  // how many iterations since tested
};

template <typename T>
inline Fabric& ClothImpl<T>::getFabric() const
{
	return getChildCloth()->mFabric;
}

template <typename T>
inline Factory& ClothImpl<T>::getFactory() const
{
	return getChildCloth()->mFactory;
}

template <typename T>
inline void ClothImpl<T>::setTranslation(const physx::PxVec3& trans)
{
	physx::PxVec3 t = reinterpret_cast<const physx::PxVec3&>(trans);
	if (t == mTargetMotion.p)
		return;

	mTargetMotion.p = t;
	wakeUp();
}

template <typename T>
inline void ClothImpl<T>::setRotation(const physx::PxQuat& q)
{
	if ((q - mTargetMotion.q).magnitudeSquared() == 0.0f)
		return;

	mTargetMotion.q = q;
	wakeUp();
}

template <typename T>
inline const physx::PxVec3& ClothImpl<T>::getTranslation() const
{
	return mTargetMotion.p;
}

template <typename T>
inline const physx::PxQuat& ClothImpl<T>::getRotation() const
{
	return mTargetMotion.q;
}

template <typename T>
inline void ClothImpl<T>::clearInertia()
{
	mCurrentMotion = mTargetMotion;
	mLinearVelocity = physx::PxVec3(0.0);
	mAngularVelocity = physx::PxVec3(0.0);

	wakeUp();
}

// Fixed 4505:local function has been removed
template <typename T>
inline void ClothImpl<T>::teleport(const physx::PxVec3& delta)
{
	mCurrentMotion.p += delta;
	mTargetMotion.p += delta;
}

template <typename T>
inline void ClothImpl<T>::teleportToLocation(const physx::PxVec3& translation, const physx::PxQuat& rotation)
{
	mCurrentMotion.p = translation;
	mCurrentMotion.q = rotation;
	mTargetMotion.p = translation;
	mTargetMotion.q = rotation;
}

template <typename T>
inline void ClothImpl<T>::ignoreVelocityDiscontinuity()
{
	mIgnoreVelocityDiscontinuityNextFrame = true;
}

template <typename T>
inline float ClothImpl<T>::getPreviousIterationDt() const
{
	return mPrevIterDt;
}

template <typename T>
inline void ClothImpl<T>::setGravity(const physx::PxVec3& gravity)
{
	physx::PxVec3 value = gravity;
	if (value == mGravity)
		return;

	mGravity = value;
	wakeUp();
}

template <typename T>
inline physx::PxVec3 ClothImpl<T>::getGravity() const
{
	return mGravity;
}

inline float safeLog2(float x)
{
	NV_CLOTH_ASSERT_WITH_MESSAGE("safeLog2",x >= 0.0f);
	return x > 0 ? physx::shdfnd::log2(x) : -FLT_MAX_EXP;
}

inline physx::PxVec3 safeLog2(const physx::PxVec3& v)
{
	return physx::PxVec3( safeLog2(v.x), safeLog2(v.y), safeLog2(v.z) );
}

inline float safeExp2(float x)
{
	if (x <= -FLT_MAX_EXP)
		return 0.0f;
	else
		return physx::shdfnd::exp2(x);
}

inline physx::PxVec3 safeExp2(const physx::PxVec3& v)
{
	return physx::PxVec3( safeExp2(v.x), safeExp2(v.y), safeExp2(v.z) );
}

template <typename T>
inline void ClothImpl<T>::setDamping(const physx::PxVec3& damping)
{
	physx::PxVec3 value = safeLog2(physx::PxVec3(1.f) - damping);
	if (value == mLogDamping)
		return;

	mLogDamping = value;
	wakeUp();
}

template <typename T>
inline physx::PxVec3 ClothImpl<T>::getDamping() const
{
	return physx::PxVec3(1.f) - safeExp2(mLogDamping);
}

template <typename T>
inline void ClothImpl<T>::setLinearDrag(const physx::PxVec3& drag)
{
	physx::PxVec3 value = safeLog2(physx::PxVec3(1.f) - drag);
	if (value == mLinearLogDrag)
		return;

	mLinearLogDrag = value;
	wakeUp();
}

template <typename T>
inline physx::PxVec3 ClothImpl<T>::getLinearDrag() const
{
	return physx::PxVec3(1.f) - safeExp2(mLinearLogDrag);
}

template <typename T>
inline void ClothImpl<T>::setAngularDrag(const physx::PxVec3& drag)
{
	physx::PxVec3 value = safeLog2(physx::PxVec3(1.f) - drag);
	if (value == mAngularLogDrag)
		return;

	mAngularLogDrag = value;
	wakeUp();
}

template <typename T>
inline physx::PxVec3 ClothImpl<T>::getAngularDrag() const
{
	return physx::PxVec3(1.f) - safeExp2(mAngularLogDrag);
}

template <typename T>
inline void ClothImpl<T>::setLinearInertia(const physx::PxVec3& inertia)
{
	physx::PxVec3 value = inertia;
	if (value == mLinearInertia)
		return;

	mLinearInertia = value;
	wakeUp();
}

template <typename T>
inline physx::PxVec3 ClothImpl<T>::getLinearInertia() const
{
	return mLinearInertia;
}

template <typename T>
inline void ClothImpl<T>::setAngularInertia(const physx::PxVec3& inertia)
{
	physx::PxVec3 value = inertia;
	if (value == mAngularInertia)
		return;

	mAngularInertia = value;
	wakeUp();
}

template <typename T>
inline physx::PxVec3 ClothImpl<T>::getAngularInertia() const
{
	return mAngularInertia;
}

template <typename T>
inline void ClothImpl<T>::setCentrifugalInertia(const physx::PxVec3& inertia)
{
	physx::PxVec3 value = inertia;
	if (value == mCentrifugalInertia)
		return;

	mCentrifugalInertia = value;
	wakeUp();
}

template <typename T>
inline physx::PxVec3 ClothImpl<T>::getCentrifugalInertia() const
{
	return mCentrifugalInertia;
}

template <typename T>
inline void ClothImpl<T>::setSolverFrequency(float frequency)
{
	if (frequency == mSolverFrequency)
		return;

	mSolverFrequency = frequency;
	getChildCloth()->mClothCostDirty = true;
	mIterDtAvg.reset();
	wakeUp();
}

template <typename T>
inline float ClothImpl<T>::getSolverFrequency() const
{
	return mSolverFrequency;
}

template <typename T>
inline void ClothImpl<T>::setStiffnessFrequency(float frequency)
{
	if (frequency == mStiffnessFrequency)
		return;

	mStiffnessFrequency = frequency;
	wakeUp();
}

template <typename T>
inline float ClothImpl<T>::getStiffnessFrequency() const
{
	return mStiffnessFrequency;
}

template <typename T>
inline void ClothImpl<T>::setAcceleationFilterWidth(uint32_t n)
{
	mIterDtAvg.resize(n);
}

template <typename T>
inline uint32_t ClothImpl<T>::getAccelerationFilterWidth() const
{
	return mIterDtAvg.size();
}

// move a subarray
template <typename Iter>
void move(Iter it, uint32_t first, uint32_t last, uint32_t result)
{
	if (result > first)
	{
		result += last - first;
		while (first < last)
			it[--result] = it[--last];
	}
	else
	{
		while (first < last)
			it[result++] = it[first++];
	}
}

// update capsule index
inline bool updateIndex(uint32_t& index, uint32_t first, int32_t delta)
{
	return index >= first && int32_t(index += delta) < int32_t(first);
}

template <typename T>
inline void ClothImpl<T>::setSpheres(Range<const physx::PxVec4> spheres, uint32_t first, uint32_t last)
{
	uint32_t oldSize = uint32_t(getChildCloth()->mStartCollisionSpheres.size());
	uint32_t newSize = uint32_t(spheres.size()) + oldSize - last + first;

	NV_CLOTH_ASSERT(newSize <= 256);
	NV_CLOTH_ASSERT(first <= oldSize);
	NV_CLOTH_ASSERT(last <= oldSize);

#if PX_DEBUG
	for (const physx::PxVec4* it = spheres.begin(); it < spheres.end(); ++it)
		NV_CLOTH_ASSERT(it->w >= 0.0f);
#endif

	if (!oldSize && !newSize)
		return;

	if (!oldSize)
	{
		ContextLockType contextLock(getChildCloth()->mFactory);
		getChildCloth()->mStartCollisionSpheres.assign(spheres.begin(), spheres.end());
		getChildCloth()->notifyChanged();
	}
	else
	{
		if (std::max(oldSize, newSize) >
		   std::min(getChildCloth()->mStartCollisionSpheres.capacity(), getChildCloth()->mTargetCollisionSpheres.capacity()))
		{
			ContextLockType contextLock(getChildCloth()->mFactory);
			getChildCloth()->mStartCollisionSpheres.reserve(newSize);
			getChildCloth()->mTargetCollisionSpheres.reserve(std::max(oldSize, newSize));
		}

		typename T::MappedVec4fVectorType start = getChildCloth()->mStartCollisionSpheres;
		typename T::MappedVec4fVectorType target = getChildCloth()->mTargetCollisionSpheres;

		// fill target from start
		for (uint32_t i = uint32_t(target.size()); i < oldSize; ++i)
			target.pushBack(start[i]);

		// resize to larger of oldSize and newSize
		start.resize(std::max(oldSize, newSize), physx::PxVec4(0.0f));
		target.resize(std::max(oldSize, newSize), physx::PxVec4(0.0f));

		if (int32_t delta = int32_t(newSize - oldSize))
		{
			// move past-range elements to new place
			move(start.begin(), last, oldSize, last + delta);
			move(target.begin(), last, oldSize, last + delta);

			// fill new elements from spheres
			for (uint32_t i = last; i < last + delta; ++i)
				start[i] = spheres[i - first];

			// adjust capsule indices
			typename T::MappedIndexVectorType indices = getChildCloth()->mCapsuleIndices;
			Vector<IndexPair>::Type::Iterator cIt, cEnd = indices.end();
			for (cIt = indices.begin(); cIt != cEnd;)
			{
				bool removed = false;
				removed |= updateIndex(cIt->first, last + std::min(0, delta), int32_t(delta));
				removed |= updateIndex(cIt->second, last + std::min(0, delta), int32_t(delta));
				if (!removed)
					++cIt;
				else
				{
					indices.replaceWithLast(cIt);
					cEnd = indices.end();
				}
			}

			start.resize(newSize);
			target.resize(newSize);

			getChildCloth()->notifyChanged();
		}

		// fill target elements with spheres
		for (uint32_t i = 0; i < spheres.size(); ++i)
			target[first + i] = spheres[i];
	}

	getChildCloth()->wakeUp();
}

template <typename T>
inline void ClothImpl<T>::setSpheres(Range<const physx::PxVec4> startSpheres, Range<const physx::PxVec4> targetSpheres)
{
	NV_CLOTH_ASSERT(startSpheres.size() == targetSpheres.size());

	//Clamp ranges to the first 32 spheres
	startSpheres = Range<const physx::PxVec4>(startSpheres.begin(), std::min(startSpheres.end(), startSpheres.begin() + 32));
	targetSpheres = Range<const physx::PxVec4>(targetSpheres.begin(), std::min(targetSpheres.end(), targetSpheres.begin() + 32));

	uint32_t oldSize = uint32_t(getChildCloth()->mStartCollisionSpheres.size());
	uint32_t newSize = uint32_t(startSpheres.size());

	if(newSize > std::min(getChildCloth()->mStartCollisionSpheres.capacity(), getChildCloth()->mTargetCollisionSpheres.capacity()))
	{
		//context lock only if we are growing the array
		ContextLockType contextLock(getChildCloth()->mFactory);
		getChildCloth()->mStartCollisionSpheres.assign(startSpheres.begin(), startSpheres.end());
		getChildCloth()->mTargetCollisionSpheres.assign(targetSpheres.begin(), targetSpheres.end());
		getChildCloth()->notifyChanged();
	}
	else
	{
		getChildCloth()->mStartCollisionSpheres.assign(startSpheres.begin(), startSpheres.end());
		getChildCloth()->mTargetCollisionSpheres.assign(targetSpheres.begin(), targetSpheres.end());

		if(newSize - oldSize) //notify only if the size changed
			getChildCloth()->notifyChanged();
	}

	wakeUp();
}

template <typename T>
inline uint32_t ClothImpl<T>::getNumSpheres() const
{
	return uint32_t(getChildCloth()->mStartCollisionSpheres.size());
}

// Fixed 4505:local function has been removed
template <typename T>
inline void ClothImpl<T>::setCapsules(Range<const uint32_t> capsules, uint32_t first, uint32_t last)
{
	const IndexPair* srcIndices = reinterpret_cast<const IndexPair*>(capsules.begin());
	const uint32_t srcIndicesSize = uint32_t(capsules.size() / 2);

	uint32_t oldSize = uint32_t(getChildCloth()->mCapsuleIndices.size());
	uint32_t newSize = srcIndicesSize + oldSize - last + first;

	NV_CLOTH_ASSERT(newSize <= 256);
	NV_CLOTH_ASSERT(first <= oldSize);
	NV_CLOTH_ASSERT(last <= oldSize);

	if (getChildCloth()->mCapsuleIndices.capacity() < newSize)
	{
		ContextLockType contextLock(getChildCloth()->mFactory);
		getChildCloth()->mCapsuleIndices.reserve(newSize);
	}

	// resize to larger of oldSize and newSize
	getChildCloth()->mCapsuleIndices.resize(std::max(oldSize, newSize));

	typename T::MappedIndexVectorType dstIndices = getChildCloth()->mCapsuleIndices;

	if (uint32_t delta = newSize - oldSize)
	{
		// move past-range elements to new place
		move(dstIndices.begin(), last, oldSize, last + delta);

		// fill new elements from capsules
		for (uint32_t i = last; i < last + delta; ++i)
			dstIndices[i] = srcIndices[i - first];

		dstIndices.resize(newSize);
		getChildCloth()->notifyChanged();
	}

	// fill existing elements from capsules
	for (uint32_t i = 0; i < srcIndicesSize; ++i)
		dstIndices[first + i] = srcIndices[i];

	getChildCloth()->wakeUp();
}

template <typename T>
inline uint32_t ClothImpl<T>::getNumCapsules() const
{
	return uint32_t(getChildCloth()->mCapsuleIndices.size());
}

template <typename T>
inline void ClothImpl<T>::setPlanes(Range<const physx::PxVec4> planes, uint32_t first, uint32_t last)
{
	uint32_t oldSize = uint32_t(getChildCloth()->mStartCollisionPlanes.size());
	uint32_t newSize = uint32_t(planes.size()) + oldSize - last + first;

	NV_CLOTH_ASSERT(newSize <= 32);
	NV_CLOTH_ASSERT(first <= oldSize);
	NV_CLOTH_ASSERT(last <= oldSize);
#if PX_DEBUG || PX_CHECKED
	int logCount = 0;
	for (int i = 0; i<static_cast<int>(planes.size()); i++)
	{
        if (fabsf(planes[i].getXYZ().magnitudeSquared() - 1.0f) > 0.01f)
		{
			if (logCount == 0)
				NV_CLOTH_LOG_INVALID_PARAMETER("The plane normals passed to Cloth::setPlanes are not normalized. First error encounterd at plane %d (%f, %f, %f, %f)",
					i, static_cast<double>(planes[i].x), static_cast<double>(planes[i].y), static_cast<double>(planes[i].z), static_cast<double>(planes[i].w));
			logCount++;
		}
	}
	if (logCount>1)
	{
		NV_CLOTH_LOG_INVALID_PARAMETER("This error was encountered %d more times.", logCount-1);
	}
#endif

	if (!oldSize && !newSize)
		return;

	if (!oldSize)
	{
		ContextLockType contextLock(getChildCloth()->mFactory);
		getChildCloth()->mStartCollisionPlanes.assign(planes.begin(), planes.end());
		getChildCloth()->notifyChanged();
	}
	else
	{
		if (std::max(oldSize, newSize) >
		   std::min(getChildCloth()->mStartCollisionPlanes.capacity(), getChildCloth()->mTargetCollisionPlanes.capacity()))
		{
			ContextLockType contextLock(getChildCloth()->mFactory);
			getChildCloth()->mStartCollisionPlanes.reserve(newSize);
			getChildCloth()->mTargetCollisionPlanes.reserve(std::max(oldSize, newSize));
		}

		typename T::MappedVec4fVectorType start = getChildCloth()->mStartCollisionPlanes;
		typename T::MappedVec4fVectorType target = getChildCloth()->mTargetCollisionPlanes;

		// fill target from start
		for (uint32_t i = target.size(); i < oldSize; ++i)
			target.pushBack(start[i]);

		// resize to larger of oldSize and newSize
		start.resize(std::max(oldSize, newSize), physx::PxVec4(0.0f));
		target.resize(std::max(oldSize, newSize), physx::PxVec4(0.0f));

		if (int32_t delta = int32_t(newSize - oldSize))
		{
			// move past-range elements to new place
			move(start.begin(), last, oldSize, last + delta);
			move(target.begin(), last, oldSize, last + delta);

			// fill new elements from planes
			for (uint32_t i = last; i < last + delta; ++i)
				start[i] = planes[i - first];

			// adjust convex indices
			uint32_t mask = (uint32_t(1) << (last + std::min(delta, 0))) - 1;
			typename T::MappedMaskVectorType masks = getChildCloth()->mConvexMasks;
			Vector<uint32_t>::Type::Iterator cIt, cEnd = masks.end();
			for (cIt = masks.begin(); cIt != cEnd;)
			{
				uint32_t convex = (*cIt & mask);
				if (delta < 0)
					convex |= *cIt >> -delta & ~mask;
				else
					convex |= (*cIt & ~mask) << delta;
				if (convex)
					*cIt++ = convex;
				else
				{
					masks.replaceWithLast(cIt);
					cEnd = masks.end();
				}
			}

			start.resize(newSize);
			target.resize(newSize);

			getChildCloth()->notifyChanged();
		}

		// fill target elements with planes
		for (uint32_t i = 0; i < planes.size(); ++i)
			target[first + i] = planes[i];
	}

	wakeUp();
}

template <typename T>
inline void ClothImpl<T>::setPlanes(Range<const physx::PxVec4> startPlanes, Range<const physx::PxVec4> targetPlanes)
{
	NV_CLOTH_ASSERT(startPlanes.size() == targetPlanes.size());

	//Clamp ranges to the first 32 planes
	startPlanes = Range<const physx::PxVec4>(startPlanes.begin(), std::min(startPlanes.end(), startPlanes.begin() + 32));
	targetPlanes = Range<const physx::PxVec4>(targetPlanes.begin(), std::min(targetPlanes.end(), targetPlanes.begin() + 32));

	uint32_t oldSize = uint32_t(getChildCloth()->mStartCollisionPlanes.size());
	uint32_t newSize = uint32_t(startPlanes.size());

	if(newSize > std::min(getChildCloth()->mStartCollisionPlanes.capacity(), getChildCloth()->mTargetCollisionPlanes.capacity()))
	{
		//context lock only if we are growing the array
		ContextLockType contextLock(getChildCloth()->mFactory);
		getChildCloth()->mStartCollisionPlanes.assign(startPlanes.begin(), startPlanes.end());
		getChildCloth()->mTargetCollisionPlanes.assign(targetPlanes.begin(), targetPlanes.end());
		getChildCloth()->notifyChanged();
	}
	else
	{
		getChildCloth()->mStartCollisionPlanes.assign(startPlanes.begin(), startPlanes.end());
		getChildCloth()->mTargetCollisionPlanes.assign(targetPlanes.begin(), targetPlanes.end());

		if(newSize - oldSize) //notify only if the size changed
			getChildCloth()->notifyChanged();
	}

	wakeUp();
}

template <typename T>
inline uint32_t ClothImpl<T>::getNumPlanes() const
{
	return uint32_t(getChildCloth()->mStartCollisionPlanes.size());
}

template <typename T>
inline void ClothImpl<T>::setConvexes(Range<const uint32_t> convexMasks, uint32_t first, uint32_t last)
{
	uint32_t oldSize = uint32_t(getChildCloth()->mConvexMasks.size());
	uint32_t newSize = uint32_t(convexMasks.size()) + oldSize - last + first;

	NV_CLOTH_ASSERT(newSize <= 32);
	NV_CLOTH_ASSERT(first <= oldSize);
	NV_CLOTH_ASSERT(last <= oldSize);
#if PX_DEBUG || PX_CHECKED
	for (int i = 0; i<static_cast<int>(convexMasks.size()); i++)
	{
		if (convexMasks[i] == 0)
		{
			NV_CLOTH_LOG_INVALID_PARAMETER("Cloth::setConvexes expects bit masks of the form (1<<planeIndex1)|(1<<planeIndex2). 0 is not a valid mask/plane index. Error found in location %d", i);
			continue;
		}
	}
#endif

	if (getChildCloth()->mConvexMasks.capacity() < newSize)
	{
		ContextLockType contextLock(getChildCloth()->mFactory);
		getChildCloth()->mConvexMasks.reserve(newSize);
	}

	// resize to larger of oldSize and newSize
	getChildCloth()->mConvexMasks.resize(std::max(oldSize, newSize));

	if (uint32_t delta = newSize - oldSize)
	{
		typename T::MappedMaskVectorType masks = getChildCloth()->mConvexMasks;

		// move past-range elements to new place
		move(masks.begin(), last, oldSize, last + delta);

		// fill new elements from capsules
		for (uint32_t i = last; i < last + delta; ++i)
			masks[i] = convexMasks[i - first];

		masks.resize(newSize);
		getChildCloth()->notifyChanged();
	}

	wakeUp();
}

template <typename T>
inline uint32_t ClothImpl<T>::getNumConvexes() const
{
	return uint32_t(getChildCloth()->mConvexMasks.size());
}

template <typename T>
inline void ClothImpl<T>::setTriangles(Range<const physx::PxVec3> triangles, uint32_t first, uint32_t last)
{
	// convert from triangle to vertex count
	first *= 3;
	last *= 3;

	triangles = getChildCloth()->clampTriangleCount(triangles, last - first);
	NV_CLOTH_ASSERT(0 == triangles.size() % 3);

	uint32_t oldSize = uint32_t(getChildCloth()->mStartCollisionTriangles.size());
	uint32_t newSize = uint32_t(triangles.size()) + oldSize - last + first;

	NV_CLOTH_ASSERT(first <= oldSize);
	NV_CLOTH_ASSERT(last <= oldSize);

	if (!oldSize && !newSize)
		return;

	if (!oldSize)
	{
		ContextLockType contextLock(getChildCloth()->mFactory);
		getChildCloth()->mStartCollisionTriangles.assign(triangles.begin(), triangles.end());
		getChildCloth()->notifyChanged();
	}
	else
	{
		if (std::max(oldSize, newSize) >
			std::min(getChildCloth()->mStartCollisionTriangles.capacity(), getChildCloth()->mTargetCollisionTriangles.capacity()))
		{
			ContextLockType contextLock(getChildCloth()->mFactory);
			getChildCloth()->mStartCollisionTriangles.reserve(newSize);
			getChildCloth()->mTargetCollisionTriangles.reserve(std::max(oldSize, newSize));
		}

		typename T::MappedVec3fVectorType start = getChildCloth()->mStartCollisionTriangles;
		typename T::MappedVec3fVectorType target = getChildCloth()->mTargetCollisionTriangles;

		// fill target from start
		for (uint32_t i = target.size(); i < oldSize; ++i)
			target.pushBack(start[i]);

		// resize to larger of oldSize and newSize
		start.resize(std::max(oldSize, newSize), physx::PxVec3(0.0f));
		target.resize(std::max(oldSize, newSize), physx::PxVec3(0.0f));

		if (int32_t delta = int32_t(newSize - oldSize))
		{
			// move past-range elements to new place
			move(start.begin(), last, oldSize, last + delta);
			move(target.begin(), last, oldSize, last + delta);

			// fill new elements from triangles
			for (uint32_t i = last; i < last + delta; ++i)
				start[i] = triangles[i - first];

			start.resize(newSize);
			target.resize(newSize);

			getChildCloth()->notifyChanged();
		}
		//////////////////////

	//	if (std::max(oldSize, newSize) >
	//	   std::min(getChildCloth()->mStartCollisionTriangles.capacity(), getChildCloth()->mTargetCollisionTriangles.capacity()))
	//	{
	//		ContextLockType contextLock(getChildCloth()->mFactory);
	//		getChildCloth()->mStartCollisionTriangles.reserve(newSize);
	//		getChildCloth()->mTargetCollisionTriangles.reserve(std::max(oldSize, newSize));
	//	}
	//
	//	// fill target from start
	//	for (uint32_t i = getChildCloth()->mTargetCollisionTriangles.size(); i < oldSize; ++i)
	//		getChildCloth()->mTargetCollisionTriangles.pushBack(getChildCloth()->mStartCollisionTriangles[i]);
	//
	//	// resize to larger of oldSize and newSize
	//	getChildCloth()->mStartCollisionTriangles.resize(std::max(oldSize, newSize));
	//	getChildCloth()->mTargetCollisionTriangles.resize(std::max(oldSize, newSize));
	//
	//	if (uint32_t delta = newSize - oldSize)
	//	{
	//		// move past-range elements to new place
	//		move(getChildCloth()->mStartCollisionTriangles.begin(), last, oldSize, last + delta);
	//		move(getChildCloth()->mTargetCollisionTriangles.begin(), last, oldSize, last + delta);
	//
	//		// fill new elements from triangles
	//		for (uint32_t i = last; i < last + delta; ++i)
	//			getChildCloth()->mStartCollisionTriangles[i] = triangles[i - first];
	//
	//		getChildCloth()->mStartCollisionTriangles.resize(newSize);
	//		getChildCloth()->mTargetCollisionTriangles.resize(newSize);
	//
	//		notifyChanged();
	//	}

		// fill target elements with triangles
		//	for (uint32_t i = 0; i < triangles.size(); ++i)
		//		getChildCloth()->mTargetCollisionTriangles[first + i] = triangles[i];

		// fill target elements with triangles
		for (uint32_t i = 0; i < triangles.size(); ++i)
			target[first + i] = triangles[i];
	}

	wakeUp();
}

template <typename T>
inline void ClothImpl<T>::setTriangles(Range<const physx::PxVec3> startTriangles, Range<const physx::PxVec3> targetTriangles,
                                       uint32_t first)
{
	NV_CLOTH_ASSERT(startTriangles.size() == targetTriangles.size());

	// convert from triangle to vertex count
	first *= 3;

	uint32_t last = uint32_t(getChildCloth()->mStartCollisionTriangles.size());

	startTriangles = getChildCloth()->clampTriangleCount(startTriangles, last - first);
	targetTriangles = getChildCloth()->clampTriangleCount(targetTriangles, last - first);

	uint32_t oldSize = uint32_t(getChildCloth()->mStartCollisionTriangles.size());
	uint32_t newSize = uint32_t(startTriangles.size()) + oldSize - last + first;

	NV_CLOTH_ASSERT(first <= oldSize);
	NV_CLOTH_ASSERT(last == oldSize); // this path only supports replacing the tail

	if (!oldSize && !newSize)
		return;

	if (newSize > std::min(getChildCloth()->mStartCollisionTriangles.capacity(), getChildCloth()->mTargetCollisionTriangles.capacity()))
	{
		ContextLockType contextLock(getChildCloth()->mFactory);
		getChildCloth()->mStartCollisionTriangles.assign(startTriangles.begin(), startTriangles.end());
		getChildCloth()->mTargetCollisionTriangles.assign(targetTriangles.begin(), targetTriangles.end());
		getChildCloth()->notifyChanged();
	}
	else
	{
		uint32_t retainSize = oldSize - last + first;
		getChildCloth()->mStartCollisionTriangles.resize(retainSize);
		getChildCloth()->mTargetCollisionTriangles.resize(retainSize);

		getChildCloth()->mStartCollisionTriangles.assign(startTriangles.begin(), startTriangles.end());
		getChildCloth()->mTargetCollisionTriangles.assign(targetTriangles.begin(), targetTriangles.end());

		if (newSize - oldSize)
			getChildCloth()->notifyChanged();
	}

	wakeUp();
}

template <typename T>
inline uint32_t ClothImpl<T>::getNumTriangles() const
{
	return uint32_t(getChildCloth()->mStartCollisionTriangles.size()) / 3;
}

template <typename T>
inline bool ClothImpl<T>::isContinuousCollisionEnabled() const
{
	return getChildCloth()->mEnableContinuousCollision;
}

template <typename T>
inline void ClothImpl<T>::enableContinuousCollision(bool enable)
{
	if (enable == getChildCloth()->mEnableContinuousCollision)
		return;

	getChildCloth()->mEnableContinuousCollision = enable;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline float ClothImpl<T>::getCollisionMassScale() const
{
	return getChildCloth()->mCollisionMassScale;
}

template <typename T>
inline void ClothImpl<T>::setCollisionMassScale(float scale)
{
	if (scale == getChildCloth()->mCollisionMassScale)
		return;

	getChildCloth()->mCollisionMassScale = scale;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline void ClothImpl<T>::setFriction(float friction)
{
	getChildCloth()->mFriction = friction;
	getChildCloth()->notifyChanged();
}

template <typename T>
inline float ClothImpl<T>::getFriction() const
{
	return getChildCloth()->mFriction;
}

template <typename T>
inline uint32_t ClothImpl<T>::getNumVirtualParticleWeights() const
{
	return uint32_t(getChildCloth()->mVirtualParticleWeights.size());
}

template <typename T>
inline void ClothImpl<T>::setTetherConstraintScale(float scale)
{
	if (scale == getChildCloth()->mTetherConstraintScale)
		return;

	getChildCloth()->mTetherConstraintScale = scale;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline float ClothImpl<T>::getTetherConstraintScale() const
{
	return getChildCloth()->mTetherConstraintScale;
}

template <typename T>
inline void ClothImpl<T>::setTetherConstraintStiffness(float stiffness)
{
	float value = safeLog2(1 - stiffness);
	if (value == getChildCloth()->mTetherConstraintLogStiffness)
		return;

	getChildCloth()->mTetherConstraintLogStiffness = value;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline float ClothImpl<T>::getTetherConstraintStiffness() const
{
	return 1.f - safeExp2(getChildCloth()->mTetherConstraintLogStiffness);
}

template <typename T>
inline Range<physx::PxVec4> ClothImpl<T>::getMotionConstraints()
{
	wakeUp();
	return getChildCloth()->push(getChildCloth()->mMotionConstraints);
}

template <typename T>
inline void ClothImpl<T>::clearMotionConstraints()
{
	getChildCloth()->clear(getChildCloth()->mMotionConstraints);
	wakeUp();
}

template <typename T>
inline uint32_t ClothImpl<T>::getNumMotionConstraints() const
{
	return uint32_t(getChildCloth()->mMotionConstraints.mStart.size());
}

template <typename T>
inline void ClothImpl<T>::setMotionConstraintScaleBias(float scale, float bias)
{
	if (scale == getChildCloth()->mMotionConstraintScale && bias == getChildCloth()->mMotionConstraintBias)
		return;

	getChildCloth()->mMotionConstraintScale = scale;
	getChildCloth()->mMotionConstraintBias = bias;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline float ClothImpl<T>::getMotionConstraintScale() const
{
	return getChildCloth()->mMotionConstraintScale;
}

template <typename T>
inline float ClothImpl<T>::getMotionConstraintBias() const
{
	return getChildCloth()->mMotionConstraintBias;
}

template <typename T>
inline void ClothImpl<T>::setMotionConstraintStiffness(float stiffness)
{
	float value = safeLog2(1 - stiffness);
	if (value == getChildCloth()->mMotionConstraintLogStiffness)
		return;

	getChildCloth()->mMotionConstraintLogStiffness = value;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline float ClothImpl<T>::getMotionConstraintStiffness() const
{
	return 1.f - safeExp2(getChildCloth()->mMotionConstraintLogStiffness);
}

template <typename T>
inline Range<physx::PxVec4> ClothImpl<T>::getSeparationConstraints()
{
	wakeUp();
	return getChildCloth()->push(getChildCloth()->mSeparationConstraints);
}

template <typename T>
inline void ClothImpl<T>::clearSeparationConstraints()
{
	getChildCloth()->clear(getChildCloth()->mSeparationConstraints);
	wakeUp();
}

template <typename T>
inline void ClothImpl<T>::clearInterpolation()
{
	if (!getChildCloth()->mTargetCollisionSpheres.empty())
	{
		ContextLockType contextLock(getChildCloth()->mFactory);
		physx::shdfnd::swap(getChildCloth()->mStartCollisionSpheres, getChildCloth()->mTargetCollisionSpheres);
		getChildCloth()->mTargetCollisionSpheres.resize(0);
	}
	getChildCloth()->mMotionConstraints.pop();
	getChildCloth()->mSeparationConstraints.pop();
	wakeUp();
}

template <typename T>
inline uint32_t ClothImpl<T>::getNumSeparationConstraints() const
{
	return uint32_t(getChildCloth()->mSeparationConstraints.mStart.size());
}

template <typename T>
inline uint32_t ClothImpl<T>::getNumParticleAccelerations() const
{
	return uint32_t(getChildCloth()->mParticleAccelerations.size());
}

template <typename T>
inline void ClothImpl<T>::setWindVelocity(physx::PxVec3 wind)
{
	if (wind == mWind)
		return;

	mWind = wind;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline physx::PxVec3 ClothImpl<T>::getWindVelocity() const
{
	return mWind;
}

template <typename T>
inline void ClothImpl<T>::setDragCoefficient(float coefficient)
{
	NV_CLOTH_ASSERT(coefficient <= 1.f);

	float value = safeLog2(1.f - coefficient);
	if (value == mDragLogCoefficient)
		return;

	mDragLogCoefficient = value;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline float ClothImpl<T>::getDragCoefficient() const
{
	return 1.f - safeExp2(mDragLogCoefficient);
}

template <typename T>
inline void ClothImpl<T>::setLiftCoefficient(float coefficient)
{
	NV_CLOTH_ASSERT(coefficient <= 1.f);

	float value = safeLog2(1.f - coefficient);
	if (value == mLiftLogCoefficient)
		return;

	mLiftLogCoefficient = value;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline float ClothImpl<T>::getLiftCoefficient() const
{
	return 1.f - safeExp2(mLiftLogCoefficient);
}

template <typename T>
inline void ClothImpl<T>::setFluidDensity(float fluidDensity)
{
	NV_CLOTH_ASSERT(fluidDensity > 0.f);
	if (fluidDensity == mFluidDensity)
		return;

	mFluidDensity = fluidDensity;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline float ClothImpl<T>::getFluidDensity() const
{
	return mFluidDensity;
}

template <typename T>
inline uint32_t ClothImpl<T>::getNumSelfCollisionIndices() const
{
	return uint32_t(getChildCloth()->mSelfCollisionIndices.size());
}


// Fixed 4505:local function has been removed
template <typename T>
inline void ClothImpl<T>::setRestPositions(Range<const physx::PxVec4> restPositions)
{
	NV_CLOTH_ASSERT(restPositions.empty() || restPositions.size() == getNumParticles());
	ContextLockType contextLock(getChildCloth()->mFactory);
	getChildCloth()->mRestPositions.assign(restPositions.begin(), restPositions.end());
	wakeUp();
}

template <typename T>
inline uint32_t ClothImpl<T>::getNumRestPositions() const
{
	return uint32_t(getChildCloth()->mRestPositions.size());
}

template <typename T>
inline void ClothImpl<T>::setSelfCollisionDistance(float distance)
{
	if (distance == getChildCloth()->mSelfCollisionDistance)
		return;

	getChildCloth()->mSelfCollisionDistance = distance;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline float ClothImpl<T>::getSelfCollisionDistance() const
{
	return getChildCloth()->mSelfCollisionDistance;
}

template <typename T>
inline void ClothImpl<T>::setSelfCollisionStiffness(float stiffness)
{
	NV_CLOTH_ASSERT(stiffness <= 1.0f);
	float value = safeLog2(1 - stiffness);
	if (value == getChildCloth()->mSelfCollisionLogStiffness)
		return;

	getChildCloth()->mSelfCollisionLogStiffness = value;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline float ClothImpl<T>::getSelfCollisionStiffness() const
{
	return 1.f - safeExp2(getChildCloth()->mSelfCollisionLogStiffness);
}

template <typename T>
inline const physx::PxVec3& ClothImpl<T>::getBoundingBoxCenter() const
{
	return getChildCloth()->mParticleBoundsCenter;
}

template <typename T>
inline const physx::PxVec3& ClothImpl<T>::getBoundingBoxScale() const
{
	return getChildCloth()->mParticleBoundsHalfExtent;
}

template <typename T>
inline void ClothImpl<T>::setSleepThreshold(float threshold)
{
	if (threshold == mSleepThreshold)
		return;

	mSleepThreshold = threshold;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline float ClothImpl<T>::getSleepThreshold() const
{
	return mSleepThreshold;
}

template <typename T>
inline void ClothImpl<T>::setSleepTestInterval(uint32_t interval)
{
	if (interval == mSleepTestInterval)
		return;

	mSleepTestInterval = interval;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline uint32_t ClothImpl<T>::getSleepTestInterval() const
{
	return mSleepTestInterval;
}

template <typename T>
inline void ClothImpl<T>::setSleepAfterCount(uint32_t afterCount)
{
	if (afterCount == mSleepAfterCount)
		return;

	mSleepAfterCount = afterCount;
	getChildCloth()->notifyChanged();
	wakeUp();
}

template <typename T>
inline uint32_t ClothImpl<T>::getSleepAfterCount() const
{
	return mSleepAfterCount;
}

template <typename T>
inline uint32_t ClothImpl<T>::getSleepPassCount() const
{
	return mSleepPassCounter;
}

template <typename T>
inline bool ClothImpl<T>::isAsleep() const
{
	return isSleeping();
}

template <typename T>
inline void ClothImpl<T>::putToSleep()
{
	mSleepPassCounter = mSleepAfterCount;
}

template <typename T>
inline bool ClothImpl<T>::isSleeping() const
{
	return mSleepPassCounter >= mSleepAfterCount;
}

template <typename T>
inline void ClothImpl<T>::wakeUp()
{
	mSleepPassCounter = 0;
}

template <typename T>
inline void ClothImpl<T>::setUserData(void* data)
{
	getChildCloth()->mUserData = data;
}

template <typename T>
inline void* ClothImpl<T>::getUserData() const
{
	return getChildCloth()->mUserData;
}

template <typename T>
template <typename U>
inline MappedRange<U> ClothImpl<T>::getMappedParticles(U* data) const
{
	return MappedRange<U>(data, data + getNumParticles(), *this, &Cloth::lockParticles, &Cloth::unlockParticles);
}


} // namespace cloth
} // namespace nv
