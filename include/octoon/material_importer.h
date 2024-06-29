#ifndef OCTOON_MATERIAL_IMPORTER_H_
#define OCTOON_MATERIAL_IMPORTER_H_

#include <octoon/asset_importer.h>
#include <filesystem>

namespace octoon
{
	class OCTOON_EXPORT MaterialImporter final : public AssetImporter
	{
		OctoonDeclareSubClass(MaterialImporter, AssetImporter)
	public:
		MaterialImporter() noexcept;
		~MaterialImporter() noexcept;

		virtual void onImportAsset(AssetImporterContext& context) noexcept(false) override;

	private:
		MaterialImporter(const MaterialImporter&) = delete;
		MaterialImporter& operator=(const MaterialImporter&) = delete;
	};
}

#endif