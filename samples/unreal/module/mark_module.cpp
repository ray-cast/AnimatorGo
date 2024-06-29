#include "mark_module.h"

namespace unreal
{
	MarkModule::MarkModule() noexcept
	{
		this->reset();
	}

	MarkModule::~MarkModule() noexcept
	{
	}

	void
	MarkModule::reset() noexcept
	{
		x = 24;
		y = 24;
		width = 0;
		height = 0;
		channel = 8;
		this->setEnable(false);
	}

	void 
	MarkModule::load(nlohmann::json& reader) noexcept
	{
	}

	void 
	MarkModule::save(nlohmann::json& writer) noexcept
	{
	}
}