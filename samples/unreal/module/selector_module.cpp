#include "selector_module.h"

namespace unreal
{
	SelectorModule::SelectorModule() noexcept
	{
		this->reset();
	}

	SelectorModule::~SelectorModule() noexcept
	{
	}

	void
	SelectorModule::reset() noexcept
	{
		this->selectedItem_ = std::nullopt;
		this->selectedItemHover_ = std::nullopt;
	}

	void 
	SelectorModule::load(nlohmann::json& reader) noexcept
	{
	}

	void 
	SelectorModule::save(nlohmann::json& writer) noexcept
	{
	}

	void
	SelectorModule::disconnect() noexcept
	{
		this->enable.disconnect();
		this->selectedItem_.disconnect();
		this->selectedItemHover_.disconnect();
	}
}