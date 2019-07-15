#ifndef OCTOON_PHYSICS_RIGIDBODY_H_
#define OCTOON_PHYSICS_RIGIDBODY_H_

#include <memory>

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_shape.h>


#include "physics_type.h"

namespace octoon
{
	namespace physics
	{
		enum class PhysicsRigidbodyType
		{
			Static,
			Dynamic
		};

		struct OCTOON_EXPORT PhysicsRigidbodyDesc
		{
		public:
			math::float3 translate;
			math::Quaternion rotation;
			PhysicsRigidbodyType type;
			float mass;
		public:
			PhysicsRigidbodyDesc()
				:translate(0.f, 0.f, 0.f),
				rotation(0.f, 0.f, 0.f, 0.f),
				type(PhysicsRigidbodyType::Dynamic),
				mass(0.f)
			{
			}
		};

		class OCTOON_EXPORT PhysicsRigidbody
		{
		public:
			PhysicsRigidbody() noexcept;
			virtual ~PhysicsRigidbody();

			virtual math::float3 getPosition() = 0;
			virtual void setPosition(math::float3 postion) = 0;
			virtual math::Quaternion getRotation() = 0;
			virtual void setRotation(math::Quaternion rotation) = 0;
			virtual void setPositionAndRotation(math::float3 postion, math::Quaternion rotation) = 0;

			virtual void addShape(std::shared_ptr<PhysicsShape> shape) = 0;

		private:
			PhysicsRigidbody(const PhysicsRigidbody&) noexcept = delete;
			PhysicsRigidbody& operator=(const PhysicsRigidbody&) noexcept = delete;
		};
	}
}

#endif