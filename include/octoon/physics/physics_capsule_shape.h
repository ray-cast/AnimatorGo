#ifndef OCTOON_PHYSICS_CAPSULE_SHAPE_H_
#define OCTOON_PHYSICS_CAPSULE_SHAPE_H_

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_shape.h>

namespace octoon
{
	struct OCTOON_EXPORT PhysicsCapsuleShapeDesc
	{
	public:
		float radius;
		float height;
	public:
		PhysicsCapsuleShapeDesc()
			: radius(1.f)
			, height(1.f)
		{
		}
	};

	class OCTOON_EXPORT PhysicsCapsuleShape : public PhysicsShape
	{
	public:
		PhysicsCapsuleShape() noexcept;
		virtual ~PhysicsCapsuleShape();

		virtual void setRadius(float width) = 0;
		virtual void setHeight(float height) = 0;

		virtual float getRadius() const = 0;
		virtual float getHeight() const = 0;

	private:
		PhysicsCapsuleShape(const PhysicsCapsuleShape&) noexcept = delete;
		PhysicsCapsuleShape& operator=(const PhysicsCapsuleShape&) noexcept = delete;
	};
}

#endif