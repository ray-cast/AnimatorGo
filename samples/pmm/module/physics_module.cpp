#include "physics_module.h"

namespace MysticLit
{
	PhysicsModule::PhysicsModule() noexcept
	{
		this->reset();
	}

	PhysicsModule::~PhysicsModule() noexcept
	{
	}

	void
	PhysicsModule::reset() noexcept
	{
		this->solverIterationCounts_ = 10;
	}
}