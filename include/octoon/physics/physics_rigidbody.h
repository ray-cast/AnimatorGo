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
	enum class PhysicsRigidbodyType
	{
		Static,
		Dynamic
	};

	struct OCTOON_EXPORT PhysicsRigidbodyDesc
	{
	public:
		math::float3 position;
		math::Quaternion rotation;
		PhysicsRigidbodyType type;
		float mass;
	public:
		PhysicsRigidbodyDesc()
			: position(math::float3::Zero)
			, rotation(math::Quaternion::Zero)
			, type(PhysicsRigidbodyType::Dynamic)
			, mass(1.f)
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

		virtual void setEnableCCD(bool enable) = 0;

		virtual void setOwnerListener(PhysicsListener* listener) = 0;

		virtual void setKinematic(bool kinematic) noexcept = 0;
		virtual void setSleepThreshold(float threshold) noexcept = 0;
		virtual void setSolverIterationCounts(std::uint32_t minPositionIters, std::uint32_t minVelocityIters = 1) noexcept = 0;

		virtual void setPosition(const math::float3& postion) = 0;
		virtual void setRotation(const math::Quaternion& rotation) = 0;
		virtual void setPositionAndRotation(const math::float3& postion, const math::Quaternion& rotation) = 0;

		virtual void setMass(float f) = 0;
		virtual void setDynamicFriction(float f) = 0;
		virtual void setStaticFriction(float f) = 0;
		virtual void setRestitution(float f) = 0;

		virtual void setLinearDamping(float value) = 0;
		virtual void setAngularDamping(float value) = 0;

		virtual void setLinearVelocity(const math::float3& value) = 0;
		virtual void setAngularVelocity(const math::float3& value) = 0;

		virtual math::float3 getPosition() = 0;
		virtual math::Quaternion getRotation() = 0;

		virtual float getMass() const = 0;
		virtual float getDynamicFriction() const = 0;
		virtual float getStaticFriction() const = 0;
		virtual float getRestitution() const = 0;

		virtual void wakeUp() = 0;
		virtual void clearForce() noexcept = 0;
		virtual void clearTorque() noexcept = 0;

		virtual void attachShape(std::shared_ptr<PhysicsShape> shape) = 0;
		virtual void detachShape(std::shared_ptr<PhysicsShape> shape) = 0;

	private:
		PhysicsRigidbody(const PhysicsRigidbody&) noexcept = delete;
		PhysicsRigidbody& operator=(const PhysicsRigidbody&) noexcept = delete;
	};
}

#endif