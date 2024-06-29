#include "physics_module.h"

namespace unreal
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
		this->gravity = octoon::math::float3(0.0, -9.8f, 0.0f);
		this->gravityScale = 5.0f;
		this->fixedTimeStep = 1.0f / 60.f;
		this->playSolverIterationCounts = std::numeric_limits<int>::max();
		this->previewSolverIterationCounts = std::numeric_limits<int>::max();
		this->recordSolverIterationCounts = std::numeric_limits<int>::max();
	}

	void 
	PhysicsModule::load(nlohmann::json& reader) noexcept
	{
		if (reader["enable"].is_boolean())
			this->enable = reader["enable"].get<nlohmann::json::boolean_t>();
		if (reader["gravity"].is_array())
			this->gravity = octoon::math::float3(reader["gravity"].get<std::array<float, 3>>());
		if (reader["gravityScale"].is_number_float())
			this->gravityScale = reader["gravityScale"].get<nlohmann::json::number_float_t>();
		if (reader["playSolverIterationCounts"].is_number_unsigned())
			this->playSolverIterationCounts = reader["playSolverIterationCounts"].get<nlohmann::json::number_unsigned_t>();
		if (reader["recordSolverIterationCounts"].is_number_unsigned())
			this->recordSolverIterationCounts = reader["recordSolverIterationCounts"].get<nlohmann::json::number_unsigned_t>();
		if (reader["previewSolverIterationCounts"].is_number_unsigned())
			this->previewSolverIterationCounts = reader["previewSolverIterationCounts"].get<nlohmann::json::number_unsigned_t>();
	}

	void 
	PhysicsModule::save(nlohmann::json& writer) noexcept
	{
		writer["enable"] = this->enable.getValue();
		writer["gravity"] = this->gravity.getValue().to_array();
		writer["gravityScale"] = this->gravityScale.getValue();
		writer["playSolverIterationCounts"] = this->playSolverIterationCounts.getValue();
		writer["recordSolverIterationCounts"] = this->recordSolverIterationCounts.getValue();
		writer["previewSolverIterationCounts"] = this->previewSolverIterationCounts.getValue();
	}

	void
	PhysicsModule::disconnect() noexcept
	{
		this->enable.disconnect();
		this->gravity.disconnect();
		this->gravityScale.disconnect();
		this->fixedTimeStep.disconnect();
		this->playSolverIterationCounts.disconnect();
		this->previewSolverIterationCounts.disconnect();
		this->recordSolverIterationCounts.disconnect();
	}
}