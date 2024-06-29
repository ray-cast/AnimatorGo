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

	std::unique_ptr<PhysicsScene>
	BulletContext::createScene(const PhysicsSceneDesc& desc)
	{
		return std::make_unique<BulletScene>(desc);
	}

	std::unique_ptr<PhysicsRigidbody>
	BulletContext::createRigidbody(const PhysicsRigidbodyDesc& desc)
	{
		return std::make_unique<BulletRigidbody>(desc);
	}

	std::unique_ptr<PhysicsBoxShape>
	BulletContext::createBoxShape(const PhysicsBoxShapeDesc& desc)
	{
		return std::make_unique<BulletBoxShape>(desc.width, desc.height, desc.depth);
	}

	std::unique_ptr<PhysicsSphereShape>
	BulletContext::createSphereShape(const PhysicsSphereShapeDesc& desc)
	{
		return std::make_unique<BulletSphereShape>(desc.radius);
	}

	std::unique_ptr<PhysicsCapsuleShape>
	BulletContext::createCapsuleShape(const PhysicsCapsuleShapeDesc& desc)
	{
		return std::make_unique<BulletCapsuleShape>(desc.radius, desc.height);
	}

	std::unique_ptr<PhysicsFixedJoint>
	BulletContext::createFixedJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
	{
		auto ret = std::make_unique<BulletFixedJoint>();
		ret->connect(lhs, rhs);
		return ret;
	}

	std::unique_ptr<PhysicsConfigurableJoint>
	BulletContext::createConfigurableJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs)
	{
		auto ret = std::make_unique<BulletConfigurableJoint>();
		ret->connect(lhs, rhs);
		return ret;
	}
}