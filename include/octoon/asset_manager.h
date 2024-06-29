#ifndef OCTOON_ASSET_MANAGER_H_
#define OCTOON_ASSET_MANAGER_H_

#include <octoon/runtime/guid.h>
#include <octoon/runtime/singleton.h>
#include <octoon/runtime/object.h>
#include <octoon/runtime/json.h>
#include <map>
#include <filesystem>

namespace octoon
{
	class OCTOON_EXPORT AssetManager final
	{
		OctoonDeclareSingleton(AssetManager)
	public:
		AssetManager() noexcept;
		~AssetManager() noexcept;

		void setAssetPath(const std::shared_ptr<const Object>& object, const std::filesystem::path& path) noexcept;
		void addObjectToAsset(const std::shared_ptr<Object>& object, const std::filesystem::path& path) noexcept;

		std::filesystem::path getAssetPath(const std::string& uuid) const noexcept;
		std::filesystem::path getAssetPath(const std::shared_ptr<const Object>& object) const noexcept;

		std::shared_ptr<Object> getSubAssets(const std::filesystem::path& path, std::int64_t localId) const noexcept;

		std::string getAssetGuid(const std::filesystem::path& path) const noexcept;

		void createMetadataAtPath(const std::filesystem::path& path) noexcept(false);
		void createMetadataAtPath(const std::filesystem::path& path, const nlohmann::json& json) noexcept(false);
		void removeMetadataAtPath(const std::filesystem::path& path) noexcept;
		nlohmann::json loadMetadataAtPath(const std::filesystem::path& path) noexcept(false);

	private:
		AssetManager(const AssetManager&) = delete;
		AssetManager& operator=(const AssetManager&) = delete;

	private:
		std::map<std::filesystem::path, std::string> paths_;
		std::map<std::string, std::filesystem::path> uniques_;
		std::map<std::weak_ptr<const Object>, std::filesystem::path, std::owner_less<std::weak_ptr<const Object>>> assetToPath_;
		std::map<std::filesystem::path, std::vector<std::weak_ptr<Object>>> pathToSubAssets_;
	};
}

#endif