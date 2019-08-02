#ifndef OCTOON_PHYSICS_CONTEXT_H_
#define OCTOON_PHYSICS_CONTEXT_H_

#include <octoon/runtime/platform.h>

#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_rigidbody.h>
#include <octoon/physics/physics_box_shape.h>
#include <octoon/physics/physics_capsule_shape.h>
#include <octoon/physics/physics_type.h>

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysicsContextDesc final
		{
		public:
			PhysicsContextDesc() noexcept;
			~PhysicsContextDesc() noexcept;
		};

		class OCTOON_EXPORT PhysicsContext
		{
		public:
			PhysicsContext() noexcept = default;
			virtual ~PhysicsContext() = default;

			virtual std::shared_ptr<PhysicsScene> createScene(PhysicsSceneDesc desc) = 0;
			virtual std::shared_ptr<PhysicsRigidbody> createRigidbody(PhysicsRigidbodyDesc desc) = 0;
			virtual std::shared_ptr<PhysicsSphereShape> createSphereShape(PhysicsSphereShapeDesc desc) = 0;
			virtual std::shared_ptr<PhysicsBoxShape> createBoxShape(PhysicsBoxShapeDesc desc) = 0;
			virtual std::shared_ptr<PhysicsCapsuleShape> createCapsuleShape(PhysicsCapsuleShapeDesc desc) = 0;
			virtual std::shared_ptr<PhysicsFixedJoint> createFixedJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) = 0;
			virtual std::shared_ptr<PhysicsConfigurableJoint> createConfigurableJoint(std::shared_ptr<PhysicsRigidbody> lhs, std::shared_ptr<PhysicsRigidbody> rhs) = 0;

		private:
			PhysicsContext(const PhysicsContext&) noexcept = delete;
			PhysicsContext& operator=(const PhysicsContext&) noexcept = delete;
		};
	}
}

#endif