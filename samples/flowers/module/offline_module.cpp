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
		this->bounces = 3;
	}

	void 
	OfflineModule::load(octoon::runtime::json& reader) noexcept
	{
		if (reader.find("bounces") != reader.end())
			this->bounces = reader["bounces"];
	}

	void 
	OfflineModule::save(octoon::runtime::json& writer) noexcept
	{
		writer["bounces"] = this->bounces;
	}
}