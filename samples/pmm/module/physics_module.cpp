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
		this->playSolverIterationCounts = 10;
		this->recordSolverIterationCounts = 10;
#if defined(_DEBUG)
		this->previewSolverIterationCounts = 0;
#else
		this->previewSolverIterationCounts = 1;
#endif
	}
}