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
			px_shape->release();
			px_shape = nullptr;
		}

		math::float3 PhysxSphereShape::getCenter()
		{
			auto pxLocalPose = px_shape->getLocalPose();
			return math::float3(pxLocalPose.p.x, pxLocalPose.p.y, pxLocalPose.p.z);
		}

		void PhysxSphereShape::setCenter(math::float3 center)
		{
			auto pxLocalPose = px_shape->getLocalPose();
			pxLocalPose.p = physx::PxVec3(center.x, center.y, center.z);
			px_shape->setLocalPose(pxLocalPose);
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