#ifndef OCTOON_ASS_IMPORTER_H_
#define OCTOON_ASS_IMPORTER_H_

#include <octoon/game_object.h>
#include <octoon/asset_importer.h>
#include <filesystem>

namespace octoon
{
	class OCTOON_EXPORT ASSImporter final : public AssetImporter
	{
		OctoonDeclareSubClass(ASSImporter, AssetImporter)
	public:
		ASSImporter() noexcept;
		~ASSImporter() noexcept;

		virtual void onImportAsset(AssetImporterContext& context) noexcept(false) override;

	private:
		ASSImporter(const ASSImporter&) = delete;
		ASSImporter& operator=(const ASSImporter&) = delete;
	};
}

#endif