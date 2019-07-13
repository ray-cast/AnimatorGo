#ifndef OCTOON_PHYSX_RIGIDBODY_H_
#define OCTOON_PHYSX_RIGIDBODY_H_

#include <memory>

#include <octoon/runtime/platform.h>

#include <octoon/physics/physics_rigidbody.h>

#include "physx_type.h"

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysxRigidbody : public PhysicsRigidbody
		{
		public:
			PhysxRigidbody(PhysxContext* context, PhysicsRigidbodyDesc desc);
			virtual ~PhysxRigidbody();

			physx::PxRigidActor* getPxRigidbody();

		private:
			PhysxRigidbody(const PhysxRigidbody&) noexcept = delete;
			PhysxRigidbody& operator=(const PhysxRigidbody&) noexcept = delete;
		private:
			PhysxContext* context;
			physx::PxRigidActor* px_rigidbody;
		};
	}
}

#endif