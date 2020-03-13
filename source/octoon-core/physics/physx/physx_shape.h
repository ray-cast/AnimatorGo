#ifndef OCTOON_PHYSX_SHAPE_H_
#define OCTOON_PHYSX_SHAPE_H_

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_shape.h>

#include "physx_type.h"

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysxShape: public PhysicsShape
		{
		public:
			PhysxShape() noexcept;
			virtual ~PhysxShape() noexcept;

			virtual physx::PxShape* getPxShape() = 0;
			virtual physx::PxMaterial* getPxMaterial() = 0;

		private:
			PhysxShape(const PhysxShape&) noexcept = delete;
			PhysxShape& operator=(const PhysxShape&) noexcept = delete;
		};
	}
}

#endif