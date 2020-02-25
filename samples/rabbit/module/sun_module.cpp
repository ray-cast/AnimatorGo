#include "sun_module.h"

namespace rabbit
{
	SunModule::SunModule() noexcept
	{
		this->reset();
	}

	SunModule::~SunModule() noexcept
	{
	}

	void
	SunModule::reset() noexcept
	{
		this->enable = false;
		this->intensity = 2.0f;
		this->color = octoon::math::float3(0.90196078f, 0.90196078f, 0.925490196f);
	}

	void 
	SunModule::load(octoon::runtime::json& reader) noexcept
	{
		if (reader.find("enable") != reader.end())
			this->enable = reader["enable"];

		if (reader.find("intensity") != reader.end())
			this->intensity = reader["intensity"];

		if (reader.count("color"))
		{
			this->color.x = reader["color"][0];
			this->color.y = reader["color"][1];
			this->color.z = reader["color"][2];
		}

		if (reader.count("rotation"))
		{
			this->rotation.x = reader["rotation"][0];
			this->rotation.y = reader["rotation"][1];
			this->rotation.z = reader["rotation"][2];
		}
	}

	void 
	SunModule::save(octoon::runtime::json& writer) noexcept
	{
		writer["enable"] = this->enable;
		writer["intensity"] = this->intensity;
		writer["color"].push_back(this->color.x);
		writer["color"].push_back(this->color.y);
		writer["color"].push_back(this->color.z);
		writer["rotation"].push_back(this->rotation.x);
		writer["rotation"].push_back(this->rotation.y);
		writer["rotation"].push_back(this->rotation.z);
	}
}