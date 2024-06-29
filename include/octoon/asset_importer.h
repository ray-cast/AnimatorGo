#ifndef OCTOON_ASSET_IMPORTER_H_
#define OCTOON_ASSET_IMPORTER_H_

#include <octoon/asset_importer_context.h>
#include <filesystem>

namespace octoon
{
	class OCTOON_EXPORT AssetImporter : public Object
	{
		OctoonDeclareSubInterface(AssetImporter, Object)
	public:
		AssetImporter() noexcept;
		virtual ~AssetImporter() noexcept;

		virtual void onImportAsset(AssetImporterContext& context) noexcept(false) = 0;

	private:
		AssetImporter(const AssetImporter&) = delete;
		AssetImporter& operator=(const AssetImporter&) = delete;
	};
}

#endif