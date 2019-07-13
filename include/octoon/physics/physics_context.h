#ifndef OCTOON_PHYSICS_CONTEXT_H_
#define OCTOON_PHYSICS_CONTEXT_H_

#include <octoon/runtime/platform.h>

#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_rigidbody.h>

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysicsContextDesc final
		{
		public:
			PhysicsContextDesc() noexcept;
			~PhysicsContextDesc() noexcept;
		};

		class OCTOON_EXPORT PhysicsContext
		{
		public:
			PhysicsContext() noexcept = default;
			virtual ~PhysicsContext() = default;

			virtual std::shared_ptr<PhysicsScene> createScene(PhysicsSceneDesc desc) = 0;
			virtual std::shared_ptr<PhysicsRigidbody> createRigidbody() = 0;

		private:
			PhysicsContext(const PhysicsContext&) noexcept = delete;
			PhysicsContext& operator=(const PhysicsContext&) noexcept = delete;
		};
	}
}

#endif