#ifndef OCTOON_PMX_IMPORTER_H_
#define OCTOON_PMX_IMPORTER_H_

#include <octoon/pmx.h>
#include <octoon/game_object.h>
#include <octoon/material/material.h>
#include <octoon/asset_importer.h>

namespace octoon
{
	class OCTOON_EXPORT PMXImporter final : public AssetImporter
	{
		OctoonDeclareSubClass(PMXImporter, AssetImporter)
	public:
		PMXImporter() noexcept;
		virtual ~PMXImporter() noexcept;

		virtual void onImportAsset(AssetImporterContext& context) noexcept(false) override;

		static bool save(const GameObject& gameObject, PMX& pmx, const std::filesystem::path& path) noexcept(false);
		static bool save(const GameObject& gameObject, const std::filesystem::path& path) noexcept(false);

	private:
		void createBones(AssetImporterContext& context, const PMX& pmx, GameObjects& bones) noexcept(false);
		void createClothes(AssetImporterContext& context, const PMX& pmx, GameObjectPtr& meshes, const GameObjects& bones) noexcept(false);
		void createColliders(AssetImporterContext& context, const PMX& pmx, GameObjects& bones) noexcept(false);
		void createRigidbodies(AssetImporterContext& context, const PMX& pmx, GameObjects& bones) noexcept(false);
		void createJoints(AssetImporterContext& context, const PMX& pmx, GameObjects& bones) noexcept(false);
		void createMorph(AssetImporterContext& context, const PMX& pmx, GameObjectPtr& mesh) noexcept(false);
		void createMeshes(AssetImporterContext& context, const PMX& pmx, GameObjectPtr& object, const GameObjects& bones) noexcept(false);
		void createMaterials(AssetImporterContext& context, const PMX& pmx, GameObjectPtr& object, Materials& materials) noexcept(false);
	};
}

#endif