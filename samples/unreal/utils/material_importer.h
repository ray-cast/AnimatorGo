#ifndef UNREAL_MATERIAL_IMPORTER_H_
#define UNREAL_MATERIAL_IMPORTER_H_

#include <map>
#include <octoon/material/mesh_standard_material.h>
#include <octoon/runtime/singleton.h>
#include <octoon/runtime/json.h>

namespace unreal
{
	class MaterialImporter final
	{
		OctoonDeclareSingleton(MaterialImporter)
	public:
		MaterialImporter() noexcept;
		virtual ~MaterialImporter() noexcept;

		void clear() noexcept(false);

		bool addMaterial(const std::shared_ptr<octoon::Material>& material);

		std::shared_ptr<octoon::Material> getMaterial(std::string_view uuid) noexcept(false);

		nlohmann::json getPackage(std::string_view uuid) noexcept;

		const std::vector<nlohmann::json>& getSceneList() const noexcept;

	private:
		MaterialImporter(const MaterialImporter&) = delete;
		MaterialImporter& operator=(const MaterialImporter&) = delete;

	private:
		std::vector<nlohmann::json> sceneList_;
		std::map<std::string, nlohmann::json> packageList_;
		std::map<std::string, std::shared_ptr<octoon::Material>> materialMap_;
		std::map<std::weak_ptr<const octoon::Material>, std::string, std::owner_less<std::weak_ptr<const octoon::Material>>> assetGuidList_;
	};
}

#endif