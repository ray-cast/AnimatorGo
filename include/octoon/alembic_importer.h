#ifndef OCTOON_ALEMBIC_IMPORTER_H_
#define OCTOON_ALEMBIC_IMPORTER_H_

#include <octoon/asset_importer.h>
#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT AlembicImporter final : public AssetImporter
	{
		OctoonDeclareSubClass(AlembicImporter, AssetImporter)
	public:
		AlembicImporter() noexcept;
		virtual ~AlembicImporter() noexcept;

		virtual void onImportAsset(AssetImporterContext& context) noexcept(false) override;

	private:
		AlembicImporter(const AlembicImporter&) = delete;
		AlembicImporter& operator=(const AlembicImporter&) = delete;
	};
}

#endif