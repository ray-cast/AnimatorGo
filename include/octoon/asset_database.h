#ifndef OCTOON_ASSET_DATABASE_H_
#define OCTOON_ASSET_DATABASE_H_

#include <octoon/video/renderer.h>
#include <octoon/asset_pipeline.h>
#include <filesystem>
#include <set>
#include <map>

namespace octoon
{
	class OCTOON_EXPORT AssetDatabase final
	{
		OctoonDeclareSingleton(AssetDatabase)
	public:
		AssetDatabase() noexcept;
		virtual ~AssetDatabase() noexcept;

		void mountPackage(const std::u8string& name, const std::filesystem::path& diskPath) noexcept(false);
		void unmountPackage(const std::u8string& name) noexcept(false);

		void importAsset(const std::filesystem::path& assetPath) noexcept(false);
		void importAsset(const std::filesystem::path& diskPath, const std::filesystem::path& assetPath) noexcept(false);

		void createAsset(const std::shared_ptr<const Texture>& texture, const std::filesystem::path& assetPath) noexcept(false);
		void createAsset(const std::shared_ptr<const Animation>& animation, const std::filesystem::path& assetPath) noexcept(false);
		void createAsset(const std::shared_ptr<const Material>& material, const std::filesystem::path& assetPath) noexcept(false);
		void createAsset(const std::shared_ptr<const GameObject>& object, const std::filesystem::path& assetPath) noexcept(false);

		void createPrefab(const std::shared_ptr<const GameObject>& object, const std::filesystem::path& relativePath) noexcept(false);
		bool isPartOfPrefabAsset(const std::shared_ptr<const GameObject>& object) const noexcept;

		bool contains(const std::shared_ptr<const Object>& asset) const noexcept;

		void deleteAsset(const std::filesystem::path& assetPath) noexcept(false);
		void saveAssets() noexcept(false);

		void createFolder(const std::filesystem::path& assetFolder) noexcept(false);
		void deleteFolder(const std::filesystem::path& assetFolder) noexcept(false);

		std::filesystem::path getAssetPath(const std::string& uuid) const noexcept;
		std::filesystem::path getAssetPath(const std::shared_ptr<const Object>& asset) const noexcept;
		std::filesystem::path getAbsolutePath(const std::filesystem::path& assetPath) const noexcept;
		std::filesystem::path getAssetExtension(const std::shared_ptr<const Object>& asset, std::string_view defaultExtension = "") const noexcept;

		std::string getAssetGuid(const std::filesystem::path& assetPath) const noexcept;
		std::string getAssetGuid(const std::shared_ptr<const Object>& asset) const noexcept;

		void setLabels(const std::shared_ptr<const Object>& asset, std::vector<std::string>&& labels) noexcept(false);
		void setLabels(const std::shared_ptr<const Object>& asset, const std::vector<std::string>& labels) noexcept(false);
		const std::vector<std::string>& getLabels(const std::shared_ptr<const Object>& asset) noexcept(false);

		bool isSubAsset(const std::shared_ptr<const Object>& asset) const noexcept;
		bool getGUIDAndLocalIdentifier(const std::shared_ptr<const Object>& asset, std::string& outGuid, std::int64_t& outLocalId);

		std::shared_ptr<Object> loadAsset(const std::string& guid, std::int64_t localId) noexcept(false);
		std::shared_ptr<Object> loadAssetAtPath(const std::filesystem::path& assetPath) noexcept(false);

		template<typename T>
		std::shared_ptr<T> loadAsset(const std::string& guid, std::int64_t localId) noexcept(false)
		{
			auto asset = loadAsset(guid, localId);
			if (asset)
				return asset->downcast_pointer<T>();
			return nullptr;
		}

		template<typename T>
		std::shared_ptr<T> loadAssetAtPath(const std::filesystem::path& assetPath) noexcept(false)
		{
			auto asset = loadAssetAtPath(assetPath);
			if (asset)
				return asset->downcast_pointer<T>();
			return nullptr;
		}

		bool isDirty() const noexcept;
		bool isDirty(const std::shared_ptr<Object>& object) const noexcept;
		void setDirty(const std::shared_ptr<Object>& object, bool dirty = true) noexcept(false);

	private:
		AssetDatabase(const AssetDatabase&) = delete;
		AssetDatabase& operator=(const AssetDatabase&) = delete;

	private:
		std::vector<std::string> defaultLabel_;

		std::vector<std::shared_ptr<AssetPipeline>> assetPipeline_;
		std::map<std::filesystem::path, std::shared_ptr<Object>> assetCaches_;

		std::set<std::weak_ptr<const Object>, std::owner_less<std::weak_ptr<const Object>>> dirtyList_;
		std::map<std::weak_ptr<const Object>, std::vector<std::string>, std::owner_less<std::weak_ptr<const Object>>> labels_;
	};
}

#endif