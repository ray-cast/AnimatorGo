#include "camera_module.h"
#include <octoon/asset_database.h>

namespace unreal
{
	CameraModule::CameraModule() noexcept
	{
		this->reset();
	}

	CameraModule::~CameraModule() noexcept
	{
	}

	void
	CameraModule::reset() noexcept
	{
		this->useDepthOfFiled = false;
		this->fov = 60.0f;
		this->focalLength = 31.18f;
		this->aperture = 5.6f;
		this->focusDistance = 10.0f;
		this->framebufferSize = octoon::math::uint2(1280, 720);
		this->translate = octoon::math::float3(0, 10, -10);
		this->rotation = octoon::math::float3::Zero;
		this->animation = nullptr;
	}

	void 
	CameraModule::load(nlohmann::json& reader) noexcept
	{
		if (reader.contains("useDepthOfFiled"))
			this->useDepthOfFiled = reader["useDepthOfFiled"].get<nlohmann::json::boolean_t>();
		if (reader["fov"].is_number_float())
			this->fov = reader["fov"].get<nlohmann::json::number_float_t>();
		if (reader["aperture"].is_number_float())
			this->aperture = reader["aperture"].get<nlohmann::json::number_float_t>();
		if (reader["focusDistance"].is_number_float())
			this->focusDistance = reader["focusDistance"].get<nlohmann::json::number_float_t>();
		if (reader["size"].is_array())
			this->framebufferSize = octoon::math::uint2(reader["size"].get<std::array<std::uint32_t, 2>>());
		if (reader["translate"].is_array())
			this->translate = octoon::math::float3(reader["translate"].get<std::array<float, 3>>());
		if (reader["rotation"].is_array())
			this->rotation = octoon::math::float3(reader["rotation"].get<std::array<float, 3>>());
		if (reader["animation"].is_string())
			this->animation = octoon::AssetDatabase::instance()->loadAssetAtPath<octoon::Animation>(octoon::AssetDatabase::instance()->getAssetPath(reader["animation"].get<std::string>()));
	}

	void 
	CameraModule::save(nlohmann::json& writer) noexcept
	{
		writer["useDepthOfFiled"] = this->useDepthOfFiled.getValue();
		writer["fov"] = this->fov.getValue();
		writer["aperture"] = this->aperture.getValue();
		writer["focusDistance"] = this->focusDistance.getValue();
		writer["size"] = this->framebufferSize.getValue().to_array();
		writer["translate"] = this->translate.getValue().to_array();
		writer["rotation"] = this->rotation.getValue().to_array();

		if (this->animation.getValue() && !this->animation.getValue()->clips.empty())
		{
			if (octoon::AssetDatabase::instance()->contains(this->animation.getValue()))
				writer["animation"] = octoon::AssetDatabase::instance()->getAssetGuid(this->animation.getValue());
			else
			{
				octoon::AssetDatabase::instance()->createFolder("Assets/Camera");
				octoon::AssetDatabase::instance()->createAsset(this->animation.getValue(), "Assets/Camera/Camera.vmd");
				writer["animation"] = octoon::AssetDatabase::instance()->getAssetGuid("Assets/Camera/Camera.vmd");
			}			
		}
	}

	void
	CameraModule::disconnect() noexcept
	{
		this->enable.disconnect();
		this->useDepthOfFiled.disconnect();
		this->fov.disconnect();
		this->focalLength.disconnect();
		this->aperture.disconnect();
		this->focusDistance.disconnect();
		this->translate.disconnect();
		this->rotation.disconnect();
		this->framebufferSize.disconnect();
		this->animation.disconnect();
		this->camera.disconnect();
	}
}