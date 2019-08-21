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
#if defined(_DEBUG)
		this->playSolverIterationCounts = 3;
#else
		this->playSolverIterationCounts = 10;
#endif
		this->recordSolverIterationCounts = 10;
		this->previewSolverIterationCounts = 1;
	}
}