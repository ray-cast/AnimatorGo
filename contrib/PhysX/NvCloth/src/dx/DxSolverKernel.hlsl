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


#include "DxClothData.h"

#ifndef FLT_MAX
#define FLT_MAX 3.402823466e+38F
#endif


struct IndexPair
{
	uint32_t first;
	uint32_t second;
};

RWByteAddressBuffer bParticles : register(u0); //contains float4 data
RWStructuredBuffer<float4> bSelfCollisionParticles : register(u1);
RWStructuredBuffer<uint32_t> bSelfCollisionData : register(u2);

RWStructuredBuffer<DxFrameData> bFrameData : register(u3);

StructuredBuffer<DxClothData> bClothData : register(t0);
StructuredBuffer<DxIterationData> bIterData : register(t2);

StructuredBuffer<DxPhaseConfig> bPhaseConfigs : register(t3);
StructuredBuffer<DxConstraint> bConstraints : register(t4);
StructuredBuffer<DxTether> bTetherConstraints : register(t5);

StructuredBuffer<IndexPair> bCapsuleIndices : register(t6);
StructuredBuffer<float4> bCollisionSpheres : register(t7);

StructuredBuffer<uint32_t> bConvexMasks : register(t8);
StructuredBuffer<float4> bCollisionPlanes : register(t9);

StructuredBuffer<float3> bCollisionTriangles : register(t10);

StructuredBuffer<float4> bMotionConstraints : register(t11);
StructuredBuffer<float4> bSeparationConstraints : register(t12);

StructuredBuffer<float4> bParticleAccelerations : register(t13);

StructuredBuffer<float4> bRestPositions : register(t14);

StructuredBuffer<int32_t> bSelfCollisionIndices : register(t15);

StructuredBuffer<float> bPerConstraintStiffness : register(t16);

//cloth mesh triangle information for air drag/lift
//Note that the buffer actually contains uint16_t values
StructuredBuffer<uint32_t> bTriangles : register(t17);

StructuredBuffer<int32_t> bVirtualParticleSetSizes : register(t18);
StructuredBuffer<uint4>   bVirtualParticleIndices : register(t19);
StructuredBuffer<float4>  bVirtualParticleWeights : register(t20);

groupshared DxClothData gClothData;
groupshared DxFrameData gFrameData;
groupshared DxIterationData gIterData;
groupshared uint gCurParticles[MaxParticlesInSharedMem * 4];

groupshared float gBounds[192];


static const uint32_t blockDim = 1024;
static const uint32_t BlockSize = blockDim;
static const uint32_t WarpsPerBlock = (BlockSize >> 5);

#include "DxSortKernel.inc"

#define FLT_EPSILON 1.192092896e-07F

interface IParticles
{
	float4 get(uint32_t index);
	void set(uint32_t index, float4 value);
	void atomicAdd(uint32_t index, float3 value);
};


void integrateParticles(IParticles curParticles, IParticles prevParticles, uint32_t threadIdx)
{
	for (uint32_t i = threadIdx; i < gClothData.mNumParticles; i += blockDim)
	{
		float4 curPos = curParticles.get(i);

		float nextX = curPos.x, curX = nextX;
		float nextY = curPos.y, curY = nextY;
		float nextZ = curPos.z, curZ = nextZ;
		float nextW = curPos.w;

		float4 prevPos = prevParticles.get(i);

		if (nextW == 0.0f)
			nextW = prevPos.w;

		if (nextW > 0.0f)
		{
			float prevX = prevPos.x;
			float prevY = prevPos.y;
			float prevZ = prevPos.z;

			if (gIterData.mIsTurning)
			{
				nextX = nextX + gIterData.mIntegrationTrafo[3] + curX * gIterData.mIntegrationTrafo[15] +
				        prevX * gIterData.mIntegrationTrafo[6] + curY * gIterData.mIntegrationTrafo[16] +
				        prevY * gIterData.mIntegrationTrafo[7] + curZ * gIterData.mIntegrationTrafo[17] +
				        prevZ * gIterData.mIntegrationTrafo[8];

				nextY = nextY + gIterData.mIntegrationTrafo[4] + curX * gIterData.mIntegrationTrafo[18] +
				        prevX * gIterData.mIntegrationTrafo[9] + curY * gIterData.mIntegrationTrafo[19] +
				        prevY * gIterData.mIntegrationTrafo[10] + curZ * gIterData.mIntegrationTrafo[20] +
						prevZ * gIterData.mIntegrationTrafo[11];

				nextZ = nextZ + gIterData.mIntegrationTrafo[5] + curX * gIterData.mIntegrationTrafo[21] +
				        prevX * gIterData.mIntegrationTrafo[12] + curY * gIterData.mIntegrationTrafo[22] +
				        prevY * gIterData.mIntegrationTrafo[13] + curZ * gIterData.mIntegrationTrafo[23] +
						prevZ * gIterData.mIntegrationTrafo[14];
			}
			else
			{
				nextX += (curX - prevX) * gIterData.mIntegrationTrafo[6] + gIterData.mIntegrationTrafo[3];
				nextY += (curY - prevY) * gIterData.mIntegrationTrafo[9] + gIterData.mIntegrationTrafo[4];
				nextZ += (curZ - prevZ) * gIterData.mIntegrationTrafo[12] + gIterData.mIntegrationTrafo[5];
			}

			curX += gIterData.mIntegrationTrafo[0];
			curY += gIterData.mIntegrationTrafo[1];
			curZ += gIterData.mIntegrationTrafo[2];
		}

		curPos.x = nextX;
		curPos.y = nextY;
		curPos.z = nextZ;
		curPos.w = nextW;
		curParticles.set(i, curPos);

		prevPos.x = curX;
		prevPos.y = curY;
		prevPos.z = curZ;
		prevParticles.set(i, prevPos);
	}
}

void accelerateParticles(IParticles curParticles, uint32_t threadIdx)
{
	// might be better to move this into integrate particles
	uint32_t accelerationsOffset = gFrameData.mParticleAccelerationsOffset;

	GroupMemoryBarrierWithGroupSync(); // looping with 4 instead of 1 thread per particle

	float sqrIterDt = ~threadIdx & 0x3 ? gFrameData.mIterDt * gFrameData.mIterDt : 0.0f;
	for (uint32_t i = threadIdx; i < gClothData.mNumParticles * 4; i += blockDim)
	{
		float4 acceleration = bParticleAccelerations[accelerationsOffset + i];

		float4 curPos = curParticles.get(i / 4);
		if (curPos.w > 0.0f)
		{
			curPos += acceleration * sqrIterDt;
			curParticles.set(i / 4, curPos);
		}
	}

	GroupMemoryBarrierWithGroupSync();
}

float rsqrt_2(const float v)
{
	float halfV = v * 0.5f;
	float threeHalf = 1.5f;
	float r = rsqrt(v);
	for(int i = 0; i < 10; ++i)
		r = r * (threeHalf - halfV * r * r);
	return r;
}

void applyWind(IParticles curParticles, IParticles prevParticles, uint32_t threadIdx)
{
	const float dragCoefficient = gFrameData.mDragCoefficient;
	const float liftCoefficient = gFrameData.mLiftCoefficient;
	const float fluidDensity = gFrameData.mFluidDensity;
	const float itrDt = gFrameData.mIterDt;
	if(dragCoefficient == 0.0f && liftCoefficient == 0.0f)
		return;

	const float oneThird = 1.0f / 3.0f;
	float3 wind = float3(gIterData.mWind[0], gIterData.mWind[1], gIterData.mWind[2]);

	GroupMemoryBarrierWithGroupSync();

	uint32_t triangleOffset = gClothData.mStartTriangleOffset;

	for(uint32_t i = threadIdx; i < gClothData.mNumTriangles; i += blockDim.x)
	{
		uint32_t i0 = bTriangles[triangleOffset + ((i * 3 + 0) >> 1)];
		uint32_t i1 = bTriangles[triangleOffset + ((i * 3 + 1) >> 1)];
		uint32_t i2 = bTriangles[triangleOffset + ((i * 3 + 2) >> 1)];

		if((i * 3) & 1)
		{
			i0 = (i0 & 0xFFFF0000) >> 16;
			i1 = (i1 & 0x0000FFFF);
			i2 = (i2 & 0xFFFF0000) >> 16;
		}
		else
		{
			i0 = (i0 & 0x0000FFFF);
			i1 = (i1 & 0xFFFF0000) >> 16;
			i2 = (i2 & 0x0000FFFF);
		}

		float4 c0 = curParticles.get(i0);
		float4 c1 = curParticles.get(i1);
		float4 c2 = curParticles.get(i2);

		float4 p0 = prevParticles.get(i0);
		float4 p1 = prevParticles.get(i1);
		float4 p2 = prevParticles.get(i2);

		float3 cur = oneThird * (c0.xyz + c1.xyz + c2.xyz);
		float3 prev = oneThird * (p0.xyz + p1.xyz + p2.xyz);

		float3 delta = cur - prev + wind;

		if(gIterData.mIsTurning)
		{
			const float3 rot[3] = {
				float3(gFrameData.mRotation[0], gFrameData.mRotation[1], gFrameData.mRotation[2]),
				float3(gFrameData.mRotation[3], gFrameData.mRotation[4], gFrameData.mRotation[5]),
				float3(gFrameData.mRotation[6], gFrameData.mRotation[7], gFrameData.mRotation[8])
			};
			float3 d = wind - prev;
			delta = cur + d.x * rot[0] + d.y * rot[1] + d.z * rot[2];
		}

		float3 normal = cross(c2.xyz - c0.xyz, c1.xyz - c0.xyz);

		const float doubleArea = sqrt(dot(normal, normal));
		normal = normal / doubleArea;

		float invSqrScale = dot(delta, delta);
		float scale = rsqrt(invSqrScale);
		float deltaLength = sqrt(invSqrScale);

		float cosTheta = dot(normal, delta) * scale;
		float sinTheta = sqrt(max(0.0f, 1.0f - cosTheta * cosTheta));

		float3 liftDir = cross(cross(delta, normal), scale * delta);

		float3 lift = liftCoefficient * cosTheta * sinTheta * liftDir * deltaLength / itrDt;
		float3 drag = dragCoefficient * abs(cosTheta) * delta * deltaLength / itrDt;

		float3 impulse = invSqrScale < 1.192092896e-07F ? float3(0.0f, 0.0f, 0.0f) : (lift + drag) * fluidDensity * doubleArea;

		curParticles.atomicAdd(i0, -impulse * c0.w);
		curParticles.atomicAdd(i1, -impulse * c1.w);
		curParticles.atomicAdd(i2, -impulse * c2.w);
	}

	GroupMemoryBarrierWithGroupSync();
}

void constrainMotion(IParticles curParticles, uint32_t threadIdx, float alpha)
{
	if (gFrameData.mStartMotionConstrainsOffset == -1)
		return;

	// negative because of fused multiply-add optimization
	float negativeScale = -gClothData.mMotionConstraintScale;
	float negativeBias = -gClothData.mMotionConstraintBias;

	uint32_t startMotionConstrainsOffset = gFrameData.mStartMotionConstrainsOffset;
	uint32_t targetMotionConstrainsOffset = gFrameData.mTargetMotionConstrainsOffset;

	for (uint32_t i = threadIdx; i < gClothData.mNumParticles; i += blockDim.x)
	{
		float4 startPos = bMotionConstraints[startMotionConstrainsOffset + i];
		float4 targetPos = bMotionConstraints[targetMotionConstrainsOffset + i];

		float4 sphere = startPos + (targetPos - startPos) * alpha;

		float4 curPos = curParticles.get(i);

		float3 delta = sphere.xyz - curPos.xyz;

		float sqrLength = FLT_EPSILON + dot(delta, delta);
		float negativeRadius = min(0.0f, sphere.w * negativeScale + negativeBias);

		float slack = max(negativeRadius * rsqrt(sqrLength) + 1.0f, 0.0f) * gFrameData.mMotionConstraintStiffness;

		curPos.xyz += slack * delta;

		// set invMass to zero if radius is zero
		if (negativeRadius >= 0.0f)
			curPos.w = 0.0f;

		curParticles.set(i, curPos);
	}

}

void constrainTether(IParticles curParticles, uint32_t threadIdx)
{
	if (0.0f == gFrameData.mTetherConstraintStiffness || !gClothData.mNumTethers)
		return;

	uint32_t numParticles = gClothData.mNumParticles;
	uint32_t numTethers = gClothData.mNumTethers;

	float stiffness = numParticles * gFrameData.mTetherConstraintStiffness / numTethers;
	float scale = gClothData.mTetherConstraintScale;

	for (uint32_t i = threadIdx; i < gClothData.mNumParticles; i += blockDim)
	{
		float4 curPos = curParticles.get(i);
		float posX = curPos.x;
		float posY = curPos.y;
		float posZ = curPos.z;

		float offsetX = 0.0f;
		float offsetY = 0.0f;
		float offsetZ = 0.0f;

		for (uint32_t j = i; j < numTethers; j += gClothData.mNumParticles)
		{
			uint32_t tether = bTetherConstraints[gClothData.mTetherOffset + j].mValue;

			uint32_t anchor = tether & 0xffff;
			float4 anchorPos = curParticles.get(anchor);
			float deltaX = anchorPos.x - posX;
			float deltaY = anchorPos.y - posY;
			float deltaZ = anchorPos.z - posZ;

			float sqrLength = FLT_EPSILON + deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;

			float radius = (tether >> 16) * scale;
			float slack = 1.0f - radius * rsqrt(sqrLength);

			if (slack > 0.0f)
			{
				offsetX += deltaX * slack;
				offsetY += deltaY * slack;
				offsetZ += deltaZ * slack;
			}
		}

		curPos.x = posX + offsetX * stiffness;
		curPos.y = posY + offsetY * stiffness;
		curPos.z = posZ + offsetZ * stiffness;

		curParticles.set(i, curPos);
	}
}

void solveFabric(IParticles curParticles, uint32_t threadIdx)
{
	for (uint32_t i = 0; i < gClothData.mNumPhases; ++i)
	{
		DxPhaseConfig phaseConfig = bPhaseConfigs[i + gClothData.mPhaseConfigOffset];

		float exponent = gFrameData.mStiffnessExponent;
		phaseConfig.mStiffness = 1.0f - exp2(phaseConfig.mStiffness * exponent);
		phaseConfig.mStiffnessMultiplier = 1.0f - exp2(phaseConfig.mStiffnessMultiplier * exponent);

		uint32_t firstConstraint = gClothData.mConstraintOffset + phaseConfig.mFirstConstraint;
		bool useStiffnessPerConstraint = gClothData.mStiffnessOffset!=-1;
		uint32_t firstStiffnessValue = gClothData.mStiffnessOffset + phaseConfig.mFirstConstraint;

		GroupMemoryBarrierWithGroupSync();

		for (uint32_t j = threadIdx; j < phaseConfig.mNumConstraints; j += blockDim)
		{
			DxConstraint constraint = bConstraints[firstConstraint + j];

			uint32_t vpi = (constraint.mIndices) & 0xffff;
			uint32_t vpj = (constraint.mIndices >> 16) & 0xffff;
			float rij = constraint.mRestvalue;

			float4 vpiPos = curParticles.get(vpi);
			float vxi = vpiPos.x;
			float vyi = vpiPos.y;
			float vzi = vpiPos.z;
			float vwi = vpiPos.w;

			float4 vpjPos = curParticles.get(vpj);
			float vxj = vpjPos.x;
			float vyj = vpjPos.y;
			float vzj = vpjPos.z;
			float vwj = vpjPos.w;

			float hxij = vxj - vxi;
			float hyij = vyj - vyi;
			float hzij = vzj - vzi;

			float e2ij = FLT_EPSILON + hxij * hxij + hyij * hyij + hzij * hzij;
			float negErij = rij > FLT_EPSILON ? -1.0f + rij * rsqrt(e2ij) : 0.0f;

			negErij = negErij + phaseConfig.mStiffnessMultiplier *
			                        max(phaseConfig.mCompressionLimit, min(-negErij, phaseConfig.mStretchLimit));

			float stiffness = useStiffnessPerConstraint?
								1.0f - exp2(bPerConstraintStiffness[firstStiffnessValue + j] * exponent)
								:
								phaseConfig.mStiffness;
			float negExij = negErij * stiffness / (FLT_EPSILON + vwi + vwj);

			float vmi = -vwi * negExij;
			vpiPos.x = vxi + vmi * hxij;
			vpiPos.y = vyi + vmi * hyij;
			vpiPos.z = vzi + vmi * hzij;
			curParticles.set(vpi, vpiPos);

			float vmj = +vwj * negExij;
			vpjPos.x = vxj + vmj * hxij;
			vpjPos.y = vyj + vmj * hyij;
			vpjPos.z = vzj + vmj * hzij;
			curParticles.set(vpj, vpjPos);
		}
	}
}

float3 calcFrictionImpulse(float3 prevPos, float3 curPos, float3 shapeVelocity,	float scale, float3 collisionImpulse)
{
	const float frictionScale = gClothData.mFrictionScale;

	// calculate collision normal
	float deltaSq = dot(collisionImpulse, collisionImpulse);

	float rcpDelta = rsqrt(deltaSq + FLT_EPSILON);

	float3 norm = collisionImpulse * rcpDelta;

	// calculate relative velocity scaled by number of collision
	float3 relVel = curPos - prevPos - shapeVelocity * scale;

	// calculate relative tangential velocity
	float3 relVelTang = relVel - dot(relVel, norm) * norm;

	// calculate magnitude of vt
	float rcpVt = rsqrt(dot(relVelTang, relVelTang) + FLT_EPSILON);

	// magnitude of friction impulse (cannot be larger than -|vt|)
	float j = max(-frictionScale * deltaSq * rcpDelta * scale * rcpVt, -1.0f);
	return relVelTang * j;
}

float calcPlaneDist(float3 position, float alpha, uint32_t planeIndex, out float3 norm)
{
	float4 startPlane = bCollisionPlanes[gFrameData.mStartCollisionPlaneOffset + planeIndex];
	float4 targetPlane = bCollisionPlanes[gFrameData.mTargetCollisionPlaneOffset + planeIndex];

	float4 plane = lerp(startPlane, targetPlane, alpha);

	norm = plane.xyz;
	return dot(position, norm) + plane.w;
}

uint32_t collideConvexes(float3 position, float alpha, out float3 delta)
{
	delta.xyz = float3(0.0f, 0.0f, 0.0f);

	uint32_t numCollisions = 0;
	for (uint32_t i = 0; i < gClothData.mNumConvexes; ++i)
	{
		uint32_t mask = bConvexMasks[gClothData.mConvexMasksOffset + i];

		float3 maxNorm;
		float maxDist = calcPlaneDist(position, alpha, firstbitlow(mask), maxNorm);

		while ((maxDist < 0.0f) && (mask &= mask - 1))
		{
			float3 norm;
			float dist = calcPlaneDist(position, alpha, firstbitlow(mask), norm);
			if (dist > maxDist)
				maxDist = dist, maxNorm = norm;
		}

		if (maxDist < 0.0f)
		{
			delta.xyz -= maxNorm * maxDist;

			++numCollisions;
		}
	}
	return numCollisions;
}

void collideConvexes(IParticles curParticles, IParticles prevParticles, uint32_t threadIdx, float alpha)
{
	if (!gClothData.mNumConvexes)
		return;

	bool frictionEnabled = gClothData.mFrictionScale > 0.0f;

	for (uint32_t j = threadIdx; j < gClothData.mNumParticles; j += blockDim)
	{
		float4 curPos = curParticles.get(j);

		float3 delta;
		uint32_t numCollisions = collideConvexes(curPos.xyz, alpha, delta);
		if (numCollisions > 0)
		{
			float scale = 1.0f / numCollisions;

			if (frictionEnabled)
			{
				float4 prevPos = prevParticles.get(j);

				float3 frictionImpulse =
					calcFrictionImpulse(prevPos.xyz, curPos.xyz, float3(0.0f, 0.0f, 0.0f), scale, delta);

				prevPos.xyz -= frictionImpulse;
				prevParticles.set(j, prevPos);
			}

			curPos.xyz += delta.xyz * scale;
			curParticles.set(j, curPos);
		}
	}
}


struct TriangleData
{
	float3 base;
	float edge0DotEdge1;

	float3 edge0;
	float edge0SqrLength;

	float3 edge1;
	float edge1SqrLength;

	float3 normal;		

	float det;
	float denom;

	float edge0InvSqrLength;
	float edge1InvSqrLength;

	// initialize struct after vertices have been stored in first 9 members
	void initialize()
	{
		edge0 -= base;
		edge1 -= base;

		normal = cross(edge0, edge1);

		float normalInvLength = rsqrt(dot(normal, normal));
		normal *= normalInvLength;

		edge0DotEdge1 = dot(edge0, edge1);
		edge0SqrLength = dot(edge0, edge0);
		edge1SqrLength = dot(edge1, edge1);

		det = 1.0f / (edge0SqrLength * edge1SqrLength - edge0DotEdge1 * edge0DotEdge1);
		denom = 1.0f / (edge0SqrLength + edge1SqrLength - edge0DotEdge1 - edge0DotEdge1);

		edge0InvSqrLength = 1.0f / edge0SqrLength;
		edge1InvSqrLength = 1.0f / edge1SqrLength;
	}
};


void collideParticleTriangles(IParticles curParticles, int32_t i, float alpha)
{
	float4 curPos = curParticles.get(i);
	float3 pos = curPos.xyz;

	float4 normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float minSqrLength = FLT_MAX;

	for (uint32_t j = 0; j < gClothData.mNumCollisionTriangles; ++j)
	{
		// start + (target - start) * alpha
		float3 startBase = bCollisionTriangles[gFrameData.mStartCollisionTrianglesOffset + 3 * j];
		float3 targetBase = bCollisionTriangles[gFrameData.mTargetCollisionTrianglesOffset + 3 * j];
		float3 startEdge0 = bCollisionTriangles[gFrameData.mStartCollisionTrianglesOffset + 3 * j + 1];
		float3 targetEdge0 = bCollisionTriangles[gFrameData.mTargetCollisionTrianglesOffset + 3 * j + 1];
		float3 startEdge1 = bCollisionTriangles[gFrameData.mStartCollisionTrianglesOffset + 3 * j + 2];
		float3 targetEdge1 = bCollisionTriangles[gFrameData.mTargetCollisionTrianglesOffset + 3 * j + 2];

		TriangleData tIt;
		tIt.base = startBase + (targetBase - startBase) * alpha;
		tIt.edge0 = startEdge0 + (targetEdge0 - startEdge0) * alpha;
		tIt.edge1 = startEdge1 + (targetEdge1 - startEdge1) * alpha;

		tIt.initialize();

		float3 delta = pos - tIt.base;

		float deltaDotEdge0 = dot(delta, tIt.edge0);
		float deltaDotEdge1 = dot(delta, tIt.edge1);
		float deltaDotNormal = dot(delta, tIt.normal);

		float s = tIt.edge1SqrLength * deltaDotEdge0 - tIt.edge0DotEdge1 * deltaDotEdge1;
		float t = tIt.edge0SqrLength * deltaDotEdge1 - tIt.edge0DotEdge1 * deltaDotEdge0;

		s = t > 0.0f ? s * tIt.det : deltaDotEdge0 * tIt.edge0InvSqrLength;
		t = s > 0.0f ? t * tIt.det : deltaDotEdge1 * tIt.edge1InvSqrLength;

		if (s + t > 1.0f)
		{
			s = (tIt.edge1SqrLength - tIt.edge0DotEdge1 + deltaDotEdge0 - deltaDotEdge1) * tIt.denom;
		}

		// Probably we should check NaN?
		s = max(0.0f, min(1.0f, s));
		t = max(0.0f, min(1.0f - s, t));

		delta -= (tIt.edge0 * s + tIt.edge1 * t);

		float sqrLength = dot(delta, delta);

		if (0.0f > deltaDotNormal)
			sqrLength *= 1.0001f;

		if (sqrLength < minSqrLength)
		{
			normal.xyz = tIt.normal;
			normal.w = deltaDotNormal;

			minSqrLength = sqrLength;
		}
	}

	if (normal.w < 0.0f)
	{
		curPos.xyz = pos - normal.xyz * normal.w;
		curParticles.set(i, curPos);
	}
}


void collideTriangles(IParticles curParticles, uint32_t threadIdx, float alpha)
{
	if (!gClothData.mNumCollisionTriangles)
		return;

	bool frictionEnabled = gClothData.mFrictionScale > 0.0f;

	// interpolate triangle vertices and store in shared memory
//	for (int32_t i = threadIdx.x, n = gClothData.mNumCollisionTriangles * 3; i < n; i += blockDim.x)
//	{
//		float3 start = bCollisionTriangles[gFrameData.mStartCollisionTrianglesOffset + i];
//		float3 target = bCollisionTriangles[gFrameData.mTargetCollisionTrianglesOffset + i];
//
//		mCurData.mSphereX[offset] = start + (target - start) * alpha;
//	}
//
	GroupMemoryBarrierWithGroupSync();

	for (uint32_t j = threadIdx; j < gClothData.mNumParticles; j += blockDim)
	{
	//	float4 curPos = curParticles.get(j);

	//	float3 delta;
		collideParticleTriangles(curParticles, j, alpha);
	//	if (numCollisions > 0)
	//	{
	//		float scale = 1.0f / numCollisions;
	//
	//		curPos.xyz += delta.xyz * scale;
	//		curParticles.set(j, curPos);
	//	}
	}

	GroupMemoryBarrierWithGroupSync();
}

uint32_t collideCapsules(float3 curPos, float alpha, float prevAlpha, out float3 outDelta, out float3 outVelocity)
{
	outDelta = float3(0.0f, 0.0f, 0.0f);
	outVelocity = float3(0.0f, 0.0f, 0.0f);
	uint32_t numCollisions = 0;

	uint32_t capsuleOffset = gClothData.mCapsuleOffset;
	uint32_t startSphereOffset = gFrameData.mStartSphereOffset;
	uint32_t targetSphereOffset = gFrameData.mTargetSphereOffset;

	bool frictionEnabled = gClothData.mFrictionScale > 0.0f;

	// cone collision
	for (uint32_t i = 0; i < gClothData.mNumCapsules; ++i)
	{
		IndexPair indices = bCapsuleIndices[capsuleOffset + i];
		
		float4 startSphere0 = bCollisionSpheres[startSphereOffset + indices.first];
		float4 targetSphere0 = bCollisionSpheres[targetSphereOffset + indices.first];
		float4 sphere0 = lerp(startSphere0, targetSphere0, alpha);

		float4 startSphere1 = bCollisionSpheres[startSphereOffset + indices.second];
		float4 targetSphere1 = bCollisionSpheres[targetSphereOffset + indices.second];
		float4 sphere1 = lerp(startSphere1, targetSphere1, alpha);

		sphere0.w = max(sphere0.w, 0.0f);
		sphere1.w = max(sphere1.w, 0.0f);

		float4 axis = (sphere1 - sphere0) * 0.5f;

		float sqrAxisLength = dot(axis.xyz, axis.xyz);
		float sqrConeLength = sqrAxisLength - axis.w * axis.w;

		if (sqrConeLength <= 0.0f)
			continue;

		float invAxisLength = rsqrt(sqrAxisLength);
		float invConeLength = rsqrt(sqrConeLength);

		float axisLength = sqrAxisLength * invAxisLength;

		float3 coneCenter = (sphere1.xyz + sphere0.xyz) * 0.5f;
		float coneRadius = (axis.w + sphere0.w) * invConeLength * axisLength;

		float3 coneAxis = axis.xyz * invAxisLength;
		float coneSlope = axis.w * invConeLength;

		float sine = axis.w * invAxisLength;
		float coneSqrCosine = 1.f - sine * sine;
		float coneHalfLength = axisLength;

		{
			float3 delta = curPos - coneCenter;

			float deltaDotAxis = dot(delta, coneAxis);
			float radius = max(deltaDotAxis * coneSlope + coneRadius, 0.0f);
			float sqrDistance = dot(delta, delta) - deltaDotAxis * deltaDotAxis;

			if (sqrDistance > radius * radius)
				continue;

			sqrDistance = max(sqrDistance, FLT_EPSILON);
			float invDistance = rsqrt(sqrDistance);

			float base = deltaDotAxis + coneSlope * sqrDistance * invDistance;
			float halfLength = coneHalfLength;

			if (abs(base) < halfLength)
			{
				delta = delta - base * coneAxis;

				float sqrCosine = coneSqrCosine;
				float scale = radius * invDistance * sqrCosine - sqrCosine;

				outDelta += delta * scale;

				if (frictionEnabled)
				{
					// get previous sphere pos
					float4 prevSphere0 = lerp(startSphere0, targetSphere0, prevAlpha);
					float4 prevSphere1 = lerp(startSphere1, targetSphere1, prevAlpha);

					// interpolate velocity between the two spheres
					float t = deltaDotAxis * 0.5f + 0.5f;
					outVelocity += lerp(sphere0.xyz - prevSphere0.xyz, sphere1.xyz - prevSphere1.xyz, t);
				}

				++numCollisions;
			}
		}
	}

	// sphere collision
	for (uint32_t j = 0; j < gClothData.mNumSpheres; ++j)
	{
		float4 startSphere = bCollisionSpheres[startSphereOffset + j];
		float4 targetSphere = bCollisionSpheres[targetSphereOffset + j];
		float4 sphere = lerp(startSphere, targetSphere, alpha);
		sphere.w = max(sphere.w, 0.0f);

		{
			float3 delta = curPos - sphere.xyz;

			float sqrDistance = FLT_EPSILON + dot(delta, delta);
			float relDistance = rsqrt(sqrDistance) * sphere.w;

			if (relDistance > 1.0f)
			{
				float scale = relDistance - 1.0f;

				outDelta += delta * scale;

				if (frictionEnabled)
				{
					// get previous sphere pos
					float4 prevSphere = lerp(startSphere, targetSphere, prevAlpha);

					outVelocity += (sphere.xyz - prevSphere.xyz);
				}

				++numCollisions;
			}
		}
	}

	return numCollisions;
}

void collideCapsules(IParticles curParticles, IParticles prevParticles, uint32_t threadIdx, float alpha, float prevAlpha)
{
	bool frictionEnabled = gClothData.mFrictionScale > 0.0f;
	bool massScaleEnabled = gClothData.mCollisionMassScale > 0.0f;

	for (uint32_t j = threadIdx; j < gClothData.mNumParticles; j += blockDim)
	{
		float4 curPos = curParticles.get(j);

		float3 delta, velocity;
		uint32_t numCollisions = collideCapsules(curPos.xyz, alpha, prevAlpha, delta, velocity);
		if (numCollisions > 0)
		{
			float scale = 1.0f / numCollisions;

			if (frictionEnabled)
			{
				float4 prevPos = prevParticles.get(j);

				float3 frictionImpulse =
					calcFrictionImpulse(prevPos.xyz, curPos.xyz, velocity, scale, delta);

				prevPos.xyz -= frictionImpulse;
				prevParticles.set(j, prevPos);
			}

			curPos.xyz += delta * scale;

			//TODO: current impl. causes glitches - fix it!
			if (massScaleEnabled)
			{
				float deltaLengthSq = dot(delta, delta);
				float massScale = 1.0f + gClothData.mCollisionMassScale * deltaLengthSq;
				curPos.w /= massScale;
			}

			curParticles.set(j, curPos);
		}
	}
}

static const float gSkeletonWidth = (1.f - 0.2f) * (1.f - 0.2f) - 1.f;

uint32_t collideCapsules(float3 curPosAbsolute, float3 prevPosAbsolute, float alpha, float prevAlpha, out float3 outDelta, out float3 outVelocity)
{
	outDelta = float3(0.0f, 0.0f, 0.0f);
	outVelocity = float3(0.0f, 0.0f, 0.0f);
	uint32_t numCollisions = 0;

	uint32_t capsuleOffset = gClothData.mCapsuleOffset;
	uint32_t startSphereOffset = gFrameData.mStartSphereOffset;
	uint32_t targetSphereOffset = gFrameData.mTargetSphereOffset;

	bool frictionEnabled = gClothData.mFrictionScale > 0.0f;

	// cone collision
	for (uint32_t i = 0; i < gClothData.mNumCapsules; ++i)
	{
		IndexPair indices = bCapsuleIndices[capsuleOffset + i];

		// load sphere data
		float4 startSphere0 = bCollisionSpheres[startSphereOffset + indices.first];
		float4 targetSphere0 = bCollisionSpheres[targetSphereOffset + indices.first];

		float4 startSphere1 = bCollisionSpheres[startSphereOffset + indices.second];
		float4 targetSphere1 = bCollisionSpheres[targetSphereOffset + indices.second];

		// interpolate sphere data to end of previous iteration
		float4 prevSphere0 = lerp(startSphere0, targetSphere0, prevAlpha);
		float4 prevSphere1 = lerp(startSphere1, targetSphere1, prevAlpha);

		prevSphere0.w = max(prevSphere0.w, 0.0f);
		prevSphere1.w = max(prevSphere1.w, 0.0f);

		float3 prevConeCenter = (prevSphere1.xyz + prevSphere0.xyz) * 0.5f;
		float4 prevHalfAxis = (prevSphere1 - prevSphere0) * 0.5f; //half axis
		//axis.w = half of radii difference

		float3 prevPos = prevPosAbsolute - prevConeCenter;

		float prevSqrAxisHalfLength = dot(prevHalfAxis.xyz, prevHalfAxis.xyz);
		float prevSqrConeHalfLength = prevSqrAxisHalfLength - prevHalfAxis.w * prevHalfAxis.w;

		if (prevSqrConeHalfLength <= 0.0f)
			continue;

		float prevInvAxisHalfLength = rsqrt(prevSqrAxisHalfLength);
		float prevInvConeHalfLength = rsqrt(prevSqrConeHalfLength);

		float prevAxisHalfLength = prevSqrAxisHalfLength * prevInvAxisHalfLength;

		float prevConeRadius = (prevHalfAxis.w + prevSphere0.w) * prevInvConeHalfLength * prevAxisHalfLength;

		float3 prevConeAxis = prevHalfAxis.xyz * prevInvAxisHalfLength;
		float prevConeSlope = prevHalfAxis.w * prevInvConeHalfLength;

		float3 prevCross = cross(prevPos, prevConeAxis);
		float prevDot = dot(prevPos, prevConeAxis);

		// interpolate sphere data to end of current iteration
		float4 curSphere0 = lerp(startSphere0, targetSphere0, alpha);
		float4 curSphere1 = lerp(startSphere1, targetSphere1, alpha);

		curSphere0.w = max(curSphere0.w, 0.0f);
		curSphere1.w = max(curSphere1.w, 0.0f);

		float3 curConeCenter = (curSphere1.xyz + curSphere0.xyz) * 0.5f;
		float4 curHalfAxis = (curSphere1 - curSphere0) * 0.5f; //half axis
		//axis.w = half of radii difference

		float3 curPos = curPosAbsolute - curConeCenter;

		float curSqrAxisHalfLength = dot(curHalfAxis.xyz, curHalfAxis.xyz);
		float curSqrConeHalfLength = curSqrAxisHalfLength - curHalfAxis.w * curHalfAxis.w;

		if(curSqrConeHalfLength <= 0.0f)
			continue;

		float curInvAxisHalfLength = rsqrt(curSqrAxisHalfLength);
		float curInvConeHalfLength = rsqrt(curSqrConeHalfLength);

		float curAxisHalfLength = curSqrAxisHalfLength * curInvAxisHalfLength;

		float curConeRadius = (curHalfAxis.w + curSphere0.w) * curInvConeHalfLength * curAxisHalfLength;

		float3 curConeAxis = curHalfAxis.xyz * curInvAxisHalfLength;

		float3 curCross = cross(curPos, curConeAxis);
		float curDot = dot(curPos, curConeAxis);

		float curSqrDistance = FLT_EPSILON + dot(curCross, curCross);

		float prevRadius = max(prevDot * prevConeSlope + prevConeRadius, 0.0f);

		float curConeSlope = curHalfAxis.w * curInvConeHalfLength;
		float curRadius = max(curDot * curConeSlope + curConeRadius, 0.0f);

		float sine = curHalfAxis.w * curInvAxisHalfLength;
		float coneSqrCosine = 1.f - (sine * sine);

		float dotPrevPrev = dot(prevCross, prevCross) - prevRadius * prevRadius;
		float dotPrevCur = dot(prevCross, curCross) - prevRadius * curRadius;
		float dotCurCur = curSqrDistance - curRadius * curRadius;

		float discriminant = dotPrevCur * dotPrevCur - dotCurCur * dotPrevPrev;
		float sqrtD = sqrt(discriminant);
		float halfB = dotPrevCur - dotPrevPrev;
		float minusA = dotPrevCur - dotCurCur + halfB;

		// time of impact or 0 if prevPos inside cone
		float toi = min(0.0f, halfB + sqrtD) / minusA;
		bool hasCollision = (toi < 1.0f) && (halfB < sqrtD);

		// skip continuous collision if the (un-clamped) particle
		// trajectory only touches the outer skin of the cone.
		float rMin = prevRadius + halfB * minusA * (curRadius - prevRadius);
		hasCollision = hasCollision && (discriminant > minusA * rMin * rMin * gSkeletonWidth);

		// a is negative when one cone is contained in the other,
		// which is already handled by discrete collision.
		hasCollision = hasCollision && minusA < -FLT_EPSILON;

		if (hasCollision)
		{
			float3 delta = prevPos - curPos;

			// interpolate delta at toi
			float3 pos = prevPos - delta * toi;

			float3 curScaledAxis = curConeAxis.xyz * curAxisHalfLength;
			float3 deltaScaledAxis = curScaledAxis - prevConeAxis * prevAxisHalfLength;

			float oneMinusToi = 1.0f - toi;

			// interpolate axis at toi
			float3 axis = curScaledAxis - deltaScaledAxis * oneMinusToi;

			float slope = prevConeSlope * oneMinusToi + curConeSlope * toi;

			float sqrHalfLength = dot(axis, axis);
			float invHalfLength = rsqrt(sqrHalfLength);
			// distance along toi cone axis (from center)
			float dotf = dot(pos, axis) * invHalfLength;

			//point line distance
			float sqrDistance = dot(pos, pos) - dotf * dotf;
			float invDistance = sqrDistance > 0.0f ? rsqrt(sqrDistance) : 0.0f;

			//offset base to take slope in to account
			float base = dotf + slope * sqrDistance * invDistance;
			float scale = base * invHalfLength;
			// use invHalfLength to map base from [-HalfLength, +HalfLength]=inside to [-1, +1] =inside

			if (abs(scale) < 1.0f)
			{
				delta += deltaScaledAxis * scale;

				// reduce ccd impulse if (clamped) particle trajectory stays in cone skin,
				// i.e. scale by exp2(-k) or 1/(1+k) with k = (tmin - toi) / (1 - toi)
				float minusK = (oneMinusToi > FLT_EPSILON) ? sqrtD / (minusA * oneMinusToi) : 0.0f;
				oneMinusToi = oneMinusToi / (1.f - minusK);

				curPos += delta * oneMinusToi;

				curDot = dot(curPos, curConeAxis.xyz);
				curRadius = max(curDot * curConeSlope + curConeRadius, 0.0f);
				curSqrDistance = dot(curPos, curPos) - curDot * curDot;

				//curPos is relative to coneCenter so we don't need to do this:
				//curPos = coneCenter + curPos;
			}
		}
		
		{		
			if (curSqrDistance > curRadius * curRadius)
				continue;
		
			curSqrDistance = max(curSqrDistance, FLT_EPSILON);
			float invDistance = rsqrt(curSqrDistance);
		
			float base = curDot + curConeSlope * curSqrDistance * invDistance;
			float halfLength = curAxisHalfLength;
		
			if (abs(base) < halfLength)
			{
				float3 delta = curPos - base * curConeAxis;
		
				float sqrCosine = coneSqrCosine;
				float scale = curRadius * invDistance * sqrCosine - sqrCosine;
		
				outDelta += delta *scale;
		
				if (frictionEnabled)
				{
					// interpolate velocity between the two spheres
					float t = curDot * 0.5f + 0.5f;
					outVelocity += lerp(curSphere0.xyz - prevSphere0.xyz, curSphere1.xyz - prevSphere1.xyz, t);
				}
		
				++numCollisions;
			}
		}
	}

	// sphere collision
	for (uint32_t j = 0; j < gClothData.mNumSpheres; ++j)
	{
		float4 startSphere = bCollisionSpheres[startSphereOffset + j];
		float4 targetSphere = bCollisionSpheres[targetSphereOffset + j];
		float4 sphere = lerp(startSphere, targetSphere, alpha);
		sphere.w = max(sphere.w, 0.0f);
		float curRadius = sphere.w;

		// get previous sphere pos
		float4 prevSphere = lerp(startSphere, targetSphere, prevAlpha);
		prevSphere.w = max(sphere.w, 0.0f);
		float prevRadius = prevSphere.w;

		{
			float3 curDelta = curPosAbsolute - sphere.xyz;
			float3 prevDelta = prevPosAbsolute - prevSphere.xyz;

			float sqrDistance = FLT_EPSILON + dot(curDelta, curDelta);

			float dotPrevPrev = dot(prevDelta, prevDelta) - prevRadius * prevRadius;
			float dotPrevCur = dot(prevDelta, curDelta) - prevRadius * curRadius;
			float dotCurCur = sqrDistance - curRadius * curRadius;

			float discriminant = dotPrevCur * dotPrevCur - dotCurCur * dotPrevPrev;
			float sqrtD = sqrt(discriminant);
			float halfB = dotPrevCur - dotPrevPrev;
			float minusA = dotPrevCur - dotCurCur + halfB;

			// time of impact or 0 if prevPos inside sphere
			float toi = min(0.0f, halfB + sqrtD) / minusA;
			bool hasCollision = toi < 1.0f && halfB < sqrtD;

			// skip continuous collision if the (un-clamped) particle
			// trajectory only touches the outer skin of the cone.
			float rMin = prevRadius + halfB * minusA * (curRadius - prevRadius);
			hasCollision = hasCollision && (discriminant > minusA * rMin * rMin * gSkeletonWidth);

			// a is negative when one cone is contained in the other,
			// which is already handled by discrete collision.
			hasCollision = hasCollision && minusA < -FLT_EPSILON;

			if (hasCollision)
			{
				float3 delta = prevDelta - curDelta;

				float oneMinusToi = 1.0f - toi;

				// reduce ccd impulse if (clamped) particle trajectory stays in cone skin,
				// i.e. scale by exp2(-k) or 1/(1 + k) with k = (tmin - toi) / (1 - toi)
				float minusK = sqrtD / (minusA * oneMinusToi);
				oneMinusToi = oneMinusToi / (1.f - minusK);

				curDelta += delta * oneMinusToi;
				curPosAbsolute = sphere.xyz + curDelta;

				sqrDistance = FLT_EPSILON + dot(curDelta, curDelta);
			}

			float relDistance = rsqrt(sqrDistance) * sphere.w;

			if (relDistance > 1.0f)
			{
				float scale = relDistance - 1.0f;

				outDelta += curDelta * scale;

				if (frictionEnabled)
				{
					outVelocity += (sphere.xyz - prevSphere.xyz);
				}

				++numCollisions;
			}
		}
	}

	return numCollisions;
}

float3 barycentricLerp(float3 pA, float3 pB, float3 pC, float3 weights)
{
	float3 outP;
	outP.x = pA.x * weights.x + pB.x * weights.y + pC.x * weights.z;
	outP.y = pA.y * weights.x + pB.y * weights.y + pC.y * weights.z;
	outP.z = pA.z * weights.x + pB.z * weights.y + pC.z * weights.z;
	return outP;
}

void applyVirtualImpulse(IParticles particles, uint3 indices, float3 weights, float3 delta, float scale)
//apply
{
	float4 pA = particles.get(indices.x);
	float4 pB = particles.get(indices.y);
	float4 pC = particles.get(indices.z);

	delta *= scale;

	pA.x += delta.x * weights.x;	pB.x += delta.x * weights.y;	pC.x += delta.x * weights.z;
	pA.y += delta.y * weights.x;	pB.y += delta.y * weights.y;	pC.y += delta.y * weights.z;
	pA.z += delta.z * weights.x;	pB.z += delta.z * weights.y;	pC.z += delta.z * weights.z;

	//if(particle is not static) store new position
	if(pA.w != 0.0f) particles.set(indices.x, pA);
	if(pB.w != 0.0f) particles.set(indices.y, pB);
	if(pC.w != 0.0f) particles.set(indices.z, pC);
}

void collideVirtualCapsules(IParticles curParticles, IParticles prevParticles, uint32_t threadIdx, float alpha, float prevAlpha)
{
	uint virtualParticleSetSizesCount = gClothData.mNumVirtualParticleSetSizes;
	
	if(virtualParticleSetSizesCount == 0)
		return;

	bool frictionEnabled = gClothData.mFrictionScale > 0.0f;
	bool massScaleEnabled = gClothData.mCollisionMassScale > 0.0f;

	if(gClothData.mEnableContinuousCollision)
	{
		//???
	}

	uint indicesEnd = 0;
	for(uint vpSetSizeIt = 0; vpSetSizeIt < virtualParticleSetSizesCount; vpSetSizeIt++)
	{
		GroupMemoryBarrierWithGroupSync();
		uint indicesIt = indicesEnd + threadIdx * 4;
		for(indicesEnd += bVirtualParticleSetSizes[vpSetSizeIt]*4; indicesIt < indicesEnd; indicesIt += blockDim * 4)
		{
			uint4 indices;
			{
				uint4 tmp = bVirtualParticleIndices[indicesIt>>1];
				if(indicesIt & 1 == 1)
				{
					indices.x = tmp.z & 0xffff;
					indices.y = tmp.z >> 16;
					indices.z = tmp.w & 0xffff;
					indices.w = tmp.w >> 16;
				}
				else
				{
					indices.x = tmp.x & 0xffff;
					indices.y = tmp.x >> 16;
					indices.z = tmp.y & 0xffff;
					indices.w = tmp.y >> 16;
				}
			}

			float4 weights = bVirtualParticleWeights[indices.w];

			float3 curPos = barycentricLerp(curParticles.get(indices.x).xyz, curParticles.get(indices.y).xyz, curParticles.get(indices.z).xyz, weights.xyz);

			float3 delta, velocity;
			uint32_t numCollisions = collideCapsules(curPos.xyz, alpha, prevAlpha, delta, velocity);
			if(numCollisions > 0)
			{
				float scale = 1.0f / (float)numCollisions;
				float wscale = weights.w * scale;

				applyVirtualImpulse(curParticles, indices.xyz, weights.xyz, delta, scale);

				if(frictionEnabled)
				{
					float3 prevPos = barycentricLerp(prevParticles.get(indices.x).xyz, prevParticles.get(indices.y).xyz, prevParticles.get(indices.z).xyz, weights.xyz);
					
					float3 frictionImpulse =
						calcFrictionImpulse(prevPos.xyz, curPos.xyz, velocity, scale, delta);

					applyVirtualImpulse(prevParticles, indices.xyz, weights.xyz, frictionImpulse, -weights.w);
				}

				if(massScaleEnabled)
				{
					float deltaLengthSq = dot(delta.xyz, delta.xyz) * scale * scale;
					float invMassScale = 1.0f / (1.0f + gClothData.mCollisionMassScale * deltaLengthSq);

					// not multiplying by weights[3] here because unlike applying velocity
					// deltas where we want the interpolated position to obtain a particular
					// value, we instead just require that the total change is equal to invMassScale
					invMassScale = invMassScale - 1.0f;

					float4 pA = curParticles.get(indices.x);
					float4 pB = curParticles.get(indices.y);
					float4 pC = curParticles.get(indices.z);

					pA.w *= 1.0f + weights.x * invMassScale;
					pB.w *= 1.0f + weights.y * invMassScale;
					pC.w *= 1.0f + weights.z * invMassScale;

					curParticles.set(indices.x,pA); 
					curParticles.set(indices.y,pB); 
					curParticles.set(indices.z,pC);
				}
			}
		}
	}
}

void collideContinuousCapsules(IParticles curParticles, IParticles prevParticles, uint32_t threadIdx, float alpha, float prevAlpha)
{
	bool frictionEnabled = gClothData.mFrictionScale > 0.0f;
	bool massScaleEnabled = gClothData.mCollisionMassScale > 0.0f;

	for (uint32_t j = threadIdx; j < gClothData.mNumParticles; j += blockDim)
	{
		float4 curPos = curParticles.get(j);
		float4 prevPos = prevParticles.get(j);

		float3 delta, velocity;
		uint32_t numCollisions = collideCapsules(curPos.xyz, prevPos.xyz, alpha, prevAlpha, delta, velocity);
		if (numCollisions > 0)
		{
			float scale = 1.0f / (float)numCollisions;

			if (frictionEnabled)
			{
				float3 frictionImpulse =
				calcFrictionImpulse(prevPos.xyz, curPos.xyz, velocity, scale, delta);

				prevPos.xyz -= frictionImpulse;
				prevParticles.set(j, prevPos);
			}

			curPos.xyz += delta * scale;

			// TODO: current impl. causes glitches - fix it!
		//	if (massScaleEnabled)
		//	{
		//		float deltaLengthSq = dot(delta, delta);
		//		float massScale = 1.0f + gClothData.mCollisionMassScale * deltaLengthSq;
		//		curPos.w /= massScale;
		//	}

			curParticles.set(j, curPos);
		}
	}
}


void collideParticles(IParticles curParticles, IParticles prevParticles, uint32_t threadIdx, float alpha, float prevAlpha)
{
	collideConvexes(curParticles, prevParticles, threadIdx, alpha);
	collideTriangles(curParticles, threadIdx, alpha);
	if (gClothData.mEnableContinuousCollision)
		collideContinuousCapsules(curParticles, prevParticles, threadIdx, alpha, prevAlpha);
	else
		collideCapsules(curParticles, prevParticles, threadIdx, alpha, prevAlpha);
	collideVirtualCapsules(curParticles, prevParticles, threadIdx, alpha, prevAlpha);
}

void constrainSeparation(IParticles curParticles, uint32_t threadIdx, float alpha)
{
	if (gFrameData.mStartSeparationConstrainsOffset == -1)
		return;

	for (uint32_t j = threadIdx; j < gClothData.mNumParticles; j += blockDim)
	{
		float4 startPos = bSeparationConstraints[j + gFrameData.mStartSeparationConstrainsOffset];
		float4 targetPos = bSeparationConstraints[j + gFrameData.mTargetSeparationConstrainsOffset];

		float4 sphere = startPos + (targetPos - startPos) * alpha;

		float4 current = curParticles.get(j);

		float3 delta = sphere.xyz - current.xyz;

		float sqrLength = FLT_EPSILON + dot(delta, delta);

		float slack = min(0.0f, 1.0f - sphere.w * rsqrt(sqrLength));

		current.xyz += slack * delta;
		curParticles.set(j, current);
	}
	GroupMemoryBarrierWithGroupSync();
}

void updateSleepState(IParticles curParticles, IParticles prevParticles, uint32_t threadIdx)
{
	if (!threadIdx)
		gFrameData.mSleepTestCounter += max(1, uint32_t(gFrameData.mIterDt * 1000));

	GroupMemoryBarrierWithGroupSync();

	if (gFrameData.mSleepTestCounter < gClothData.mSleepTestInterval)
		return;

	float maxDelta = 0.0f;
	for (uint32_t i = threadIdx; i < gClothData.mNumParticles; i += blockDim)
	{
		float4 curPos = curParticles.get(i);
		float4 prevPos = prevParticles.get(i);

		float3 delta = abs(curPos.xyz - prevPos.xyz);

		maxDelta = max(max(max(delta.x, delta.y), delta.z), maxDelta);
	}

	if (!threadIdx)
	{
		++gFrameData.mSleepPassCounter;
		gFrameData.mSleepTestCounter -= gClothData.mSleepTestInterval;
	}

	if (maxDelta > gClothData.mSleepThreshold * gFrameData.mIterDt)
		gFrameData.mSleepPassCounter = 0;
}


#define USE_SELF_COLLISION_SORT 1

struct DxSelfCollisionGrid
{
	float mPosBias[3];
	float mPosScale[3];
	uint32_t mPosElemId[3];
};
groupshared DxSelfCollisionGrid gSelfCollisionGrid;
groupshared float gExpandedEdgeLength[3];

void selfCollideParticles(IParticles curParticles, uint32_t threadIdx)
{
	if (min(gClothData.mSelfCollisionDistance, gFrameData.mSelfCollisionStiffness) <= 0.0f)
	{
		return;
	}

	const int32_t numIndices = gClothData.mNumSelfCollisionIndices;
	const int32_t numParticles = gClothData.mNumParticles;

#if USE_SELF_COLLISION_SORT
	float expandedNegativeLower = 0;
	float expandedEdgeLength = 0;
	if (threadIdx.x < 3)
	{
		float upper = gFrameData.mParticleBounds[threadIdx.x * 2];
		float negativeLower = gFrameData.mParticleBounds[threadIdx.x * 2 + 1];

		// expand bounds
		float eps = (upper + negativeLower) * 1e-4f;
		float expandedUpper = upper + eps;
		expandedNegativeLower = negativeLower + eps;
		expandedEdgeLength = expandedUpper + expandedNegativeLower;

		gExpandedEdgeLength[threadIdx.x] = expandedEdgeLength;
	}
	GroupMemoryBarrierWithGroupSync();
	if (threadIdx.x < 3)
	{
		// calculate shortest axis
		int32_t shortestAxis = gExpandedEdgeLength[0] > gExpandedEdgeLength[1];
		if (gExpandedEdgeLength[shortestAxis] > gExpandedEdgeLength[2])
			shortestAxis = 2;

		uint32_t writeAxis = threadIdx.x - shortestAxis;
		writeAxis += writeAxis >> 30;

		float maxInvCellSize = (127.0f / expandedEdgeLength);
		float invCollisionDistance = (1.0f / gClothData.mSelfCollisionDistance);
		float invCellSize = min(maxInvCellSize, invCollisionDistance);

		gSelfCollisionGrid.mPosScale[writeAxis] = invCellSize;
		gSelfCollisionGrid.mPosBias[writeAxis] = invCellSize * expandedNegativeLower;
		gSelfCollisionGrid.mPosElemId[writeAxis] = threadIdx.x;
	}
	GroupMemoryBarrierWithGroupSync();

	const int32_t cellStartOffset = gClothData.mSelfCollisionDataOffset + numIndices * 2;
	const int32_t cellStartSize = (129 + 128 * 128 + 130);
	if (gFrameData.mInitSelfCollisionData)
	{
		for (int32_t i = threadIdx; i < cellStartSize; i += BlockSize)
		{
			bSelfCollisionData[cellStartOffset + i] = -1;
		}
	}

	//build acceleration grid
	float rowScale = gSelfCollisionGrid.mPosScale[1], rowBias = gSelfCollisionGrid.mPosBias[1];
	float colScale = gSelfCollisionGrid.mPosScale[2], colBias = gSelfCollisionGrid.mPosBias[2];
	int32_t rowElemId = gSelfCollisionGrid.mPosElemId[1];
	int32_t colElemId = gSelfCollisionGrid.mPosElemId[2];

	// calculate keys
	for (int32_t i = threadIdx.x; i < numIndices; i += BlockSize)
	{
		int32_t index = gClothData.mSelfCollisionIndicesOffset != -1 ? bSelfCollisionIndices[gClothData.mSelfCollisionIndicesOffset + i] : i;
		//assert(index < gClothData.mNumParticles);

		float4 pos = curParticles.get(index);

		int32_t rowIndex = int32_t(max(0.0f, min(pos[rowElemId] * rowScale + rowBias, 127.5f)));
		int32_t colIndex = int32_t(max(0.0f, min(pos[colElemId] * colScale + colBias, 127.5f)));
		//assert(rowIndex >= 0 && rowIndex < 128 && colIndex >= 0 && colIndex < 128);

		int32_t key = (colIndex << 7 | rowIndex) + 129; // + row and column sentinel
		//assert(key <= 0x4080);

		bSelfCollisionData[gClothData.mSelfCollisionDataOffset + i] = key << 16 | index; // (key, index) pair in a single int32_t
	}
	GroupMemoryBarrierWithGroupSync();

	// sort keys
	class SelfCollisionKeys : ISortElements
	{
		int inOffset;
		int outOffset;
		int get(int index)
		{
			return bSelfCollisionData[inOffset + index];
		}
		void set(int index, int value)
		{
			bSelfCollisionData[outOffset + index] = value;
		}
		void swap()
		{
			int temp = inOffset;
			inOffset = outOffset;
			outOffset = temp;
		}
	} sortedKeys;
	sortedKeys.inOffset = gClothData.mSelfCollisionDataOffset;
	sortedKeys.outOffset = gClothData.mSelfCollisionDataOffset + numIndices;


	class SortShared : ISortShared
	{
		uint4 getReduce(int index)
		{
			uint4 res;
			res.x = (gCurParticles[index + BlockSize * 0]);
			res.y = (gCurParticles[index + BlockSize * 1]);
			res.z = (gCurParticles[index + BlockSize * 2]);
			res.w = (gCurParticles[index + BlockSize * 3]);
			return res;
		}
		void setReduce(int index, uint4 value)
		{
			gCurParticles[index + BlockSize * 0] = (value.x);
			gCurParticles[index + BlockSize * 1] = (value.y);
			gCurParticles[index + BlockSize * 2] = (value.z);
			gCurParticles[index + BlockSize * 3] = (value.w);
		}

		uint getScan(int index)
		{
			return gCurParticles[index + BlockSize * 4];
		}
		void setScan(int index, uint value)
		{
			gCurParticles[index + BlockSize * 4] = value;
		}
	} sortShared;
#endif

	// copy current particles to temporary array (radixSort reuses the same shared memory used for particles!)
	for (int32_t j = threadIdx; j < numParticles; j += blockDim)
	{
		bSelfCollisionParticles[gClothData.mSelfCollisionParticlesOffset + j] = curParticles.get(j);
	}
	GroupMemoryBarrierWithGroupSync();

#if USE_SELF_COLLISION_SORT
	radixSort_BitCount(threadIdx, numIndices, sortedKeys, 16, 32, sortShared);

	// mark cell start if keys are different between neighboring threads
	for (int32_t k = threadIdx.x; k < numIndices; k += BlockSize)
	{
		int32_t key = sortedKeys.get(k) >> 16;
		int32_t prevKey = k ? sortedKeys.get(k - 1) >> 16 : key - 1;
		if (key != prevKey)
		{
			bSelfCollisionData[cellStartOffset + key] = k;
			bSelfCollisionData[cellStartOffset + prevKey + 1] = k;
		}
	}
#endif
	//GroupMemoryBarrierWithGroupSync();

#if USE_SELF_COLLISION_SORT
	// copy only sorted (indexed) particles to shared mem
	for (i = threadIdx.x; i < numIndices; i += blockDim)
	{
		int32_t index = bSelfCollisionData[gClothData.mSelfCollisionDataOffset + i] & 0xFFFF;
		curParticles.set(i, bSelfCollisionParticles[gClothData.mSelfCollisionParticlesOffset + index]);
	}
	GroupMemoryBarrierWithGroupSync();

	const float cdist = gClothData.mSelfCollisionDistance;
	const float cdistSq = cdist * cdist;

	for (i = threadIdx; i < numIndices; i += blockDim)
#else
	for (i = threadIdx; i < numParticles; i += blockDim)
#endif
	{
#if USE_SELF_COLLISION_SORT
		const int32_t index = bSelfCollisionData[gClothData.mSelfCollisionDataOffset + i] & 0xFFFF;
#else
		const int32_t index = i;
#endif
		//assert(index < gClothData.mNumParticles);

		float4 iPos = curParticles.get(i);
		float4 delta = float4(0.0f, 0.0f, 0.0f, FLT_EPSILON);

		float4 iRestPos;
		if (gFrameData.mRestPositionsOffset != -1)
		{
			iRestPos = bRestPositions[gFrameData.mRestPositionsOffset + index];
		}

#if USE_SELF_COLLISION_SORT
		// get cell index for this particle
		int32_t rowIndex = int32_t(max(0.0f, min(iPos[rowElemId] * rowScale + rowBias, 127.5f)));
		int32_t colIndex = int32_t(max(0.0f, min(iPos[colElemId] * colScale + colBias, 127.5f)));
		//assert(rowIndex >= 0 && rowIndex < 128 && colIndex >= 0 && colIndex < 128);

		int32_t key = colIndex << 7 | rowIndex;
		//assert(key <= 0x4080);

		// check cells in 3 columns
		for (int32_t keyEnd = key + 256; key <= keyEnd; key += 128)
		{
			uint32_t cellStart[4];
			cellStart[0] = bSelfCollisionData[cellStartOffset + key + 0];
			cellStart[1] = bSelfCollisionData[cellStartOffset + key + 1];
			cellStart[2] = bSelfCollisionData[cellStartOffset + key + 2];
			cellStart[3] = bSelfCollisionData[cellStartOffset + key + 3];

			uint32_t startIndex = min(min(cellStart[0], cellStart[1]), cellStart[2]);
			uint32_t endIndex   = max(max(max(asint(cellStart[1]), asint(cellStart[2])), asint(cellStart[3])), 0);
#else
		{
			uint32_t startIndex = 0;
			uint32_t endIndex = numParticles;
#endif
			// comparison must be unsigned to skip cells with negative startIndex
			for (int32_t j = startIndex; asuint(j) < endIndex; ++j)
			{
				if (j != i) // avoid same particle
				{
					float4 jPos = curParticles.get(j);
					float3 diff = iPos.xyz - jPos.xyz;

					float distSqr = dot(diff, diff);
					if (distSqr > cdistSq)
						continue;

					float restScale = 1.0f;
					if (gFrameData.mRestPositionsOffset != -1)
					{
#if USE_SELF_COLLISION_SORT
						const int32_t jndex = bSelfCollisionData[gClothData.mSelfCollisionDataOffset + j] & 0xFFFF;
#else
						const int32_t jndex = j;
#endif

						float4 jRestPos = bRestPositions[gFrameData.mRestPositionsOffset + jndex];

						// calculate distance in rest configuration
						float3 rdiff = iRestPos.xyz - jRestPos.xyz;
						float rdistSq = dot(rdiff, rdiff);
						if (rdistSq <= cdistSq)
							continue;

						// ratio = rest distance / collision distance - 1.0
						float stiffnessRatio = rsqrt(cdistSq / (rdistSq + FLT_EPSILON)) - 1.0f;
						restScale = min(1.0, stiffnessRatio);
					}
					// premultiply ratio for weighted average
					float ratio = max(0.0f, cdist * rsqrt(FLT_EPSILON + distSqr) - 1.0f);
					float scale = (restScale * ratio * ratio) / (FLT_EPSILON + iPos.w + jPos.w);

					delta.xyz += scale * diff;
					delta.w += ratio;
				}
			}
		}
		const float stiffness = gFrameData.mSelfCollisionStiffness * iPos.w;
		float scale = (stiffness / delta.w);

		// apply collision impulse
		float4 tmpPos = bSelfCollisionParticles[gClothData.mSelfCollisionParticlesOffset + index];
		tmpPos.xyz += delta.xyz * scale;
		bSelfCollisionParticles[gClothData.mSelfCollisionParticlesOffset + index] = tmpPos;
	}
	GroupMemoryBarrierWithGroupSync();

	// copy temporary particle array back to shared mem
	for (i = threadIdx; i < numParticles; i += blockDim)
	{
		curParticles.set(i, bSelfCollisionParticles[gClothData.mSelfCollisionParticlesOffset + i]);
	}

	// unmark occupied cells to empty again (faster than clearing all the cells)
	for (i = threadIdx.x; i < numIndices; i += blockDim)
	{
		int32_t key = bSelfCollisionData[gClothData.mSelfCollisionDataOffset + i] >> 16;
		bSelfCollisionData[cellStartOffset + key] = -1;
		bSelfCollisionData[cellStartOffset + key + 1] = -1;
	}
	GroupMemoryBarrierWithGroupSync();
}

void computeParticleBounds(IParticles curParticles, uint32_t threadIdx)
{
	if (threadIdx < 192)
	{
		int32_t axisIdx = threadIdx >> 6; // x, y, or z
		float signf = (threadIdx & 32) ? -1.0f : +1.0f;  // sign bit (min or max)

		uint32_t curIt = min(threadIdx.x & 31, gClothData.mNumParticles - 1);

		gBounds[threadIdx] = curParticles.get(curIt)[axisIdx] * signf;
		while (curIt += 32, curIt < gClothData.mNumParticles)
		{
			gBounds[threadIdx] = max(gBounds[threadIdx], curParticles.get(curIt)[axisIdx] * signf);
		}
	}
	GroupMemoryBarrierWithGroupSync();
	if (threadIdx < 192 - 16)
	{
		gBounds[threadIdx] = max(gBounds[threadIdx], gBounds[threadIdx + 16]);
	}
	GroupMemoryBarrierWithGroupSync();
	if (threadIdx < 192 - 16)
	{
		gBounds[threadIdx] = max(gBounds[threadIdx], gBounds[threadIdx + 8]);
	}
	GroupMemoryBarrierWithGroupSync();
	if (threadIdx < 192 - 16)
	{
		gBounds[threadIdx] = max(gBounds[threadIdx], gBounds[threadIdx + 4]);
	}
	GroupMemoryBarrierWithGroupSync();
	if (threadIdx < 192 - 16)
	{
		gBounds[threadIdx] = max(gBounds[threadIdx], gBounds[threadIdx + 2]);
	}
	GroupMemoryBarrierWithGroupSync();
	if (threadIdx < 192 - 16)
	{
		gBounds[threadIdx] = max(gBounds[threadIdx], gBounds[threadIdx + 1]);
	}
	GroupMemoryBarrierWithGroupSync();

	if (threadIdx.x < 192 && !(threadIdx & 31))
	{
		gFrameData.mParticleBounds[threadIdx >> 5] = gBounds[threadIdx];
	}
	GroupMemoryBarrierWithGroupSync();
}


void simulateCloth(IParticles curParticles, IParticles prevParticles, uint32_t threadIdx)
{
	for (uint32_t i = 0; i < gFrameData.mNumIterations; ++i)
	{
		const float alpha = (i + 1.0f) / gFrameData.mNumIterations;

		if (!threadIdx)
			gIterData = bIterData[gFrameData.mFirstIteration + i];
		GroupMemoryBarrierWithGroupSync();

		integrateParticles(curParticles, prevParticles, threadIdx);
		accelerateParticles(curParticles, threadIdx);
		applyWind(curParticles, prevParticles, threadIdx);
		constrainMotion(curParticles, threadIdx, alpha);
		constrainTether(curParticles, threadIdx);
		// note: GroupMemoryBarrierWithGroupSync at beginning of each fabric phase
		solveFabric(curParticles, threadIdx);
		GroupMemoryBarrierWithGroupSync();
		constrainSeparation(curParticles, threadIdx, alpha);
		computeParticleBounds(curParticles, threadIdx);
		collideParticles(curParticles, prevParticles, threadIdx, alpha, float(i) / gFrameData.mNumIterations);
		selfCollideParticles(curParticles, threadIdx);
		updateSleepState(curParticles, prevParticles, threadIdx);
	}
	GroupMemoryBarrierWithGroupSync();
}

class ParticlesInSharedMem : IParticles
{
	float4 get(uint32_t index)
	{
		float4 res;
		res.x = asfloat(gCurParticles[index + MaxParticlesInSharedMem * 0]);
		res.y = asfloat(gCurParticles[index + MaxParticlesInSharedMem * 1]);
		res.z = asfloat(gCurParticles[index + MaxParticlesInSharedMem * 2]);
		res.w = asfloat(gCurParticles[index + MaxParticlesInSharedMem * 3]);
		return res;
	}
	void set(uint32_t index, float4 value)
	{
		gCurParticles[index + MaxParticlesInSharedMem * 0] = asuint(value.x);
		gCurParticles[index + MaxParticlesInSharedMem * 1] = asuint(value.y);
		gCurParticles[index + MaxParticlesInSharedMem * 2] = asuint(value.z);
		gCurParticles[index + MaxParticlesInSharedMem * 3] = asuint(value.w);
	}
	void atomicAdd(uint32_t index, float3 value)
	{
		interlockedAddFloat(index + MaxParticlesInSharedMem * 0, value.x);
		interlockedAddFloat(index + MaxParticlesInSharedMem * 1, value.y);
		interlockedAddFloat(index + MaxParticlesInSharedMem * 2, value.z);
	}

	void interlockedAddFloat(uint addr, float value)
	{
		uint comp, original = gCurParticles[addr];
		[allow_uav_condition]do
		{
			InterlockedCompareExchange(gCurParticles[addr], comp = original, asuint(asfloat(original) + value), original);
		} while(original != comp);
	}
};

class ParticlesInGlobalMem : IParticles
{
	uint32_t _offset;

	float4 get(uint32_t index)
	{
		return asfloat(bParticles.Load4((_offset + index)*16));
	}
	void set(uint32_t index, float4 value)
	{
		bParticles.Store4((_offset + index) * 16, asuint(value));
	}
	void atomicAdd(uint32_t index, float3 value)
	{
		interlockedAddFloat((_offset + index) * 16 + 0, value.x);
		interlockedAddFloat((_offset + index) * 16 + 4, value.y);
		interlockedAddFloat((_offset + index) * 16 + 8, value.z);
	}

	void interlockedAddFloat(uint addr, float value)
	{
		uint comp, original = bParticles.Load(addr);
		[allow_uav_condition]do
		{
			bParticles.InterlockedCompareExchange(addr, comp = original, asuint(asfloat(original) + value), original);
		} while(original != comp);
	}
};


[numthreads(blockDim, 1, 1)] void main(uint32_t blockIdx : SV_GroupID, uint32_t threadIdx : SV_GroupThreadID)
{
	if (!threadIdx)
	{
		gClothData = bClothData[blockIdx];
		gFrameData = bFrameData[blockIdx];
	}
	GroupMemoryBarrierWithGroupSync(); // wait for gClothData being written

	ParticlesInGlobalMem prevParticles;
	prevParticles._offset = gClothData.mParticlesOffset + gClothData.mNumParticles;

	if (gClothData.mNumParticles <= MaxParticlesInSharedMem)
	{
		ParticlesInSharedMem curParticles;

		uint32_t i;
		for (i = threadIdx; i < gClothData.mNumParticles; i += blockDim)
		{
			curParticles.set(i, asfloat(bParticles.Load4((gClothData.mParticlesOffset + i) * 16)));
		}

		simulateCloth(curParticles, prevParticles, threadIdx);

		for (i = threadIdx; i < gClothData.mNumParticles; i += blockDim)
		{
			bParticles.Store4((gClothData.mParticlesOffset + i) * 16, asuint(curParticles.get(i)));
		}
	}
	else
	{
		ParticlesInGlobalMem curParticles;
		curParticles._offset = gClothData.mParticlesOffset;

		simulateCloth(curParticles, prevParticles, threadIdx);
	}

	if (!threadIdx)
	{
		bFrameData[blockIdx] = gFrameData;
	}
}
