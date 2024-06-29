#ifndef OCTOON_OBJ_IMPORTER_H_
#define OCTOON_OBJ_IMPORTER_H_

#include <octoon/game_object.h>
#include <octoon/asset_importer.h>
#include <filesystem>

namespace octoon
{
	class OCTOON_EXPORT OBJImporter final : public AssetImporter
	{
		OctoonDeclareSubClass(OBJImporter, AssetImporter)
	public:
		OBJImporter() noexcept;
		~OBJImporter() noexcept;

		virtual void onImportAsset(AssetImporterContext& context) noexcept(false) override;

	private:
		OBJImporter(const OBJImporter&) = delete;
		OBJImporter& operator=(const OBJImporter&) = delete;
	};
}

#endif