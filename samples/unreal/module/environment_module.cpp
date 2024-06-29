#include "environment_module.h"
#include <octoon/asset_database.h>

namespace unreal
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
		this->intensity = 1.0f;
		this->showBackground = true;
		this->useTexture = true;
		this->texture = nullptr;
		this->offset = octoon::math::float2(0.f, 0.f);
		this->color = octoon::math::float3(0.90196078f, 0.90196078f, 0.925490196f);
	}

	void 
	EnvironmentModule::load(nlohmann::json& reader) noexcept
	{
		if (reader["enable"].is_boolean())
			this->enable = reader["enable"].get<nlohmann::json::boolean_t>();
		if (reader["showBackground"].is_boolean())
			this->showBackground = reader["showBackground"].get<nlohmann::json::boolean_t>();
		if (reader["useTexture"].is_boolean())
			this->useTexture = reader["useTexture"].get<nlohmann::json::boolean_t>();
		if (reader["color"].is_array())
			this->color = octoon::math::float3(reader["color"].get<std::array<float, 3>>());
		if (reader["offset"].is_array())
			this->offset = octoon::math::float2(reader["offset"].get<std::array<float, 2>>());
		if (reader["texture"].is_string())
			this->texture = octoon::AssetDatabase::instance()->loadAssetAtPath<octoon::Texture>(octoon::AssetDatabase::instance()->getAssetPath(reader["texture"].get<std::string>()));
	}

	void 
	EnvironmentModule::save(nlohmann::json& writer) noexcept
	{
		writer["enable"] = this->enable.getValue();
		writer["intensity"] = this->intensity.getValue();
		writer["showBackground"] = this->showBackground.getValue();
		writer["useTexture"] = this->useTexture.getValue();
		writer["offset"] = this->offset.getValue().to_array();
		writer["color"] = this->color.getValue().to_array();

		if (this->texture.getValue())
		{
			if (octoon::AssetDatabase::instance()->contains(this->texture.getValue()))
				writer["texture"] = octoon::AssetDatabase::instance()->getAssetGuid(this->texture.getValue());
			else
			{
				octoon::AssetDatabase::instance()->createFolder("Assets/Environment");
				octoon::AssetDatabase::instance()->createAsset(this->texture.getValue(), "Assets/Environment/Texture.hdr");
				writer["texture"] = octoon::AssetDatabase::instance()->getAssetGuid("Assets/Environment/Texture.hdr");
			}			
		}
	}

	void
	EnvironmentModule::disconnect() noexcept
	{
		this->enable.disconnect();
		this->intensity.disconnect();
		this->showBackground.disconnect();
		this->useTexture.disconnect();
		this->texture.disconnect();
		this->offset.disconnect();
		this->color.disconnect();
	}
}