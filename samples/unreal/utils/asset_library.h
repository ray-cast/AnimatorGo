#ifndef UNREAL_ASSET_DATABASE_H_
#define UNREAL_ASSET_DATABASE_H_

#include <octoon/game_object.h>
#include <octoon/texture/texture.h>
#include <octoon/material/material.h>
#include <octoon/animation/animation.h>
#include <octoon/runtime/singleton.h>
#include <octoon/asset_database.h>
#include <filesystem>

namespace unreal
{
	class AssetLibrary final
	{
		OctoonDeclareSingleton(AssetLibrary)
	public:
		AssetLibrary() noexcept;
		virtual ~AssetLibrary() noexcept;

		void open(const std::filesystem::path& indexPath) noexcept(false);
		void close() noexcept;

		nlohmann::json importAsset(const std::filesystem::path& path) noexcept(false);
		
		nlohmann::json getPackage(const std::string& uuid) const noexcept;
		nlohmann::json getPackage(const std::shared_ptr<octoon::Object>& asset) const noexcept;

		std::filesystem::path getAssetPath(const std::string& uuid, bool absolutePath = false) const noexcept;
		std::filesystem::path getAssetPath(const std::shared_ptr<const octoon::Object>& asset, bool absolutePath = false) const noexcept;

		const nlohmann::json& getMotionList() const noexcept;
		const nlohmann::json& getTextureList() const noexcept;
		const nlohmann::json& getHDRiList() const noexcept;
		const nlohmann::json& getMaterialList() const noexcept;
		const nlohmann::json& getPrefabList() const noexcept;

		bool hasPackage(const std::string& uuid) noexcept;

		void unload() noexcept;
		void saveAssets() noexcept(false);
		void removeAsset(const std::string& uuid) noexcept(false);

		std::shared_ptr<octoon::Object> loadAsset(const std::string& uuid, const octoon::Rtti& rtti) noexcept(false);

		template<typename T, typename = std::enable_if_t<std::is_base_of<octoon::Object, T>::value>>
		std::shared_ptr<T> loadAsset(const std::string& uuid) noexcept(false)
		{
			auto asset = loadAsset(uuid, *T::getRtti());
			if (asset)
				return asset->downcast_pointer<T>();
			return nullptr;
		}

	private:
		nlohmann::json importAsset(const std::shared_ptr<octoon::Texture>& texture, const std::filesystem::path& relativeFolder) noexcept(false);
		nlohmann::json importAsset(const std::shared_ptr<octoon::Animation>& animation, const std::filesystem::path& relativeFolder) noexcept(false);
		nlohmann::json importAsset(const std::shared_ptr<octoon::Material>& material, const std::filesystem::path& relativeFolder) noexcept(false);
		nlohmann::json importAsset(const std::shared_ptr<octoon::GameObject>& gameObject, const std::filesystem::path& relativeFolder, const std::filesystem::path& modelPath) noexcept(false);

		std::shared_ptr<octoon::Object> loadAssetAtPackage(const nlohmann::json& package, const octoon::Rtti& type) noexcept(false);

		template<typename T, typename = std::enable_if_t<std::is_base_of<octoon::Object, T>::value>>
		std::shared_ptr<T> loadAssetAtPackage(const nlohmann::json& package) noexcept(false)
		{
			auto asset = loadAssetAtPackage(package, *T::getRtti());
			if (asset)
				return asset->downcast_pointer<T>();
			return nullptr;
		}

	private:
		nlohmann::json motionDb_;
		nlohmann::json textureDb_;
		nlohmann::json hdriDb_;
		nlohmann::json materialDb_;
		nlohmann::json prefabDb_;

		std::filesystem::path assetPath_;

		std::map<std::string, std::weak_ptr<octoon::Object>> assetCache_;
		std::map<std::string, nlohmann::json> packageCache_;
		std::map<std::weak_ptr<octoon::Object>, std::string, std::owner_less<std::weak_ptr<octoon::Object>>> assetGuidCache_;
	};
}

#endif