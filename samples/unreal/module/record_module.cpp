#include "record_module.h"

namespace unreal
{
	RecordModule::RecordModule() noexcept
	{
		this->reset();
	}

	RecordModule::~RecordModule() noexcept
	{
	}

	void
	RecordModule::reset() noexcept
	{
		this->hdr = false;
		this->srgb = true;
		this->active = false;
		this->denoise = true;
	}

	void 
	RecordModule::load(nlohmann::json& reader) noexcept
	{
		if (reader["denoise"].is_boolean())
			this->denoise = reader["denoise"].get<nlohmann::json::boolean_t>();
	}

	void 
	RecordModule::save(nlohmann::json& writer) noexcept
	{
		writer["denoise"] = this->denoise.getValue();
	}

	void
	RecordModule::disconnect() noexcept
	{
		this->enable.disconnect();
		this->hdr.disconnect();
		this->srgb.disconnect();
		this->active.disconnect();
		this->denoise.disconnect();
	}
}