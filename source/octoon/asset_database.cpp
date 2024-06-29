#include <octoon/asset_database.h>
#include <octoon/asset_importer.h>
#include <octoon/asset_manager.h>
#include <octoon/game_component.h>
#include <octoon/animator_component.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <fstream>

namespace octoon
{
	OctoonImplementSingleton(AssetDatabase)

	AssetDatabase::AssetDatabase() noexcept
	{
	}

	AssetDatabase::~AssetDatabase() noexcept
	{
		assetCaches_.clear();
		assetPipeline_.clear();
	}

	void
	AssetDatabase::mountPackage(const std::u8string& name, const std::filesystem::path& diskPath) noexcept(false)
	{
		for (auto& it : this->assetPipeline_)
		{
			if (it->getName() == name)
				throw std::runtime_error(std::string("Mount package at path ") + (char*)diskPath.u8string().c_str() + " failed.");
		}

		auto pipeline = std::make_shared<AssetPipeline>(name);
		this->assetPipeline_.push_back(pipeline);
		pipeline->open(diskPath);
	}

	void
	AssetDatabase::unmountPackage(const std::u8string& name) noexcept(false)
	{
		for (auto it = this->assetPipeline_.begin(); it != this->assetPipeline_.end(); ++it)
		{
			if ((*it)->getName() == name)
			{
				this->assetPipeline_.erase(it);
				return;
			}
		}
	}

	void
	AssetDatabase::importAsset(const std::filesystem::path& relativePath) noexcept(false)
	{
		auto metadata = AssetManager::instance()->loadMetadataAtPath(relativePath);
		if (metadata.is_null())
		{
			if (!relativePath.is_absolute())
				AssetManager::instance()->createMetadataAtPath(relativePath);
		}
	}

	void
	AssetDatabase::importAsset(const std::filesystem::path& diskPath, const std::filesystem::path& relativePath) noexcept(false)
	{
		assert(!diskPath.empty());

		if (relativePath.empty())
			return;

		if (diskPath.empty() || !std::filesystem::exists(diskPath))
			return;

		for (auto& it : assetPipeline_)
		{
			if (it->isValidPath(relativePath))
			{
				it->importAsset(diskPath, relativePath);
				return;
			}
		}
	}

	void
	AssetDatabase::createAsset(const std::shared_ptr<const Texture>& asset, const std::filesystem::path& relativePath) noexcept(false)
	{
		if (!asset || relativePath.empty())
			throw std::runtime_error(std::string("Creating asset at path ") + (char*)relativePath.u8string().c_str() + " failed.");

		if (AssetDatabase::instance()->contains(asset))
			throw std::runtime_error(std::string("Creating asset at path ") + (char*)relativePath.u8string().c_str() + " failed.");

		for (auto& it : assetPipeline_)
		{
			if (it->isValidPath(relativePath))
			{
				it->createAsset(asset, relativePath);
				return;
			}
		}
	}

	void
	AssetDatabase::createAsset(const std::shared_ptr<const Animation>& asset, const std::filesystem::path& relativePath) noexcept(false)
	{
		if (!asset || relativePath.empty())
			throw std::runtime_error(std::string("Creating asset at path ") + (char*)relativePath.u8string().c_str() + " failed.");

		if (AssetDatabase::instance()->contains(asset))
			throw std::runtime_error(std::string("Creating asset at path ") + (char*)relativePath.u8string().c_str() + " failed.");

		for (auto& it : assetPipeline_)
		{
			if (it->isValidPath(relativePath))
			{
				it->createAsset(asset, relativePath);
				return;
			}
		}
	}

	void
	AssetDatabase::createAsset(const std::shared_ptr<const Material>& asset, const std::filesystem::path& relativePath) noexcept(false)
	{
		if (!asset || relativePath.empty())
			throw std::runtime_error(std::string("Creating asset at path ") + (char*)relativePath.u8string().c_str() + " failed.");

		if (AssetDatabase::instance()->contains(asset))
			throw std::runtime_error(std::string("Creating asset at path ") + (char*)relativePath.u8string().c_str() + " failed.");

		for (auto& it : assetPipeline_)
		{
			if (it->isValidPath(relativePath))
			{
				it->createAsset(asset, relativePath);
				return;
			}
		}
	}

	void
	AssetDatabase::createAsset(const std::shared_ptr<const GameObject>& asset, const std::filesystem::path& relativePath) noexcept(false)
	{
		if (!asset || relativePath.empty())
			throw std::runtime_error(std::string("Creating asset at path ") + (char*)relativePath.u8string().c_str() + " failed.");

		if (AssetDatabase::instance()->contains(asset))
			throw std::runtime_error(std::string("Creating asset at path ") + (char*)relativePath.u8string().c_str() + " failed.");

		for (auto& it : assetPipeline_)
		{
			if (it->isValidPath(relativePath))
			{
				it->createAsset(asset, relativePath);
				return;
			}
		}
	}

	void
	AssetDatabase::createPrefab(const std::shared_ptr<const GameObject>& asset, const std::filesystem::path& relativePath) noexcept(false)
	{
		if (!asset || relativePath.empty())
			throw std::runtime_error(std::string("Creating prefab at path ") + (char*)relativePath.u8string().c_str() + " failed.");

		if (AssetDatabase::instance()->contains(asset) && AssetDatabase::instance()->isPartOfPrefabAsset(asset))
			throw std::runtime_error(std::string("Creating prefab at path ") + (char*)relativePath.u8string().c_str() + " failed.");

		for (auto& it : assetPipeline_)
		{
			if (it->isValidPath(relativePath))
			{
				it->createPrefab(asset, relativePath);
				return;
			}
		}
	}

	bool
	AssetDatabase::isPartOfPrefabAsset(const std::shared_ptr<const GameObject>& asset) const noexcept
	{
		auto path = this->getAssetPath(asset);
		if (!path.empty())
		{
			auto ext = path.extension().wstring();
			for (auto& it : ext)
				it = (char)std::tolower(it);

			return ext == L".prefab";
		}

		for (auto& component : asset->getComponents())
		{
			auto assetPath = this->getAssetPath(component);
			if (!assetPath.empty())
			{
				auto ext = assetPath.extension().wstring();
				for (auto& it : ext)
					it = (char)std::tolower(it);

				if (ext == L".prefab" || ext == L".pmx" || ext == L".obj" || ext == L".fbx")
					return true;
			}
		}

		return false;
	}

	void
	AssetDatabase::deleteAsset(const std::filesystem::path& relativePath) noexcept(false)
	{
		for (auto& it : assetPipeline_)
		{
			if (it->isValidPath(relativePath))
			{
				it->deleteAsset(relativePath);
				return;
			}
		}
	}

	void
	AssetDatabase::saveAssets() noexcept(false)
	{
		for (auto& it : dirtyList_)
		{
			if (!it.expired())
			{
				auto asset = it.lock();
				auto assetPath = this->getAssetPath(asset);

				for (auto& pipeline : assetPipeline_)
				{
					if (!pipeline->isValidPath(assetPath))
						continue;

					if (asset->isInstanceOf<Texture>())
						pipeline->createAsset(asset->downcast_pointer<Texture>(), assetPath);
					else if (asset->isInstanceOf<Material>())
						pipeline->createAsset(asset->downcast_pointer<Material>(), assetPath);
					else if (asset->isInstanceOf<Animation>())
						pipeline->createAsset(asset->downcast_pointer<Animation>(), assetPath);
					else if (asset->isInstanceOf<GameObject>())
					{
						if (this->isPartOfPrefabAsset(asset->downcast_pointer<GameObject>()))
							pipeline->createPrefab(asset->downcast_pointer<GameObject>(), assetPath);
						else
							pipeline->createAsset(asset->downcast_pointer<GameObject>(), assetPath);
					}

					break;
				}
			}
		}

		dirtyList_.clear();

		for (auto& pipeline : assetPipeline_)
			pipeline->saveAssets();
	}

	std::filesystem::path
	AssetDatabase::getAssetPath(const std::string& uuid) const noexcept
	{
		return AssetManager::instance()->getAssetPath(uuid);
	}

	std::filesystem::path
	AssetDatabase::getAssetPath(const std::shared_ptr<const Object>& object) const noexcept
	{
		return AssetManager::instance()->getAssetPath(object);
	}

	std::filesystem::path
	AssetDatabase::getAbsolutePath(const std::filesystem::path& path) const noexcept
	{
		for (auto& it : assetPipeline_)
		{
			if (it->isValidPath(path))
				return it->getAbsolutePath(path);
		}

		return std::filesystem::path();
	}

	std::filesystem::path
	AssetDatabase::getAssetExtension(const std::shared_ptr<const Object>& asset, std::string_view defaultExtension) const noexcept
	{
		auto path = this->getAssetPath(asset);
		if (!path.empty())
			return path.extension();

		return defaultExtension;
	}

	std::string
	AssetDatabase::getAssetGuid(const std::filesystem::path& path) const noexcept
	{
		return AssetManager::instance()->getAssetGuid(path);
	}

	std::string
	AssetDatabase::getAssetGuid(const std::shared_ptr<const Object>& asset) const noexcept
	{
		auto path = this->getAssetPath(asset);
		if (!path.empty())
			return this->getAssetGuid(path);
		return std::string();
	}

	void
	AssetDatabase::createFolder(const std::filesystem::path& relativePath) noexcept(false)
	{
		for (auto& it : assetPipeline_)
		{
			if (it->isValidPath(relativePath))
			{
				it->createFolder(relativePath);
				return;
			}
		}
	}

	void
	AssetDatabase::deleteFolder(const std::filesystem::path& relativePath) noexcept(false)
	{
		for (auto& it : assetPipeline_)
		{
			if (it->isValidPath(relativePath))
			{
				it->deleteFolder(relativePath);
				return;
			}
		}
	}

	bool
	AssetDatabase::contains(const std::shared_ptr<const Object>& asset) const noexcept
	{
		auto assetPath = this->getAssetPath(asset);
		if (!assetPath.empty())
		{
			if (!assetPath.is_absolute())
				return std::filesystem::exists(this->getAbsolutePath(assetPath));
		}

		return false;
	}

	void
	AssetDatabase::setLabels(const std::shared_ptr<const Object>& asset, std::vector<std::string>&& labels) noexcept(false)
	{
		auto assetPath = this->getAssetPath(asset);
		if (!assetPath.empty())
		{
			auto metaData = AssetManager::instance()->loadMetadataAtPath(assetPath);
			if (metaData.is_object())
			{
				metaData["label"] = labels;
				AssetManager::instance()->createMetadataAtPath(assetPath, metaData);
			}

			this->labels_[asset] = std::move(labels);
		}
		else
		{
			throw std::runtime_error(std::string("Failed to set labels."));
		}
	}

	void
	AssetDatabase::setLabels(const std::shared_ptr<const Object>& asset, const std::vector<std::string>& labels) noexcept(false)
	{
		auto assetPath = this->getAssetPath(asset);
		if (!assetPath.empty())
		{
			auto metaData = AssetManager::instance()->loadMetadataAtPath(assetPath);
			if (metaData.is_object())
			{
				metaData["label"] = labels;
				AssetManager::instance()->createMetadataAtPath(assetPath, metaData);
			}

			this->labels_[asset] = labels;
		}
		else
		{
			throw std::runtime_error(std::string("Failed to set labels."));
		}
	}

	const std::vector<std::string>&
	AssetDatabase::getLabels(const std::shared_ptr<const Object>& asset) noexcept(false)
	{
		auto assetPath = this->getAssetPath(asset);
		if (!assetPath.empty())
		{
			auto it = this->labels_.find(asset);
			if (it != this->labels_.end())
				return it->second;

			auto metaData = AssetManager::instance()->loadMetadataAtPath(assetPath);
			if (metaData.is_object())
			{
				for (auto& label : metaData["label"])
					this->labels_[asset].push_back(label.get<std::string>());

				return this->labels_[asset];
			}

			return defaultLabel_;
		}
		else
		{
			throw std::runtime_error(std::string("Failed to get labels."));
		}
	}

	bool
	AssetDatabase::isSubAsset(const std::shared_ptr<const Object>& asset) const noexcept
	{
		return asset->getLocalIdentifier() > 0;
	}

	bool
	AssetDatabase::getGUIDAndLocalIdentifier(const std::shared_ptr<const Object>& asset, std::string& outGuid, std::int64_t& outLocalId)
	{
		auto assetPath = this->getAssetPath(asset);
		if (!assetPath.empty())
		{
			outGuid = this->getAssetGuid(assetPath);
			outLocalId = asset->getLocalIdentifier();
			return true;
		}

		return false;
	}

	std::shared_ptr<Object>
	AssetDatabase::loadAssetAtPath(const std::filesystem::path& path) noexcept(false)
	{
		if (!path.empty())
		{
			for (auto& it : assetPipeline_)
			{
				if (it->isValidPath(path))
				{
					auto asset = it->loadAssetAtPath(path);
					assetCaches_.clear();
					return asset;
				}
			}
		}

		return nullptr;
	}

	std::shared_ptr<Object>
	AssetDatabase::loadAsset(const std::string& guid, std::int64_t localId) noexcept(false)
	{
		auto assetPath = AssetDatabase::instance()->getAssetPath(guid);
		if (!assetPath.empty())
		{
			std::shared_ptr<Object> object;
			if (assetCaches_.contains(assetPath))
				object = assetCaches_.at(assetPath);
			else
			{
				for (auto& it : assetPipeline_)
				{
					if (it->isValidPath(assetPath))
					{
						object = it->loadAssetAtPath(assetPath);
						if (object)
							assetCaches_[assetPath] = object;
						break;
					}
				}
			}

			if (localId > 0)
				return AssetManager::instance()->getSubAssets(assetPath, localId);
			else
				return object;
		}

		return nullptr;
	}

	void
	AssetDatabase::setDirty(const std::shared_ptr<Object>& object, bool dirty) noexcept(false)
	{
		if (this->contains(object))
		{
			if (dirty)
				this->dirtyList_.insert(object);
			else
			{
				auto it = this->dirtyList_.find(object);
				if (it != this->dirtyList_.end())
					this->dirtyList_.erase(it);
			}
		}
	}

	bool
	AssetDatabase::isDirty() const noexcept
	{
		return !this->dirtyList_.empty();
	}

	bool
	AssetDatabase::isDirty(const std::shared_ptr<Object>& object) const noexcept
	{
		return this->dirtyList_.contains(object);
	}
}