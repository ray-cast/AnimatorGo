#ifndef OCTOON_PHYSICS_SPHERE_SHAPE_H_
#define OCTOON_PHYSICS_SPHERE_SHAPE_H_

#include <octoon/runtime/platform.h>
#include <octoon/math/math.h>

#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_shape.h>

namespace octoon
{
	namespace physics
	{
		struct OCTOON_EXPORT PhysicsSphereShapeDesc
		{
		public:
			float radius;
		public:
			PhysicsSphereShapeDesc()
				:radius(1.f)
			{
			}
		};

		class OCTOON_EXPORT PhysicsSphereShape: public PhysicsShape
		{
		public:
			PhysicsSphereShape() noexcept;
			virtual ~PhysicsSphereShape();

			virtual math::float3 getCenter() = 0;
			virtual void setCenter(math::float3 center) = 0;
			virtual float getRadius() = 0;
			virtual void setRadius(float radius) = 0;

		private:
			PhysicsSphereShape(const PhysicsSphereShape&) noexcept = delete;
			PhysicsSphereShape& operator=(const PhysicsSphereShape&) noexcept = delete;
		};
	}
}

#endif