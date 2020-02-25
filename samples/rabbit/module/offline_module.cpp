#include "offline_module.h"

namespace rabbit
{
	OfflineModule::OfflineModule() noexcept
	{
		this->reset();
	}

	OfflineModule::~OfflineModule() noexcept
	{
	}

	void
	OfflineModule::reset() noexcept
	{
		this->offlineEnable = false;
	}

	void 
	OfflineModule::load(octoon::runtime::json& reader) noexcept
	{
	}

	void 
	OfflineModule::save(octoon::runtime::json& writer) noexcept
	{
	}
}