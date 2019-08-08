#ifndef OCTOON_PHYSX_CLOTH_H_
#define OCTOON_PHYSX_CLOTH_H_

#include <memory>
#include <octoon/runtime/platform.h>
#include <octoon/physics/physics_rigidbody.h>

#include "physx_type.h"

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysxCloth
		{
		public:
			PhysxCloth(PhysxContext* context, PhysicsRigidbodyDesc desc);
			virtual ~PhysxCloth();

		private:
			PhysxCloth(const PhysxCloth&) noexcept = delete;
			PhysxCloth& operator=(const PhysxCloth&) noexcept = delete;

		private:
			PhysxContext* context;
			physx::PxRigidDynamic* px_rigidbody;

			std::shared_ptr<PhysxShape> shape_;
		};
	}
}

#endif