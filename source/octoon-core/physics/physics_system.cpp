#include <octoon/physics/physics_system.h>
#include <octoon/runtime/except.h>

#include "physx/physx_context.h"
#include "Bullet/bullet_context.h"

namespace octoon
{
	OctoonImplementSingleton(PhysicsSystem)

	PhysicsSystem::PhysicsSystem() noexcept
	{
	}

	PhysicsSystem::~PhysicsSystem() noexcept
	{
	}

	std::shared_ptr<PhysicsContext> PhysicsSystem::createContext(PhysicsDevice device)
	{
		if (device == PhysicsDevice::Physx)
			return std::make_shared<PhysxContext>();
		else if (device == PhysicsDevice::Bullet)
			return std::make_shared<BulletContext>();
		else
			throw runtime::not_implemented::create("Not implemented yet");
	}
}