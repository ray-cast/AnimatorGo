#include "material_module.h"

namespace rabbit
{
	MaterialModule::MaterialModule() noexcept
	{
		this->reset();
	}

	MaterialModule::~MaterialModule() noexcept
	{
	}

	void
	MaterialModule::reset() noexcept
	{
	}

	void 
	MaterialModule::load(octoon::runtime::json& reader) noexcept
	{
	}

	void 
	MaterialModule::save(octoon::runtime::json& writer) noexcept
	{
	}
}