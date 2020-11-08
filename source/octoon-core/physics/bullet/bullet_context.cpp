#include "bullet_context.h"
#include "bullet_rigidbody.h"
#include "bullet_shape.h"
#include "bullet_box_shape.h"
#include "bullet_sphere_shape.h"
#include "bullet_capsule_shape.h"
#include "bullet_fixed_joint.h"
#include "bullet_configurable_joint.h"

namespace octoon
{
	BulletContext::BulletContext()
	{
	}

	BulletContext::~BulletContext()
	{
	}

	std::shared_ptr<PhysicsScene>
	BulletContext::createScene(PhysicsSceneDesc desc)
	{
		return std::make_shared<BulletScene>(desc);
	}

	std::shared_ptr<PhysicsRigidbody>
	BulletContext::createRigidbody(PhysicsRigidbodyDesc desc)
	{
		return std::make_shared<BulletRigidbody>(desc);
	}

	std::shared_ptr<PhysicsBoxShape>
	BulletContext::createBoxShape(PhysicsBoxShapeDesc desc)
	{
		return std::make_shared<BulletBoxShape>(desc.width, desc.height, desc.depth);
	}

	std::shared_ptr<PhysicsSphereShape>
	BulletContext::createSphereShape(PhysicsSphereShapeDesc desc)
	{
		return std::make_shared<BulletSphereShape>(desc.radius);
	}

	std::shared_ptr<PhysicsCapsuleShape>
	BulletContext::createCapsuleShape(PhysicsCapsuleShapeDesc desc)
	{
		return std::make_shared<BulletCapsuleShape>(desc.radius, desc.height);
	}

	std::shared_ptr<PhysicsFixedJoint>
	BulletContext::createFixedJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
	{
		auto ret = std::make_shared<BulletFixedJoint>();
		ret->connect(lhs, rhs);
		return ret;
	}

	std::shared_ptr<PhysicsConfigurableJoint>
	BulletContext::createConfigurableJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
	{
		auto ret = std::make_shared<BulletConfigurableJoint>();
		ret->connect(lhs, rhs);
		return ret;
	}
}