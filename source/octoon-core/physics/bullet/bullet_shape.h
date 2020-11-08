#ifndef OCTOON_BULLET_SHAPE_H_
#define OCTOON_BULLET_SHAPE_H_

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_shape.h>

#include "bullet_type.h"

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT BulletShape : public PhysicsShape
		{
		public:
			BulletShape() noexcept;
			virtual ~BulletShape() noexcept;

			virtual btCollisionShape* getPxShape() const = 0;

		private:
			BulletShape(const BulletShape&) noexcept = delete;
			BulletShape& operator=(const BulletShape&) noexcept = delete;
		};
	}
}

#endif