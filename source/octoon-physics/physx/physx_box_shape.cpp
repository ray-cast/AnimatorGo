#include "physx_box_shape.h"
#include "physx_context.h"

#include <octoon/runtime/except.h>

#include <PxPhysicsAPI.h>

namespace octoon
{
    namespace physics
    {
        PhysxBoxShape::PhysxBoxShape(PhysxContext* context, float hx, float hy, float hz)
			:px_shape(nullptr)
			, context_(context)
		{
			physx::PxBoxGeometry geometry(hx*0.5f, hy*0.5f, hz*0.5f);
			px_material = context->getPxPhysics()->createMaterial(0.f, 0.f, 0.f);
			px_shape = context->getPxPhysics()->createShape(geometry, *px_material, true);
			if (!px_shape)
				runtime::runtime_error::create("create shape failed!");
		}

		PhysxBoxShape::~PhysxBoxShape() noexcept
		{
			px_shape->release();
			px_shape = nullptr;
		}

		math::float3 PhysxBoxShape::getCenter()
		{
			auto pxLocalPose = px_shape->getLocalPose();
			return math::float3(pxLocalPose.p.x, pxLocalPose.p.y, pxLocalPose.p.z);
		}

		void PhysxBoxShape::setCenter(math::float3 center)
		{
			auto pxLocalPose = px_shape->getLocalPose();
			pxLocalPose.p = physx::PxVec3(center.x, center.y, center.z);
			px_shape->setLocalPose(pxLocalPose);
		}

		void PhysxBoxShape::setWidth(float width)
		{
			physx::PxBoxGeometry geometry;
			if (px_shape->getBoxGeometry(geometry))
			{
				geometry.halfExtents.x = width*0.5f;
				px_shape->setGeometry(geometry);
			}
		}

		void  PhysxBoxShape::setHeight(float height)
		{
			physx::PxBoxGeometry geometry;
			if (px_shape->getBoxGeometry(geometry))
			{
				geometry.halfExtents.y = height*0.5f;
				px_shape->setGeometry(geometry);
			}
		}

		void  PhysxBoxShape::setDepth(float depth)
		{
			physx::PxBoxGeometry geometry;
			if (px_shape->getBoxGeometry(geometry))
			{
				geometry.halfExtents.z = depth*0.5f;
				px_shape->setGeometry(geometry);
			}
		}

		float PhysxBoxShape::getWidth() const
		{
			physx::PxBoxGeometry geometry;
			if (px_shape->getBoxGeometry(geometry))
				return geometry.halfExtents.x*2.0f;
			else
				return 0.f;
		}

		float PhysxBoxShape::getHeight() const
		{
			physx::PxBoxGeometry geometry;
			if (px_shape->getBoxGeometry(geometry))
				return geometry.halfExtents.y*2.0f;
			else
				return 0.f;
		}

		float PhysxBoxShape::getDepth() const
		{
			physx::PxBoxGeometry geometry;
			if (px_shape->getBoxGeometry(geometry))
				return geometry.halfExtents.z*2.0f;
			else
				return 0.f;
		}

		physx::PxShape * PhysxBoxShape::getPxShape()
		{
			return px_shape;
		}

		physx::PxMaterial * PhysxBoxShape::getPxMaterial()
		{
			return px_material;
		}
    }
}