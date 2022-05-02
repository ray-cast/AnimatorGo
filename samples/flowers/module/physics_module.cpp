#include "physics_module.h"

namespace flower
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
		this->gravity = octoon::math::float3(0.0, -9.8f * 10.f, 0.0f);
		this->playSolverIterationCounts = 10;
		this->recordSolverIterationCounts = 10;
		this->previewSolverIterationCounts = 1;
	}

	void 
	PhysicsModule::load(octoon::runtime::json& reader) noexcept
	{
		this->playSolverIterationCounts = reader["playSolverIterationCounts"];
		this->recordSolverIterationCounts = reader["recordSolverIterationCounts"];
	}

	void 
	PhysicsModule::save(octoon::runtime::json& reader) noexcept
	{
		reader["playSolverIterationCounts"] = this->playSolverIterationCounts;
		reader["recordSolverIterationCounts"] = this->recordSolverIterationCounts;
	}
}