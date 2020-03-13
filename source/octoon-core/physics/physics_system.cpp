#include <octoon/physics/physics_system.h>

#include "physx/physx_context.h"

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
		}

		std::shared_ptr<PhysicsContext> PhysicsSystem::createContext()
		{
			return std::make_shared<PhysxContext>();
		}
	}
}