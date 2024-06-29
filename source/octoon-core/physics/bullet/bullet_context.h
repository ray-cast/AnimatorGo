#ifndef OCTOON_BULLET_CONTEXT_H_
#define OCTOON_BULLET_CONTEXT_H_

#include <memory>

#include <octoon/runtime/platform.h>

#include <octoon/physics/physics_context.h>
#include <octoon/physics/physics_shape.h>
#include <octoon/physics/physics_sphere_shape.h>
#include <octoon/physics/physics_joint.h>
#include <octoon/physics/physics_fixed_joint.h>
#include <octoon/physics/physics_configurable_joint.h>

#include "bullet_scene.h"
#include "bullet_type.h"

namespace octoon
{
	class OCTOON_EXPORT BulletContext : public PhysicsContext
	{
	public:
		BulletContext();
		virtual ~BulletContext();

		virtual std::unique_ptr<PhysicsScene> createScene(const PhysicsSceneDesc& desc) override;
		virtual std::unique_ptr<PhysicsRigidbody> createRigidbody(const PhysicsRigidbodyDesc& desc) override;
		virtual std::unique_ptr<PhysicsSphereShape> createSphereShape(const PhysicsSphereShapeDesc& desc) override;
		virtual std::unique_ptr<PhysicsBoxShape> createBoxShape(const PhysicsBoxShapeDesc& desc) override;
		virtual std::unique_ptr<PhysicsCapsuleShape> createCapsuleShape(const PhysicsCapsuleShapeDesc& desc) override;
		virtual std::unique_ptr<PhysicsFixedJoint> createFixedJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) override;
		virtual std::unique_ptr<PhysicsConfigurableJoint> createConfigurableJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) override;

	private:
		BulletContext(const BulletContext&) noexcept = delete;
		BulletContext& operator=(const BulletContext&) noexcept = delete;
	};
}

#endif