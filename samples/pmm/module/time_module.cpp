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
		this->recordFps = 30.0f;
		this->recordTimeStep = 10.0f;

		this->playFps = 60.0f;
		this->playTimeStep = 1.0f / 60.0f;
	}
}