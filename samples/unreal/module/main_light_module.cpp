#include "main_light_module.h"

namespace unreal
{
	MainLightModule::MainLightModule() noexcept
	{
		this->reset();
	}

	MainLightModule::~MainLightModule() noexcept
	{
	}

	void
	MainLightModule::reset() noexcept
	{
		this->enable = true;
		this->size = 0.1f;
		this->intensity = 1.0f;
		this->color = octoon::math::float3(0.90196078f, 0.90196078f, 0.925490196f);
		this->rotation = octoon::math::float3::Zero;
	}

	void 
	MainLightModule::load(nlohmann::json& reader) noexcept
	{
		if (reader["enable"].is_boolean())
			this->enable = reader["enable"].get<nlohmann::json::boolean_t>();
		if (reader["size"].is_number_float())
			this->size = reader["size"].get<nlohmann::json::number_float_t>();
		if (reader["intensity"].is_number_float())
			this->intensity = reader["intensity"].get<nlohmann::json::number_float_t>();
		if (reader["color"].is_array())
			this->color = octoon::math::float3(reader["color"].get<std::array<float, 3>>());
		if (reader["rotation"].is_array())
			this->rotation = octoon::math::float3(reader["rotation"].get<std::array<float, 3>>());
	}

	void 
	MainLightModule::save(nlohmann::json& writer) noexcept
	{
		writer["enable"] = this->enable.getValue();
		writer["intensity"] = this->intensity.getValue();
		writer["size"] = this->size.getValue();
		writer["color"] = this->color.getValue().to_array();
		writer["rotation"] = this->rotation.getValue().to_array();
	}

	void
	MainLightModule::disconnect() noexcept
	{
		this->enable.disconnect();
		this->size.disconnect();
		this->intensity.disconnect();
		this->color.disconnect();
		this->rotation.disconnect();
	}
}