#ifndef OCTOON_ASSET_PIPELINE_H_
#define OCTOON_ASSET_PIPELINE_H_

#include <octoon/game_object.h>
#include <octoon/asset_importer.h>
#include <octoon/texture/texture.h>
#include <octoon/material/mesh_standard_material.h>
#include <octoon/animation/animation.h>
#include <filesystem>
#include <set>

namespace octoon
{
	class OCTOON_EXPORT AssetPipeline final
	{
	public:
		AssetPipeline(const std::u8string& name) noexcept;
		~AssetPipeline() noexcept;

		void open(const std::filesystem::path& diskPath) noexcept(false);
		void close() noexcept;

		const std::u8string& getName() const noexcept;

		void importAsset(const std::filesystem::path& diskPath, const std::filesystem::path& assetPath) noexcept(false);

		void createAsset(const std::shared_ptr<const Texture>& asset, const std::filesystem::path& relativePath) noexcept(false);
		void createAsset(const std::shared_ptr<const Animation>& animation, const std::filesystem::path& assetPath) noexcept(false);
		void createAsset(const std::shared_ptr<const Material>& material, const std::filesystem::path& assetPath) noexcept(false);
		void createAsset(const std::shared_ptr<const GameObject>& object, const std::filesystem::path& assetPath) noexcept(false);
		void deleteAsset(const std::filesystem::path& assetPath) noexcept(false);

		void createPrefab(const std::shared_ptr<const GameObject>& object, const std::filesystem::path& relativePath) noexcept(false);

		void createFolder(const std::filesystem::path& assetFolder) noexcept(false);
		void deleteFolder(const std::filesystem::path& assetFolder) noexcept(false);

		bool isValidPath(const std::filesystem::path& diskPath) const noexcept;

		std::filesystem::path getAbsolutePath(const std::filesystem::path& relativePath) const noexcept;

		void saveAssets() noexcept(false);

		std::shared_ptr<Object> loadAssetAtPath(const std::filesystem::path& assetPath) noexcept(false);

		template<typename T>
		std::shared_ptr<T> loadAssetAtPath(const std::filesystem::path& assetPath) noexcept(false)
		{
			auto asset = loadAssetAtPath(assetPath);
			if (asset)
				return asset->downcast_pointer<T>();
			return nullptr;
		}

	private:
		std::filesystem::path getRelativePath(const std::filesystem::path& assetPath) const noexcept(false);

	private:
		AssetPipeline(const AssetPipeline&) = delete;
		AssetPipeline& operator=(const AssetPipeline&) = delete;

	private:
		std::u8string name_;
		std::filesystem::path rootPath_;

		std::set<std::filesystem::path> assetPaths_;
	};
}

#endif