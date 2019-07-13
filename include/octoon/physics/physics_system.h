#ifndef OCTOON_PHYSICS_SYSTEM_H_
#define OCTOON_PHYSICS_SYSTEM_H_

#include <memory>

#include <octoon/runtime/singleton.h>

#include <octoon/physics/physics_context.h>

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysicsSystem final
		{
			OctoonDeclareSingleton(PhysicsSystem)
		public:
			PhysicsSystem() noexcept;
			~PhysicsSystem() noexcept;

			std::shared_ptr<PhysicsContext> createContext();

		private:
			PhysicsSystem(const PhysicsSystem&) = delete;
			PhysicsSystem& operator=(const PhysicsSystem&) = delete;
		};
	}
}

#endif