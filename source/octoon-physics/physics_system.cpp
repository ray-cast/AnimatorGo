#include <octoon/physics/physics_system.h>

namespace octoon
{
	namespace physics
	{
		OctoonImplementSingleton(PhysicsSystem)

		PhysicsSystem::PhysicsSystem() noexcept
		{
		}

		PhysicsSystem::~PhysicsSystem() noexcept
		{
			this->close();
		}

		void
		PhysicsSystem::close() noexcept
		{
		}
	}
}