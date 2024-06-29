#include <octoon/prefab_importer.h>
#include <octoon/asset_database.h>
#include <fstream>

namespace octoon
{
	OctoonImplementSubClass(PrefabImporter, AssetImporter, "PrefabImporter")

	PrefabImporter::PrefabImporter() noexcept
	{
	}

	PrefabImporter::~PrefabImporter()
	{
	}

	void
	PrefabImporter::onImportAsset(AssetImporterContext& context) noexcept(false)
	{
		auto filepath = AssetDatabase::instance()->getAbsolutePath(context.getAssetPath());
		std::ifstream ifs(filepath);
		if (ifs)
		{
			auto prefab = nlohmann::json::parse(ifs);
			auto object = std::make_shared<GameObject>();
			object->load(prefab);

			context.setMainObject(object);
		}
	}
}