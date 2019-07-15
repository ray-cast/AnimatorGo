#include "physx_rigidbody.h"

#include "physx_context.h"
#include "physx_shape.h"

#include<octoon/runtime/except.h>

#include <PxPhysicsAPI.h>

namespace octoon
{
	namespace physics
	{
		PhysxRigidbody::PhysxRigidbody(PhysxContext * context, PhysicsRigidbodyDesc desc)
			:context(nullptr), px_rigidbody(nullptr)
		{
			physx::PxTransform pose;
			pose.p = physx::PxVec3(desc.translate.x, desc.translate.y, desc.translate.z);
			pose.q = physx::PxQuat(desc.rotation.x, desc.rotation.y, desc.rotation.z, desc.rotation.w);

			if (desc.type == PhysicsRigidbodyType::Static)
			{
				auto rigidbody = context->getPxPhysics()->createRigidStatic(pose);
				if (!rigidbody)
					throw  runtime::runtime_error::create("create body failed!");
				px_rigidbody = rigidbody;
			}
			else if(desc.type == PhysicsRigidbodyType::Dynamic)
			{
				auto rigidbody = context->getPxPhysics()->createRigidDynamic(pose);
				if (!rigidbody)
					throw  runtime::runtime_error::create("create body failed!");

				rigidbody->setMass(desc.mass);

				px_rigidbody = rigidbody;
			}
		}

		PhysxRigidbody::~PhysxRigidbody()
		{
			px_rigidbody->release();
			px_rigidbody = nullptr;
		}

		math::float3 PhysxRigidbody::getPosition()
		{
			auto pxGlobalPose = px_rigidbody->getGlobalPose();
			return math::float3(pxGlobalPose.p.x, pxGlobalPose.p.y, pxGlobalPose.p.z);
		}
		void PhysxRigidbody::setPosition(math::float3 postion)
		{
			auto pxGlobalPose = px_rigidbody->getGlobalPose();
			pxGlobalPose.p = physx::PxVec3(postion.x, postion.y, postion.z);
			px_rigidbody->setGlobalPose(pxGlobalPose);
		}
		math::Quaternion PhysxRigidbody::getRotation()
		{
			auto pxGlobalPose = px_rigidbody->getGlobalPose();
			return math::Quaternion(pxGlobalPose.q.x, pxGlobalPose.q.y, pxGlobalPose.q.z, pxGlobalPose.q.w);
		}
		void PhysxRigidbody::setRotation(math::Quaternion rotation)
		{
			auto pxGlobalPose = px_rigidbody->getGlobalPose();
			pxGlobalPose.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);
			px_rigidbody->setGlobalPose(pxGlobalPose);
		}

		void PhysxRigidbody::setPositionAndRotation(math::float3 postion, math::Quaternion rotation)
		{
			auto pxGlobalPose = px_rigidbody->getGlobalPose();
			pxGlobalPose.p = physx::PxVec3(postion.x, postion.y, postion.z);
			pxGlobalPose.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);
			px_rigidbody->setGlobalPose(pxGlobalPose);
		}

		void PhysxRigidbody::addShape(std::shared_ptr<PhysicsShape> shapeAdded)
		{
			this->shape = std::dynamic_pointer_cast<PhysxShape>(shapeAdded);
		}

		physx::PxRigidActor * PhysxRigidbody::getPxRigidbody()
		{
			return px_rigidbody;
		}
	}
}


