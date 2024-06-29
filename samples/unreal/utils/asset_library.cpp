#include "asset_library.h"
#include <octoon/runtime/guid.h>
#include <octoon/asset_importer.h>
#include <octoon/asset_preview.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/mdl_loader.h>
#include <fstream>
#include <filesystem>
#include <set>

namespace unreal
{
	OctoonImplementSingleton(AssetLibrary)

	AssetLibrary::AssetLibrary() noexcept
	{
	}

	AssetLibrary::~AssetLibrary() noexcept
	{
		this->close();
	}

	void
	AssetLibrary::open(const std::filesystem::path& path) noexcept(false)
	{
		this->assetPath_ = path;
		this->assetPath_.make_preferred();

		octoon::AssetDatabase::instance()->mountPackage(u8"Packages/Assets/", std::filesystem::path(path).append("Assets"));

		auto libraryPath = std::filesystem::path(path).append("Library");

		std::ifstream textureLib(std::filesystem::path(libraryPath).append("TextureDB.json"), std::ios_base::binary);
		if (textureLib)
		{
			try
			{
				this->textureDb_ = nlohmann::json::parse(textureLib);
				for (auto& package : this->textureDb_)
				{
					auto it = package.find("uuid");
					if (it != package.end())
						this->packageCache_[(*it).get<std::string>()] = package;
				}
			}
			catch (...)
			{
			}
		}

		std::ifstream environmentLib(std::filesystem::path(libraryPath).append("EnvironmentDB.json"), std::ios_base::binary);
		if (environmentLib)
		{
			try
			{
				this->hdriDb_ = nlohmann::json::parse(environmentLib);
				for (auto& package : this->hdriDb_)
				{
					auto it = package.find("uuid");
					if (it != package.end())
						this->packageCache_[(*it).get<std::string>()] = package;
				}
			}
			catch (...)
			{
			}
		}

		std::ifstream motionLib(std::filesystem::path(libraryPath).append("MotionDB.json"), std::ios_base::binary);
		if (motionLib)
		{
			try
			{
				this->motionDb_ = nlohmann::json::parse(motionLib);
				for (auto& package : this->motionDb_)
				{
					auto it = package.find("uuid");
					if (it != package.end())
						this->packageCache_[(*it).get<std::string>()] = package;
				}
			}
			catch (...)
			{
			}
		}

		std::ifstream materialLib(std::filesystem::path(libraryPath).append("MaterialDB.json"), std::ios_base::binary);
		if (materialLib)
		{
			try
			{
				this->materialDb_ = nlohmann::json::parse(materialLib);
				for (auto& package : this->materialDb_)
				{
					auto it = package.find("uuid");
					if (it != package.end())
						this->packageCache_[(*it).get<std::string>()] = package;
				}
			}
			catch (...)
			{
			}
		}

		std::ifstream prefabLib(std::filesystem::path(libraryPath).append("PrefabDB.json"), std::ios_base::binary);
		if (prefabLib)
		{
			try
			{
				this->prefabDb_ = nlohmann::json::parse(prefabLib);
				for (auto& package : this->prefabDb_)
				{
					auto it = package.find("uuid");
					if (it != package.end())
						this->packageCache_[(*it).get<std::string>()] = package;
				}
			}
			catch (...)
			{
			}
		}
	}

	void
	AssetLibrary::close() noexcept
	{
		this->unload();
		this->assetPath_.clear();
	}

	nlohmann::json
	AssetLibrary::importAsset(const std::filesystem::path& path) noexcept(false)
	{
		auto ext = path.extension().u8string();
		for (auto& it : ext)
			it = (char)std::tolower(it);

		if (ext == u8".hdr")
		{
			auto texture = octoon::AssetDatabase::instance()->loadAssetAtPath<octoon::Texture>(path);
			if (texture)
			{
				texture->setMipLevel(8);
				return this->importAsset(texture, "Packages/Assets/Textures");
			}
		}
		if (ext == u8".bmp" || ext == u8".tga" || ext == u8".jpg" || ext == u8".png" || ext == u8".jpeg" || ext == u8".dds")
		{
			auto texture = octoon::AssetDatabase::instance()->loadAssetAtPath<octoon::Texture>(path);
			if (texture)
				return this->importAsset(texture, "Packages/Assets/Textures");
		}
		else if (ext == u8".vmd")
		{
			auto animation = octoon::AssetDatabase::instance()->loadAssetAtPath<octoon::Animation>(path);
			if (animation)
			{
				animation->setName((char*)path.filename().u8string().c_str());
				return this->importAsset(animation, "Packages/Assets/Motions");
			}
		}
		else if (ext == u8".pmx" || ext == u8".obj" || ext == u8".fbx")
		{
			auto modelPath = std::filesystem::path("Packages/Assets/Models").append(octoon::make_guid()).append(path.filename().wstring());
			octoon::AssetDatabase::instance()->importAsset(path, modelPath);

			auto gameObject = octoon::AssetDatabase::instance()->loadAssetAtPath<octoon::GameObject>(modelPath);
			if (gameObject)
				return this->importAsset(gameObject, "Packages/Assets/Prefabs", modelPath.parent_path());
		}
		else if (ext == u8".mdl")
		{
			std::ifstream stream(path);
			if (stream)
			{
				nlohmann::json items;

				octoon::MDLLoader loader;
				loader.load("resource", stream);

				auto materialPath = std::filesystem::path("Packages/Assets/Materials");

				for (auto& material : loader.getMaterials())
					items.push_back(this->importAsset(material, materialPath));

				this->saveAssets();

				return items;
			}
		}

		return nlohmann::json();
	}

	nlohmann::json
	AssetLibrary::importAsset(const std::shared_ptr<octoon::Texture>& texture, const std::filesystem::path& relativeFolder) noexcept(false)
	{
		auto guid = octoon::make_guid();
		auto relativePath = std::filesystem::path(relativeFolder).append(guid.substr(0, 2));

		try
		{
			auto hdr = (texture->format() == octoon::Format::R32G32B32SFloat) ? true : false;
			auto ext = octoon::AssetDatabase::instance()->getAssetExtension(texture, hdr ? ".hdr" : ".png").string();
			auto outputPath = std::filesystem::path(relativePath).append(guid + ext);

			octoon::AssetDatabase::instance()->createFolder(relativePath);
			octoon::AssetDatabase::instance()->createAsset(texture, outputPath);
			octoon::AssetDatabase::instance()->setLabels(texture, { guid });

			nlohmann::json package;
			package["uuid"] = guid;
			package["hdr"] = hdr;
			package["type"] = texture->type_name();
			package["name"] = texture->getName();
			package["data"] = octoon::AssetDatabase::instance()->getAssetGuid(outputPath);
			package["visible"] = true;

			auto preview = octoon::AssetPreview::instance()->getAssetPreview(texture);
			if (preview)
			{
				auto uuid = octoon::make_guid();
				auto previewFolder = std::filesystem::path("Packages/Assets/Thumbnails").append(uuid.substr(0, 2));
				auto previewPath = std::filesystem::path(previewFolder).append(uuid + ".png");
				octoon::AssetDatabase::instance()->createFolder(previewFolder);
				octoon::AssetDatabase::instance()->createAsset(preview, previewPath);
				package["preview"] = octoon::AssetDatabase::instance()->getAssetGuid(previewPath);
			}

			if (hdr)
				this->hdriDb_.push_back(package);
			else
				this->textureDb_.push_back(package);

			this->packageCache_[guid] = package;
			this->saveAssets();

			return std::move(package);
		}
		catch (const std::exception& e)
		{
			octoon::AssetDatabase::instance()->deleteAsset(relativePath);
			throw e;
		}
	}

	nlohmann::json
	AssetLibrary::importAsset(const std::shared_ptr<octoon::Animation>& animation, const std::filesystem::path& relativeFolder) noexcept(false)
	{
		auto guid = octoon::make_guid();
		auto relativePath = std::filesystem::path(relativeFolder).append(guid.substr(0, 2));

		try
		{
			auto ext = octoon::AssetDatabase::instance()->getAssetExtension(animation, ".vmd").string();
			auto outputPath = std::filesystem::path(relativePath).append(guid + ext);

			octoon::AssetDatabase::instance()->createFolder(relativePath);
			octoon::AssetDatabase::instance()->createAsset(animation, outputPath);
			octoon::AssetDatabase::instance()->setLabels(animation, { guid });

			nlohmann::json package;
			package["uuid"] = guid;
			package["type"] = animation->type_name();
			package["name"] = animation->getName();
			package["data"] = octoon::AssetDatabase::instance()->getAssetGuid(outputPath);
			package["visible"] = true;

			this->motionDb_.push_back(package);
			this->packageCache_[guid] = package;
			this->saveAssets();

			return std::move(package);
		}
		catch (const std::exception& e)
		{
			octoon::AssetDatabase::instance()->deleteAsset(relativePath);
			throw e;
		}
	}

	nlohmann::json
	AssetLibrary::importAsset(const std::shared_ptr<octoon::Material>& material, const std::filesystem::path& relativeFolder) noexcept(false)
	{
		auto guid = octoon::make_guid();
		auto relativePath = std::filesystem::path(relativeFolder).append(guid.substr(0, 2));

		try
		{
			auto texturePath = std::filesystem::path("Packages/Assets/Textures");
			auto materialPath = std::filesystem::path(relativePath).append(guid + ".mat");

			for (auto& it : material->getMaterialParams())
			{
				switch (it.type)
				{
				case octoon::PropertyTypeInfo::PropertyTypeInfoTexture:
				{
					auto texture = material->get<std::shared_ptr<octoon::Texture>>(it.key);
					if (texture && !octoon::AssetDatabase::instance()->contains(texture))
						this->importAsset(texture, texturePath);
				}
				break;
				}
			}

			octoon::AssetDatabase::instance()->createFolder(relativePath);
			octoon::AssetDatabase::instance()->createAsset(material, materialPath);
			octoon::AssetDatabase::instance()->setLabels(material, { guid });

			nlohmann::json package;
			package["uuid"] = guid;
			package["type"] = octoon::Material::getRtti()->type_name();
			package["name"] = material->getName();
			package["data"] = octoon::AssetDatabase::instance()->getAssetGuid(materialPath);
			package["visible"] = true;

			auto preview = octoon::AssetPreview::instance()->getAssetPreview(material);
			if (preview)
			{
				auto uuid = octoon::make_guid();
				auto previewFolder = std::filesystem::path("Packages/Assets/Thumbnails").append(uuid.substr(0, 2));
				auto previewPath = std::filesystem::path(previewFolder).append(uuid + ".png");
				octoon::AssetDatabase::instance()->createFolder(previewFolder);
				octoon::AssetDatabase::instance()->createAsset(preview, previewPath);
				package["preview"] = octoon::AssetDatabase::instance()->getAssetGuid(previewPath);
			}

			this->materialDb_.push_back(package);
			this->packageCache_[guid] = package;

			return std::move(package);
		}
		catch (const std::exception& e)
		{
			octoon::AssetDatabase::instance()->deleteAsset(relativePath);
			throw e;
		}
	}

	nlohmann::json
	AssetLibrary::importAsset(const std::shared_ptr<octoon::GameObject>& gameObject, const std::filesystem::path& relativeFolder, const std::filesystem::path& modelPath) noexcept(false)
	{
		auto guid = octoon::make_guid();
		auto relativePath = std::filesystem::path(relativeFolder).append(guid.substr(0, 2));

		try
		{
			auto meshRenderer = gameObject->getComponent<octoon::MeshRendererComponent>();
			if (meshRenderer)
			{
				for (auto& material : meshRenderer->getMaterials())
				{
					if (octoon::AssetDatabase::instance()->contains(material))
						continue;

					for (auto& it : material->getMaterialParams())
					{
						switch (it.type)
						{
						case octoon::PropertyTypeInfo::PropertyTypeInfoTexture:
						{
							auto texture = material->get<std::shared_ptr<octoon::Texture>>(it.key);
							if (texture && !octoon::AssetDatabase::instance()->contains(texture))
							{
								auto uuid = octoon::make_guid();
								auto textureFolder = std::filesystem::path("Packages/Assets/Textures").append(uuid.substr(0, 2));
								octoon::AssetDatabase::instance()->createFolder(textureFolder);
								octoon::AssetDatabase::instance()->createAsset(texture, std::filesystem::path(textureFolder).append(uuid + ".png"));
							}
						}
						break;
						}
					}

					auto materialGuid = octoon::make_guid();
					auto materialFolder = std::filesystem::path("Packages/Assets/Materials").append(materialGuid.substr(0, 2));
					octoon::AssetDatabase::instance()->createFolder(materialFolder);
					octoon::AssetDatabase::instance()->createAsset(material, std::filesystem::path(materialFolder).append(materialGuid + ".mat"));
				}
			}

			auto prefabPath = std::filesystem::path(relativePath).append(guid + ".prefab");

			octoon::AssetDatabase::instance()->createFolder(relativePath);
			octoon::AssetDatabase::instance()->createPrefab(gameObject, prefabPath);
			octoon::AssetDatabase::instance()->setLabels(gameObject, { guid });

			nlohmann::json package;
			package["uuid"] = guid;
			package["visible"] = true;
			package["type"] = gameObject->type_name();
			package["name"] = gameObject->getName();
			package["model"] = octoon::AssetDatabase::instance()->getAssetGuid(modelPath);
			package["data"] = octoon::AssetDatabase::instance()->getAssetGuid(prefabPath);

			auto preview = octoon::AssetPreview::instance()->getAssetPreview(gameObject);
			if (preview)
			{
				auto uuid = octoon::make_guid();
				auto previewFolder = std::filesystem::path("Packages/Assets/Thumbnails").append(uuid.substr(0, 2));
				auto previewPath = std::filesystem::path(previewFolder).append(uuid + ".png");
				octoon::AssetDatabase::instance()->createFolder(previewFolder);
				octoon::AssetDatabase::instance()->createAsset(preview, previewPath);
				package["preview"] = octoon::AssetDatabase::instance()->getAssetGuid(previewPath);
			}

			this->prefabDb_.push_back(package);
			this->packageCache_[guid] = package;

			this->saveAssets();

			return std::move(package);
		}
		catch (const std::exception& e)
		{
			if (std::filesystem::exists(relativePath))
				std::filesystem::remove_all(relativePath);

			throw e;
		}
	}

	std::shared_ptr<octoon::Object>
	AssetLibrary::loadAsset(const std::string& uuid, const octoon::Rtti& type) noexcept(false)
	{
		if (packageCache_.contains(uuid))
		{
			if (type.isDerivedFrom(octoon::Texture::getRtti()))
				return this->loadAssetAtPackage<octoon::Texture>(packageCache_.at(uuid));
			else if (type.isDerivedFrom(octoon::Material::getRtti()))
				return this->loadAssetAtPackage<octoon::Material>(packageCache_.at(uuid));
			else if (type.isDerivedFrom(octoon::Animation::getRtti()))
				return this->loadAssetAtPackage<octoon::Animation>(packageCache_.at(uuid));
			else if (type.isDerivedFrom(octoon::GameObject::getRtti()))
				return this->loadAssetAtPackage<octoon::GameObject>(packageCache_.at(uuid));
		}

		return nullptr;
	}

	std::shared_ptr<octoon::Object>
	AssetLibrary::loadAssetAtPackage(const nlohmann::json& package, const octoon::Rtti& type) noexcept(false)
	{
		if (package.is_object() && package.contains("uuid") && package.contains("data"))
		{
			auto uuid = package["uuid"].get<std::string>();
			if (this->assetCache_.contains(uuid))
			{
				auto it = this->assetCache_.at(uuid);
				if (!it.expired())
					return it.lock();
			}

			auto data = package["data"].get<std::string>();

			std::shared_ptr<octoon::Object> asset;
			if (type.isDerivedFrom(octoon::Texture::getRtti()))
				asset = octoon::AssetDatabase::instance()->loadAssetAtPath<octoon::Texture>(octoon::AssetDatabase::instance()->getAssetPath(data));
			else if (type.isDerivedFrom(octoon::Material::getRtti()))
				asset = octoon::AssetDatabase::instance()->loadAssetAtPath<octoon::Material>(octoon::AssetDatabase::instance()->getAssetPath(data));
			else if (type.isDerivedFrom(octoon::Animation::getRtti()))
				asset = octoon::AssetDatabase::instance()->loadAssetAtPath<octoon::Animation>(octoon::AssetDatabase::instance()->getAssetPath(data));
			else if (type.isDerivedFrom(octoon::GameObject::getRtti()))
				asset = octoon::AssetDatabase::instance()->loadAssetAtPath<octoon::GameObject>(octoon::AssetDatabase::instance()->getAssetPath(data));

			if (asset)
			{
				assetCache_[uuid] = asset;
				assetGuidCache_[asset] = uuid;
			}

			return asset;
		}

		return nullptr;
	}

	bool
	AssetLibrary::hasPackage(const std::string& uuid) noexcept
	{
		return packageCache_.contains(uuid);
	}

	nlohmann::json
	AssetLibrary::getPackage(const std::string& uuid) const noexcept
	{
		if (packageCache_.contains(uuid))
			return packageCache_.at(uuid);
		return nlohmann::json();
	}

	nlohmann::json
	AssetLibrary::getPackage(const std::shared_ptr<octoon::Object>& asset) const noexcept
	{
		if (assetGuidCache_.contains(asset))
			return this->getPackage(assetGuidCache_.at(asset));

		auto label = octoon::AssetDatabase::instance()->getLabels(asset);
		if (!label.empty())
			return this->getPackage(label.front());

		return nlohmann::json();
	}

	std::filesystem::path
	AssetLibrary::getAssetPath(const std::string& uuid, bool absolutePath) const noexcept
	{
		auto assetPath = octoon::AssetDatabase::instance()->getAssetPath(uuid);
		if (absolutePath && !assetPath.empty())
			return octoon::AssetDatabase::instance()->getAbsolutePath(assetPath);
		return assetPath;
	}

	std::filesystem::path
	AssetLibrary::getAssetPath(const std::shared_ptr<const octoon::Object>& asset, bool absolutePath) const noexcept
	{
		auto assetPath = octoon::AssetDatabase::instance()->getAssetPath(asset);
		if (absolutePath && !assetPath.empty())
			return octoon::AssetDatabase::instance()->getAbsolutePath(assetPath);
		return assetPath;
	}

	void
	AssetLibrary::unload() noexcept
	{
		this->assetCache_.clear();
	}

	void
	AssetLibrary::saveAssets() noexcept(false)
	{
		if (octoon::AssetDatabase::instance())
			octoon::AssetDatabase::instance()->saveAssets();

		auto libraryPath = std::filesystem::path(this->assetPath_).append("Library");
		std::filesystem::create_directories(libraryPath);

		std::ofstream textureStream(std::filesystem::path(libraryPath).append("TextureDB.json"), std::ios_base::binary);
		if (textureStream)
		{
			auto dump = textureDb_.dump();
			textureStream.write(dump.c_str(), dump.size());
			textureStream.close();
		}

		std::ofstream materialStream(std::filesystem::path(libraryPath).append("MaterialDB.json"), std::ios_base::binary);
		if (materialStream)
		{
			auto dump = materialDb_.dump();
			materialStream.write(dump.c_str(), dump.size());
			materialStream.close();
		}

		std::ofstream environmentStream(std::filesystem::path(libraryPath).append("EnvironmentDB.json"), std::ios_base::binary);
		if (environmentStream)
		{
			auto dump = hdriDb_.dump();
			environmentStream.write(dump.c_str(), dump.size());
			environmentStream.close();
		}

		std::ofstream animationStream(std::filesystem::path(libraryPath).append("MotionDB.json"), std::ios_base::binary);
		if (animationStream)
		{
			auto dump = motionDb_.dump();
			animationStream.write(dump.c_str(), dump.size());
			animationStream.close();
		}

		std::ofstream prefabStream(std::filesystem::path(libraryPath).append("PrefabDB.json"), std::ios_base::binary);
		if (prefabStream)
		{
			auto dump = prefabDb_.dump();
			prefabStream.write(dump.c_str(), dump.size());
			prefabStream.close();
		}
	}

	void
	AssetLibrary::removeAsset(const std::string& uuid) noexcept(false)
	{
		auto deleteAsset = [this](const std::string& uuid)
		{
			try
			{
				auto path = this->getAssetPath(uuid);
				if (!path.empty())
				{
					if (octoon::AssetDatabase::instance())
						octoon::AssetDatabase::instance()->deleteAsset(path);

					auto parent_path = path.parent_path();
					if (std::filesystem::is_empty(octoon::AssetDatabase::instance()->getAbsolutePath(parent_path)))
						octoon::AssetDatabase::instance()->deleteFolder(parent_path);
				}
			}
			catch (...)
			{
			}
		};

		auto cache = packageCache_.find(uuid);
		if (cache != packageCache_.end())
		{
			auto package = (*cache).second;
			if (package.contains("preview"))
				deleteAsset(package["preview"].get<std::string>());

			if (package.contains("type"))
			{
				auto type = package["type"].get<std::string>();
				if (type == octoon::Texture::getRtti()->type_name())
				{
					if (package.contains("data"))
						deleteAsset(package["data"].get<std::string>());

					auto hdr = package.contains("hdr") ? package["hdr"].get<bool>() : false;
					if (hdr)
					{
						for (auto it = this->hdriDb_.begin(); it != this->hdriDb_.end(); ++it)
						{
							auto uuid_ = (*it)["uuid"].get<std::string>();
							if (uuid_ == uuid)
							{
								this->hdriDb_.erase(it);
								break;
							}
						}
					}
					else
					{
						for (auto it = this->textureDb_.begin(); it != this->textureDb_.end(); ++it)
						{
							auto uuid_ = (*it)["uuid"].get<std::string>();
							if (uuid_ == uuid)
							{
								this->textureDb_.erase(it);
								break;
							}
						}
					}
				}
				else if (type == octoon::Animation::getRtti()->type_name())
				{
					if (package.contains("data"))
						deleteAsset(package["data"].get<std::string>());

					for (auto it = this->motionDb_.begin(); it != this->motionDb_.end(); ++it)
					{
						auto uuid_ = (*it)["uuid"].get<std::string>();
						if (uuid_ == uuid)
						{
							this->motionDb_.erase(it);
							break;
						}
					}
				}
				else if (type == octoon::Material::getRtti()->type_name())
				{
					if (package.contains("data"))
						deleteAsset(package["data"].get<std::string>());

					for (auto it = this->materialDb_.begin(); it != this->materialDb_.end(); ++it)
					{
						auto uuid_ = (*it)["uuid"].get<std::string>();
						if (uuid_ == uuid)
						{
							this->materialDb_.erase(it);
							break;
						}
					}
				}
				else if (type == octoon::GameObject::getRtti()->type_name())
				{
					if (package.contains("data"))
					{
						auto assetPath = octoon::AssetDatabase::instance()->getAssetPath(package["data"].get<std::string>());
						if (!assetPath.empty())
						{
							deleteAsset(uuid);
							octoon::AssetDatabase::instance()->deleteFolder(assetPath.parent_path());
						}
					}

					if (package.contains("model"))
					{
						auto folderPath = this->getAssetPath(package["model"].get<std::string>());
						if (octoon::AssetDatabase::instance())
							octoon::AssetDatabase::instance()->deleteFolder(folderPath);
					}

					for (auto it = this->prefabDb_.begin(); it != this->prefabDb_.end(); ++it)
					{
						auto uuid_ = (*it)["uuid"].get<std::string>();
						if (uuid_ == uuid)
						{
							this->prefabDb_.erase(it);
							break;
						}
					}
				}
			}

			packageCache_.erase(cache);
		}
	}

	const nlohmann::json&
	AssetLibrary::getMotionList() const noexcept
	{
		return motionDb_;
	}

	const nlohmann::json&
	AssetLibrary::getTextureList() const noexcept
	{
		return textureDb_;
	}

	const nlohmann::json&
	AssetLibrary::getHDRiList() const noexcept
	{
		return hdriDb_;
	}

	const nlohmann::json&
	AssetLibrary::getMaterialList() const noexcept
	{
		return materialDb_;
	}

	const nlohmann::json&
	AssetLibrary::getPrefabList() const noexcept
	{
		return prefabDb_;
	}
}