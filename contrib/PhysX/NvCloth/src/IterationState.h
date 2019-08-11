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

#include "Simd.h"
#include <foundation/PxVec4.h>
#include <foundation/PxVec3.h>
#include <foundation/PxMat44.h>
#include <foundation/PxMat33.h>
#include "Vec4T.h"
#include <algorithm>
#include "NvCloth/ps/PsMathUtils.h"

namespace nv
{

/* function object to perform solver iterations on one cloth */

// todo: performance optimization: cache this object and test if velocity/iterDt has changed
// c'tor takes about 5% of the iteration time of a 20x20 cloth

namespace cloth
{

/*  helper functions */

template <typename T>
T sqr(const T& x)
{
	return x * x;
}

inline physx::PxVec3 log(const physx::PxQuat& q)
{
	float theta = q.getImaginaryPart().magnitude();
	float scale = theta > PX_EPS_REAL ? physx::PxAsin(theta) / theta : 1.0f;
	scale = physx::intrinsics::fsel(q.w, scale, -scale);
	return physx::PxVec3(q.x * scale, q.y * scale, q.z * scale);
}

inline physx::PxQuat exp(const physx::PxVec3& v)
{
	float theta = v.magnitude();
	float scale = theta > PX_EPS_REAL ? physx::PxSin(theta) / theta : 1.0f;
	return physx::PxQuat(v.x * scale, v.y * scale, v.z * scale, physx::PxCos(theta));
}

template <typename T4f, uint32_t N>
inline void assign(T4f (&columns)[N], const physx::PxMat44& matrix)
{
	for (uint32_t i = 0; i < N; ++i)
		columns[i] = load(nv::cloth::array(matrix[i]));
}

template <typename T4f>
inline T4f transform(const T4f (&columns)[3], const T4f& vec)
{
	return splat<0>(vec) * columns[0] + splat<1>(vec) * columns[1] + splat<2>(vec) * columns[2];
}

template <typename T4f>
inline T4f transform(const T4f (&columns)[3], const T4f& translate, const T4f& vec)
{
	return translate + splat<0>(vec) * columns[0] + splat<1>(vec) * columns[1] + splat<2>(vec) * columns[2];
}

template <typename>
struct IterationState; // forward declaration

struct IterationStateFactory
{
	template <typename MyCloth>
	IterationStateFactory(MyCloth& cloth, float frameDt);

	template <typename T4f, typename MyCloth>
	IterationState<T4f> create(MyCloth const& cloth) const;

	template <typename T4f>
	static T4f lengthSqr(T4f const& v)
	{
		return dot3(v, v);
	}

	template <typename T4f>
	static physx::PxVec3 castToPxVec3(const T4f& v)
	{
		return *reinterpret_cast<const physx::PxVec3*>(reinterpret_cast<const char*>(&v));
	}

	int mNumIterations;
	float mInvNumIterations;
	float mIterDt, mIterDtRatio, mIterDtAverage;
	physx::PxQuat mCurrentRotation;
	physx::PxVec3 mPrevLinearVelocity;
	physx::PxVec3 mPrevAngularVelocity;
};

/* solver iterations helper functor */
template <typename T4f>
struct IterationState
{
	// call after each iteration
	void update();

	inline float getCurrentAlpha() const;
	inline float getPreviousAlpha() const;

  public:
	T4f mRotationMatrix[3]; // should rename to 'mRotation'

	T4f mCurBias;  // in local space
	T4f mPrevBias; // in local space
	T4f mWind;     // delta position per iteration (wind velocity * mIterDt)

	T4f mPrevMatrix[3];
	T4f mCurMatrix[3];
	T4f mDampScaleUpdate;

	// iteration counter
	uint32_t mRemainingIterations;

	// reciprocal total number of iterations
	float mInvNumIterations;

	// time step size per iteration
	float mIterDt;

	bool mIsTurning; // if false, mPositionScale = mPrevMatrix[0]
};

} // namespace cloth

template <typename T4f>
inline float cloth::IterationState<T4f>::getCurrentAlpha() const
{
	return getPreviousAlpha() + mInvNumIterations;
}

template <typename T4f>
inline float cloth::IterationState<T4f>::getPreviousAlpha() const
{
	return 1.0f - mRemainingIterations * mInvNumIterations;
}

template <typename MyCloth>
cloth::IterationStateFactory::IterationStateFactory(MyCloth& cloth, float frameDt)
{
	mNumIterations = std::max(1, int(frameDt * cloth.mSolverFrequency + 0.5f));
	mInvNumIterations = 1.0f / mNumIterations;
	mIterDt = frameDt * mInvNumIterations;

	mIterDtRatio = cloth.mPrevIterDt ? mIterDt / cloth.mPrevIterDt : 1.0f;
	mIterDtAverage = cloth.mIterDtAvg.empty() ? mIterDt : cloth.mIterDtAvg.average();

	mCurrentRotation = cloth.mCurrentMotion.q;
	mPrevLinearVelocity = cloth.mLinearVelocity;
	mPrevAngularVelocity = cloth.mAngularVelocity;

	if(!cloth.mIgnoreVelocityDiscontinuityNextFrame)
	{
		// update cloth
		float invFrameDt = 1.0f / frameDt;
		cloth.mLinearVelocity = invFrameDt * (cloth.mTargetMotion.p - cloth.mCurrentMotion.p);
		physx::PxQuat dq = cloth.mTargetMotion.q * cloth.mCurrentMotion.q.getConjugate();
		cloth.mAngularVelocity = log(dq) * invFrameDt;
	}
	cloth.mIgnoreVelocityDiscontinuityNextFrame = false;

	cloth.mPrevIterDt = mIterDt;
	cloth.mIterDtAvg.push(static_cast<uint32_t>(mNumIterations), mIterDt);
	cloth.mCurrentMotion = cloth.mTargetMotion;
}

/*
momentum conservation:
m2*x2 - m1*x1 = m1*x1 - m0*x0 + g*dt2, m = r+t
r2*x2+t2 = 2(r1*x1+t1) - (r0*x0+t0) + g*dt2
r2*x2 = r1*x1 + r1*x1 - r0*x0 - (t2-2t1+t0) + g*dt2
substitue       r1*x1 - r0*x0 = r1*(x1-x0) + (r1-r0)*x0
and     r1*x1 = r2*x1 - (r2-r1)*x1

x2 = x1 + r2'*g*dt2
   + r2'r1*(x1-x0) //< damp
   + (r2'r1-r2'r0)*x0 - (1-r2'r1)*x1 - r2'*(t2-2t1+t0) //< inertia
   + (1-r2'r1)x1 + t2-t1 //< drag (not momentum conserving)

x2 = x0 + a0*x0 + a1*x1 + b with
a0 = (inertia-damp)*r2'r1 - inertia*r2'r0 - eye
a1 = (1-inertia-drag)*eye + (damp+inertia+drag)*r2'r1
b = r2'*(g*dt2 - (inertia+drag)*(t2-t1) + inertia*(t1-t0))

Velocities are used to deal with multiple iterations and varying dt. Only b needs
to updated from one iteration to the next. Specifically, it is multiplied
by (r2'r1)^1/numIterations. a0 and a1 are unaffected by that multiplication.

The centrifugal and coriolis forces of non-inertial (turning) reference frame are
not generally captured in these formulas. The 'inertia' term above contains radial
acceleration plus centrifugal and coriolis force for a single iteration.
For multiple iterations, or when the centrifugal forces are scaled differently
than angular inertia, we need to add explicit centrifugal and coriolis forces.
We only use them to correct the above formula because their discretization is
not accurate.

Possible improvements: multiply coriolis and centrifugal matrix by curInvRotation
from the left. Do the alpha trick of linearInertia also for angularInertia, write
prevParticle after multiplying it with matrix.

If you change anything in this function, make sure that ClothCustomFloating and
ClothInertia haven't regressed for any choice of solver frequency.
*/

template <typename T4f, typename MyCloth>
cloth::IterationState<T4f> cloth::IterationStateFactory::create(MyCloth const& cloth) const
{
	IterationState<T4f> result;

	result.mRemainingIterations = static_cast<uint32_t>(mNumIterations);
	result.mInvNumIterations = mInvNumIterations;
	result.mIterDt = mIterDt;

	T4f curLinearVelocity = load(array(cloth.mLinearVelocity));
	T4f prevLinearVelocity = load(array(mPrevLinearVelocity));

	T4f iterDt = simd4f(mIterDt);
	T4f dampExponent = simd4f(cloth.mStiffnessFrequency) * iterDt;

	T4f translation = iterDt * curLinearVelocity;

	// gravity delta per iteration
	T4f gravity = load(array(cloth.mGravity)) * static_cast<T4f>(simd4f(sqr(mIterDtAverage)));

	// scale of local particle velocity per iteration
	T4f dampScale = exp2(load(array(cloth.mLogDamping)) * dampExponent);
	// adjust for the change in time step during the first iteration
	T4f firstDampScale = dampScale * simd4f(mIterDtRatio);

	// portion of negative frame velocity to transfer to particle
	T4f linearDrag = (gSimd4fOne - exp2(load(array(cloth.mLinearLogDrag)) * dampExponent)) * translation;

	// portion of frame acceleration to transfer to particle
	T4f linearInertia = load(array(cloth.mLinearInertia)) * iterDt * (prevLinearVelocity - curLinearVelocity);

	// for inertia, we want to violate newton physics to
	// match velocity and position as given by the user, which means:
	// vt = v0 + a * t and xt = x0 + v0 * t + (!) a * t^2
	// this is achieved by applying a different portion to cur and prev
	// position, compared to the normal +0.5 and -0.5 for '... 1/2 a*t^2'.
	// specifically, the portion is alpha=(n+1)/2n and 1-alpha.

	float linearAlpha = (mNumIterations + 1) * 0.5f * mInvNumIterations;
	T4f curLinearInertia = linearInertia * simd4f(linearAlpha);

	// rotate to local space (use mRotationMatrix temporarily to hold matrix)
	physx::PxMat44 invRotation = physx::PxMat44(mCurrentRotation.getConjugate());
	assign(result.mRotationMatrix, invRotation);

	T4f maskXYZ = simd4f(simd4i(~0, ~0, ~0, 0));

	// Previously, we split the bias between previous and current position to
	// get correct disretized position and velocity. However, this made a
	// hanging cloth experience a downward velocity, which is problematic
	// when scaled by the iterDt ratio and results in jitter under variable
	// timesteps. Instead, we now apply the entire bias to current position
	// and accept a less noticeable error for a free falling cloth.

	T4f bias = gravity - linearDrag;
	result.mCurBias = transform(result.mRotationMatrix, curLinearInertia + bias) & maskXYZ;
	result.mPrevBias = transform(result.mRotationMatrix, linearInertia - curLinearInertia) & maskXYZ;

	T4f wind = load(array(cloth.mWind)) * iterDt; // multiply with delta time here already so we don't have to do it inside the solver
	result.mWind = transform(result.mRotationMatrix, translation - wind) & maskXYZ;

	result.mIsTurning = mPrevAngularVelocity.magnitudeSquared() + cloth.mAngularVelocity.magnitudeSquared() > 0.0f;

	if (result.mIsTurning)
	{
		T4f curAngularVelocity = load(array(invRotation.rotate(cloth.mAngularVelocity)));
		T4f prevAngularVelocity = load(array(invRotation.rotate(mPrevAngularVelocity)));

		// rotation for one iteration in local space
		T4f curInvAngle = -iterDt * curAngularVelocity;
		T4f prevInvAngle = -iterDt * prevAngularVelocity;

		physx::PxQuat curInvRotation = exp(castToPxVec3(curInvAngle));
		physx::PxQuat prevInvRotation = exp(castToPxVec3(prevInvAngle));

		physx::PxMat44 curMatrix = physx::PxMat44(curInvRotation);
		physx::PxMat44 prevMatrix = physx::PxMat44(prevInvRotation * curInvRotation);

		assign(result.mRotationMatrix, curMatrix);

		T4f angularDrag = gSimd4fOne - exp2(load(array(cloth.mAngularLogDrag)) * dampExponent);
		T4f centrifugalInertia = load(array(cloth.mCentrifugalInertia));
		T4f angularInertia = load(array(cloth.mAngularInertia));
		T4f angularAcceleration = curAngularVelocity - prevAngularVelocity;

		T4f epsilon = simd4f(sqrtf(FLT_MIN)); // requirement: sqr(epsilon) > 0
		T4f velocityLengthSqr = lengthSqr(curAngularVelocity) + epsilon;
		T4f dragLengthSqr = lengthSqr(T4f(curAngularVelocity * angularDrag)) + epsilon;
		T4f centrifugalLengthSqr = lengthSqr(T4f(curAngularVelocity * centrifugalInertia)) + epsilon;
		T4f accelerationLengthSqr = lengthSqr(angularAcceleration) + epsilon;
		T4f inertiaLengthSqr = lengthSqr(T4f(angularAcceleration * angularInertia)) + epsilon;

		float dragScale = array(rsqrt(velocityLengthSqr * dragLengthSqr) * dragLengthSqr)[0];
		float inertiaScale =
		    mInvNumIterations * array(rsqrt(accelerationLengthSqr * inertiaLengthSqr) * inertiaLengthSqr)[0];

		// magic factor found by comparing to global space simulation:
		// some centrifugal force is in inertia part, remainder is 2*(n-1)/n
		// after scaling the inertia part, we get for centrifugal:
		float centrifugalAlpha = (2 * mNumIterations - 1) * mInvNumIterations;
		float centrifugalScale =
		    centrifugalAlpha * array(rsqrt(velocityLengthSqr * centrifugalLengthSqr) * centrifugalLengthSqr)[0] -
		    inertiaScale;

		// slightly better in ClothCustomFloating than curInvAngle alone
		T4f centrifugalVelocity = (prevInvAngle + curInvAngle) * simd4f(0.5f);
		const T4f data = lengthSqr(centrifugalVelocity);
		float centrifugalSqrLength = array(data)[0] * centrifugalScale;

		T4f coriolisVelocity = centrifugalVelocity * simd4f(centrifugalScale);
		physx::PxMat33 coriolisMatrix = physx::shdfnd::star(castToPxVec3(coriolisVelocity));

		const float* dampScalePtr = array(firstDampScale);
		const float* centrifugalPtr = array(centrifugalVelocity);

		for (unsigned int j = 0; j < 3; ++j)
		{
			float centrifugalJ = -centrifugalPtr[j] * centrifugalScale;
			for (unsigned int i = 0; i < 3; ++i)
			{
				float damping = dampScalePtr[j];
				float coriolis = coriolisMatrix(i, j);
				float centrifugal = centrifugalPtr[i] * centrifugalJ;

				prevMatrix(i, j) = centrifugal - coriolis + curMatrix(i, j) * (inertiaScale - damping) -
				                   prevMatrix(i, j) * inertiaScale;
				curMatrix(i, j) = centrifugal + coriolis + curMatrix(i, j) * (inertiaScale + damping + dragScale);
			}
			curMatrix(j, j) += centrifugalSqrLength - inertiaScale - dragScale;
			prevMatrix(j, j) += centrifugalSqrLength;
		}

		assign(result.mPrevMatrix, prevMatrix);
		assign(result.mCurMatrix, curMatrix);
	}
	else
	{
		T4f minusOne = -static_cast<T4f>(gSimd4fOne);
		result.mRotationMatrix[0] = minusOne;
		result.mPrevMatrix[0] = select(maskXYZ, firstDampScale, minusOne);
	}

	// difference of damp scale between first and other iterations
	result.mDampScaleUpdate = (dampScale - firstDampScale) & maskXYZ;

	return result;
}

template <typename T4f>
void cloth::IterationState<T4f>::update()
{
	if (mIsTurning)
	{
		// only need to turn bias, matrix is unaffected (todo: verify)
		mCurBias = transform(mRotationMatrix, mCurBias);
		mPrevBias = transform(mRotationMatrix, mPrevBias);
		mWind = transform(mRotationMatrix, mWind);
	}

	// remove time step ratio in damp scale after first iteration
	for (uint32_t i = 0; i < 3; ++i)
	{
		mPrevMatrix[i] = mPrevMatrix[i] - mRotationMatrix[i] * mDampScaleUpdate;
		mCurMatrix[i] = mCurMatrix[i] + mRotationMatrix[i] * mDampScaleUpdate;
	}
	mDampScaleUpdate = gSimd4fZero; // only once

	--mRemainingIterations;
}

} // namespace nv
