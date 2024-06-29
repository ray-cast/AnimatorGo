#include "entities_module.h"
#include <octoon/runtime/guid.h>
#include <octoon/asset_database.h>
#include <octoon/asset_importer.h>
#include <octoon/animator_component.h>
#include <octoon/skinned_mesh_renderer_component.h>
#include <octoon/mesh_animation_component.h>
#include <octoon/transform_component.h>
#include <filesystem>

namespace unreal
{
	EntitiesModule::EntitiesModule() noexcept
	{
		this->reset();
	}

	EntitiesModule::~EntitiesModule() noexcept
	{
	}

	void
	EntitiesModule::reset() noexcept
	{
		this->objects.getValue().clear();
	}

	void 
	EntitiesModule::load(nlohmann::json& reader) noexcept(false)
	{
		if (reader.contains("scene"))
		{
			octoon::GameObjects objects_;

			for (auto& it : reader["scene"])
			{
				auto uuid = it.get<std::string>();
				auto assetPath = octoon::AssetDatabase::instance()->getAssetPath(uuid);
				if (!assetPath.empty())
				{
					auto object = octoon::AssetDatabase::instance()->loadAssetAtPath<octoon::GameObject>(assetPath);
					if (object)
						objects_.push_back(std::move(object));
				}
			}

			this->objects = std::move(objects_);
		}
	}

	void 
	EntitiesModule::save(nlohmann::json& writer) noexcept(false)
	{
		nlohmann::json sceneJson;

		for (auto& it : this->objects.getValue())
		{
			if (octoon::AssetDatabase::instance()->contains(it))
				sceneJson.push_back(octoon::AssetDatabase::instance()->getAssetGuid(it));
			else
			{
				auto modelPath = octoon::AssetDatabase::instance()->getAssetPath(it);
				if (!modelPath.empty() && modelPath.is_absolute())
				{
					auto outputPath = std::filesystem::path("Assets/Models").append(octoon::make_guid());
					octoon::AssetDatabase::instance()->createFolder(outputPath);
					octoon::AssetDatabase::instance()->createAsset(it, outputPath.append(modelPath.filename().wstring()));

					auto uuid = octoon::make_guid();
					auto path = std::filesystem::path("Assets/Prefabs").append(uuid + ".prefab");
					octoon::AssetDatabase::instance()->createFolder("Assets/Prefabs");
					octoon::AssetDatabase::instance()->createPrefab(it, path);
					sceneJson.push_back(octoon::AssetDatabase::instance()->getAssetGuid(path));
				}
				else
				{
					auto uuid = octoon::make_guid();
					auto path = std::filesystem::path("Assets/Prefabs").append(uuid + ".prefab");
					octoon::AssetDatabase::instance()->createFolder("Assets/Prefabs");
					octoon::AssetDatabase::instance()->createPrefab(it, path);
					sceneJson.push_back(octoon::AssetDatabase::instance()->getAssetGuid(path));
				}
			}
		}

		writer["scene"] = std::move(sceneJson);
	}

	void
	EntitiesModule::disconnect() noexcept
	{
		this->enable.disconnect();
		this->objects.disconnect();
	}
}