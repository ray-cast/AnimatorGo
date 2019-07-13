#ifndef OCTOON_PHYSICS_RIGIDBODY_H_
#define OCTOON_PHYSICS_RIGIDBODY_H_

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_scene.h>

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
			math::float3 translate;
			math::Quaternion rotation;
			PhysicsRigidbodyType type;
			float mass;
		};

		class OCTOON_EXPORT PhysicsRigidbody
		{
		public:
			PhysicsRigidbody() noexcept;
			virtual ~PhysicsRigidbody();

		private:
			PhysicsRigidbody(const PhysicsRigidbody&) noexcept = delete;
			PhysicsRigidbody& operator=(const PhysicsRigidbody&) noexcept = delete;
		};
	}
}

#endif