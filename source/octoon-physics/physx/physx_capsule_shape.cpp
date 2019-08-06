#include "physx_capsule_shape.h"
#include "physx_context.h"

#include <octoon/runtime/except.h>

#include <PxPhysicsAPI.h>

namespace octoon
{
    namespace physics
    {
        PhysxCapsuleShape::PhysxCapsuleShape(PhysxContext* context, float radius, float height)
			: px_shape(nullptr)
			, context_(context)
		{
			physx::PxCapsuleGeometry geometry(radius, height*0.5f);
			px_material = context->getPxPhysics()->createMaterial(0.f, 0.f, 0.f);
			px_shape = context->getPxPhysics()->createShape(geometry, *px_material, true);
			if (px_shape)
				px_shape->setLocalPose(physx::PxTransform(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1))));
			else
				runtime::runtime_error::create("create shape failed!");
		}

		PhysxCapsuleShape::~PhysxCapsuleShape() noexcept
		{
			px_shape->release();
			px_shape = nullptr;
		}

		math::float3 PhysxCapsuleShape::getCenter()
		{
			auto pxLocalPose = px_shape->getLocalPose();
			return math::float3(pxLocalPose.p.x, pxLocalPose.p.y, pxLocalPose.p.z);
		}

		void PhysxCapsuleShape::setCenter(math::float3 center)
		{
			auto pxLocalPose = px_shape->getLocalPose();
			pxLocalPose.p = physx::PxVec3(center.x, center.y, center.z);
			px_shape->setLocalPose(pxLocalPose);
		}

		void PhysxCapsuleShape::setRadius(float radius)
		{
			physx::PxCapsuleGeometry geometry;
			if (px_shape->getCapsuleGeometry(geometry))
			{
				geometry.radius = radius;
				px_shape->setGeometry(geometry);
			}
		}

		void  PhysxCapsuleShape::setHeight(float height)
		{
			physx::PxCapsuleGeometry geometry;
			if (px_shape->getCapsuleGeometry(geometry))
			{
				geometry.halfHeight = height*0.5f;
				px_shape->setGeometry(geometry);
			}
		}
	
		float PhysxCapsuleShape::getRadius() const
		{
			physx::PxCapsuleGeometry geometry;
			if (px_shape->getCapsuleGeometry(geometry))
				return geometry.radius;
			else
				return 0.f;
		}

		float PhysxCapsuleShape::getHeight() const
		{
			physx::PxCapsuleGeometry geometry;
			if (px_shape->getCapsuleGeometry(geometry))
				return geometry.halfHeight*2.0f;
			else
				return 0.f;
		}

		physx::PxShape * PhysxCapsuleShape::getPxShape()
		{
			return px_shape;
		}

		physx::PxMaterial * PhysxCapsuleShape::getPxMaterial()
		{
			return nullptr;
		}
    }
}