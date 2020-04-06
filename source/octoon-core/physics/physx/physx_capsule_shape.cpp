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
			, renderer_(context)
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
			if (px_material)
			{
				px_material->release();
				px_material = nullptr;
			}

			if (px_shape)
			{
				px_shape->release();
				px_shape = nullptr;
			}
		}

		math::float3 PhysxCapsuleShape::getCenter()
		{
			auto pxLocalPose = px_shape->getLocalPose();
			return math::float3(pxLocalPose.p.x, pxLocalPose.p.y, pxLocalPose.p.z);
		}

		void PhysxCapsuleShape::setCenter(const math::float3& center)
		{
			auto pxLocalPose = px_shape->getLocalPose();
			pxLocalPose.p = physx::PxVec3(center.x, center.y, center.z);
			px_shape->setLocalPose(pxLocalPose);
		}

		math::Quaternion
		PhysxCapsuleShape::getQuaternion()
		{
			auto pxLocalPose = px_shape->getLocalPose();
			return math::Quaternion(pxLocalPose.q.x, pxLocalPose.q.y, pxLocalPose.q.z, pxLocalPose.q.w);
		}

		void
		PhysxCapsuleShape::setQuaternion(const math::Quaternion& q)
		{
			auto pxLocalPose = px_shape->getLocalPose();
			pxLocalPose.q = physx::PxQuat(q.x, q.y, q.z, q.w);
			pxLocalPose.q *= physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1));
			px_shape->setLocalPose(pxLocalPose);
		}

		void
		PhysxCapsuleShape::setContactOffset(float offset)
		{
			px_shape->setContactOffset(offset);
		}

		float
		PhysxCapsuleShape::getContactOffset() const
		{
			return px_shape->getContactOffset();
		}

		void
		PhysxCapsuleShape::setRestOffset(float offset)
		{
			px_shape->setRestOffset(offset);
		}

		float 
		PhysxCapsuleShape::getRestOffset() const
		{
			return px_shape->getRestOffset();
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
			return px_material;
		}
    }
}