#include "mark_module.h"

namespace MysticLit
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

		markEnable = false;
	}

	void 
	MarkModule::load(octoon::runtime::json& reader) noexcept
	{
	}

	void 
	MarkModule::save(octoon::runtime::json& writer) noexcept
	{
	}
}