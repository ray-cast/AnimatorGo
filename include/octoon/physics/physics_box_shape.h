#ifndef OCTOON_PHYSICS_BOX_SHAPE_H_
#define OCTOON_PHYSICS_BOX_SHAPE_H_

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_shape.h>

namespace octoon
{
	namespace physics
	{
		struct OCTOON_EXPORT PhysicsBoxShapeDesc
		{
		public:
			float width;
			float height;
			float depth;
		public:
			PhysicsBoxShapeDesc()
				: width(1.f)
				, height(1.f)
				, depth(1.f)
			{
			}
		};

		class OCTOON_EXPORT PhysicsBoxShape : public PhysicsShape
		{
		public:
			PhysicsBoxShape() noexcept;
			virtual ~PhysicsBoxShape();

			virtual void setWidth(float width) = 0;
			virtual void setHeight(float height) = 0;
			virtual void setDepth(float depth) = 0;

			virtual float getWidth() const = 0;
			virtual float getHeight() const = 0;
			virtual float getDepth() const = 0;

		private:
			PhysicsBoxShape(const PhysicsBoxShape&) noexcept = delete;
			PhysicsBoxShape& operator=(const PhysicsBoxShape&) noexcept = delete;
		};
	}
}

#endif