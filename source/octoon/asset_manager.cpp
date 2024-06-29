#include <octoon/asset_manager.h>
#include <octoon/asset_database.h>
#include <octoon/runtime/md5.h>

namespace octoon
{
	OctoonImplementSingleton(AssetManager)

	AssetManager::AssetManager() noexcept
	{
	}

	AssetManager::~AssetManager() noexcept
	{
	}

	void
	AssetManager::setAssetPath(const std::shared_ptr<const Object>& object, const std::filesystem::path& path) noexcept
	{
		auto it = assetToPath_.find(object);
		if (it != assetToPath_.end())
		{
			if (pathToSubAssets_.contains(it->second))
			{
				for (auto& subAsset : pathToSubAssets_[it->second])
					assetToPath_[subAsset] = path;
			}
		}

		assetToPath_[object] = path;
	}

	void
	AssetManager::addObjectToAsset(const std::shared_ptr<Object>& object, const std::filesystem::path& path) noexcept
	{
		pathToSubAssets_[path].push_back(object);
	}

	std::filesystem::path
	AssetManager::getAssetPath(const std::shared_ptr<const Object>& object) const noexcept
	{
		auto asset = assetToPath_.find(object);
		if (asset != assetToPath_.end())
			return asset->second;

		return std::filesystem::path();
	}

	std::filesystem::path
	AssetManager::getAssetPath(const std::string& uuid) const noexcept
	{
		auto item = uniques_.find(uuid);
		if (item != uniques_.end())
			return item->second;

		return std::filesystem::path();
	}

	std::string
	AssetManager::getAssetGuid(const std::filesystem::path& path) const noexcept
	{
		auto item = paths_.find(path);
		if (item != paths_.end())
			return item->second;

		return std::string();
	}

	std::shared_ptr<Object>
	AssetManager::getSubAssets(const std::filesystem::path& path, std::int64_t localId) const noexcept
	{
		auto assets = pathToSubAssets_.find(path);
		if (assets != pathToSubAssets_.end())
		{
			for (auto& it : assets->second)
			{
				if (!it.expired())
				{
					auto asset = it.lock();
					if (asset->getLocalIdentifier() == localId)
						return asset;
				}
			}
		}

		return nullptr;
	}

	void
	AssetManager::createMetadataAtPath(const std::filesystem::path& relativePath) noexcept(false)
	{
		nlohmann::json metadata;
		metadata["uuid"] = MD5(std::filesystem::path(relativePath).make_preferred().u8string()).toString();

		this->createMetadataAtPath(relativePath, metadata);
	}

	void
	AssetManager::createMetadataAtPath(const std::filesystem::path& path, const nlohmann::json& json) noexcept(false)
	{
		std::ofstream ifs(AssetDatabase::instance()->getAbsolutePath(path).concat(L".meta"), std::ios_base::binary);
		if (ifs)
		{
			auto uuid = json["uuid"].get<std::string>();

			paths_[path] = uuid;
			uniques_[uuid] = path;

			auto dump = json.dump();
			ifs.write(dump.c_str(), dump.size());
			ifs.close();
		}
		else
		{
			throw std::runtime_error(std::string("Creating metadata at path ") + (char*)path.u8string().c_str() + " failed.");
		}
	}

	void
	AssetManager::removeMetadataAtPath(const std::filesystem::path& relativePath_) noexcept
	{
		auto relativePath = std::filesystem::path(relativePath_).make_preferred();
		auto uuid = this->getAssetGuid(relativePath);
		paths_.erase(paths_.find(relativePath));
		uniques_.erase(uniques_.find(uuid));

		auto metaPath = AssetDatabase::instance()->getAbsolutePath(relativePath).concat(L".meta");
		if (std::filesystem::exists(metaPath))
			std::filesystem::remove(metaPath);
	}

	nlohmann::json
	AssetManager::loadMetadataAtPath(const std::filesystem::path& relativePath) noexcept(false)
	{
		std::ifstream ifs(AssetDatabase::instance()->getAbsolutePath(relativePath).concat(L".meta"));
		if (ifs)
		{
			auto metaData = nlohmann::json::parse(ifs);
			if (metaData.is_object())
			{
				if (metaData.contains("uuid"))
				{
					auto guid = metaData["uuid"].get<std::string>();
					paths_[relativePath] = guid;
					uniques_[guid] = relativePath;
				}
			}

			return metaData;
		}

		return nlohmann::json();
	}
}