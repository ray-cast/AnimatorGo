#include "physx_sphere_shape.h"

#include <octoon/runtime/except.h>

#include "physx_context.h"

#include <PxPhysicsAPI.h>

namespace octoon
{
    namespace physics
    {
        PhysxSphereShape::PhysxSphereShape(PhysxContext* context, float radius)
			:px_shape(nullptr)
			, context_(context)
		{
			physx::PxSphereGeometry geometry(radius);
			px_material = context->getPxPhysics()->createMaterial(0.f, 0.f, 0.f);
			px_shape = context->getPxPhysics()->createShape(geometry, *px_material, true);
			if (!px_shape)
				runtime::runtime_error::create("create shape failed!");
		}

		PhysxSphereShape::~PhysxSphereShape() noexcept
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

		void PhysxSphereShape::setCenter(const math::float3& center)
		{
			auto pxLocalPose = px_shape->getLocalPose();
			pxLocalPose.p = physx::PxVec3(center.x, center.y, center.z);
			px_shape->setLocalPose(pxLocalPose);
		}

		math::float3 PhysxSphereShape::getCenter()
		{
			auto pxLocalPose = px_shape->getLocalPose();
			return math::float3(pxLocalPose.p.x, pxLocalPose.p.y, pxLocalPose.p.z);
		}

		math::Quaternion
		PhysxSphereShape::getQuaternion()
		{
			auto pxLocalPose = px_shape->getLocalPose();
			return math::Quaternion(pxLocalPose.q.x, pxLocalPose.q.y, pxLocalPose.q.z, pxLocalPose.q.w);
		}

		void
		PhysxSphereShape::setQuaternion(const math::Quaternion& q)
		{
			auto pxLocalPose = px_shape->getLocalPose();
			pxLocalPose.q = physx::PxQuat(q.x, q.y, q.z, q.w);
			px_shape->setLocalPose(pxLocalPose);
		}

		void
		PhysxSphereShape::setContactOffset(float offset)
		{
			px_shape->setContactOffset(offset);
		}

		float
		PhysxSphereShape::getContactOffset() const
		{
			return px_shape->getContactOffset();
		}

		void
		PhysxSphereShape::setRestOffset(float offset)
		{
			px_shape->setRestOffset(offset);
		}

		float 
		PhysxSphereShape::getRestOffset() const
		{
			return px_shape->getRestOffset();
		}

		float PhysxSphereShape::getRadius()
		{
			physx::PxSphereGeometry geometry;
			if (px_shape->getSphereGeometry(geometry))
				return geometry.radius;
			else
				return 0.f;
		}

		void PhysxSphereShape::setRadius(float radius)
		{
			physx::PxSphereGeometry geometry;
			if (px_shape->getSphereGeometry(geometry))
			{
				geometry.radius = radius;
				px_shape->setGeometry(geometry);
			}
		}

		physx::PxShape * PhysxSphereShape::getPxShape()
		{
			return px_shape;
		}

		physx::PxMaterial * PhysxSphereShape::getPxMaterial()
		{
			return px_material;
		}
    }
}