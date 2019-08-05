#ifndef OCTOON_PHYSICS_RIGIDBODY_H_
#define OCTOON_PHYSICS_RIGIDBODY_H_

#include <memory>

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_shape.h>
#include <octoon/physics/physics_listener.h>

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
				rotation(1.f, 0.f, 0.f, 0.f),
				type(PhysicsRigidbodyType::Dynamic),
				mass(1.f)
			{
			}
		};

		class OCTOON_EXPORT PhysicsRigidbody
		{
		public:
			PhysicsRigidbody() noexcept;
			virtual ~PhysicsRigidbody();

			virtual void setGroup(std::uint8_t group) = 0;
			virtual void setGroupMask(std::uint16_t groupMask) = 0;

			virtual void setOwnerListener(PhysicsListener* listener) = 0;

			virtual void setKinematic(bool kinematic) noexcept = 0;
			virtual void setSleepThreshold(float threshold) noexcept = 0;

			virtual void setPosition(math::float3 postion) = 0;
			virtual void setRotation(math::Quaternion rotation) = 0;
			virtual void setPositionAndRotation(math::float3 postion, math::Quaternion rotation) = 0;

			virtual void setMass(float f) = 0;
			virtual void setDynamicFriction(float f) = 0;
			virtual void setStaticFriction(float f) = 0;
			virtual void setRestitution(float f) = 0;

			virtual void setLinearDamping(float value) = 0;
			virtual void setAngularDamping(float value) = 0;

			virtual math::float3 getPosition() = 0;
			virtual math::Quaternion getRotation() = 0;

			virtual float getDynamicFriction() const = 0;
			virtual float getStaticFriction() const = 0;
			virtual float getRestitution() const = 0;

			virtual void attachShape(std::shared_ptr<PhysicsShape> shape) = 0;
			virtual void detachShape(std::shared_ptr<PhysicsShape> shape) = 0;

		private:
			PhysicsRigidbody(const PhysicsRigidbody&) noexcept = delete;
			PhysicsRigidbody& operator=(const PhysicsRigidbody&) noexcept = delete;
		};
	}
}

#endif