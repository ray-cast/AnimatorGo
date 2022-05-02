#include "environment_module.h"

namespace rabbit
{
	EnvironmentModule::EnvironmentModule() noexcept
	{
		this->reset();
	}

	EnvironmentModule::~EnvironmentModule() noexcept
	{
	}

	void
	EnvironmentModule::reset() noexcept
	{
		this->enable = false;
		this->intensity = 1.0f;
		this->offset = octoon::math::float2(0.f, 0.f);
		this->color = octoon::math::float3(0.90196078f, 0.90196078f, 0.925490196f);
	}

	void 
	EnvironmentModule::load(octoon::runtime::json& reader) noexcept
	{
		if (reader.find("enable") != reader.end())
			this->enable = reader["enable"];
	}

	void 
	EnvironmentModule::save(octoon::runtime::json& writer) noexcept
	{
		writer["enable"] = this->enable;
	}
}