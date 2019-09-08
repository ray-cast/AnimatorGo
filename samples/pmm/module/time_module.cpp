#include "time_module.h"

namespace MysticLit
{
	TimeModule::TimeModule() noexcept
	{
		this->reset();
	}

	TimeModule::~TimeModule() noexcept
	{
	}

	void
	TimeModule::reset() noexcept
	{
		this->recordFps = 24.0f;
		this->recordTimeStep = 10.0f;

		this->playFps = 60.0f;
		this->playTimeStep = 1.0f / 60.0f;
	}

	void 
	TimeModule::load(octoon::runtime::json& reader) noexcept
	{
		this->recordFps = reader["recordFps"];
		this->recordTimeStep = reader["recordTimeStep"];
		this->playFps = reader["playFps"];
		this->playTimeStep = reader["playTimeStep"];
	}

	void 
	TimeModule::save(octoon::runtime::json& writer) noexcept
	{
		writer["recordFps"] = this->recordFps;
		writer["recordTimeStep"] = this->recordTimeStep;
		writer["playFps"] = this->playFps;
		writer["playTimeStep"] = this->playTimeStep;
	}
}