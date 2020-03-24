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
		this->path = "../../system/materials";
	}

	void 
	MaterialModule::load(octoon::runtime::json& reader) noexcept
	{
		if (reader.find("path") != reader.end())
			this->path = reader["path"].get<nlohmann::json::string_t>();
	}

	void 
	MaterialModule::save(octoon::runtime::json& writer) noexcept
	{
		writer["path"] = this->path;
	}
}