#ifndef OCTOON_PHYSICS_SHAPE_H_
#define OCTOON_PHYSICS_SHAPE_H_

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_scene.h>

namespace octoon
{
	class OCTOON_EXPORT PhysicsShape
	{
	public:
		PhysicsShape() noexcept;
		virtual ~PhysicsShape() noexcept;

		virtual math::float3 getCenter() = 0;
		virtual void setCenter(const math::float3& center) = 0;

		virtual math::Quaternion getQuaternion() = 0;
		virtual void setQuaternion(const math::Quaternion& center) = 0;

		virtual void setContactOffset(float offset) = 0;
		virtual float getContactOffset() const = 0;

		virtual void setRestOffset(float offset) = 0;
		virtual float getRestOffset() const = 0;

	private:
		PhysicsShape(const PhysicsShape&) noexcept = delete;
		PhysicsShape& operator=(const PhysicsShape&) noexcept = delete;
	};
}

#endif