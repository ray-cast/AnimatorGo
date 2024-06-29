#include <octoon/alembic_importer.h>
#include <octoon/asset_database.h>
#include <octoon/mesh_animation_component.h>

namespace octoon
{
	OctoonImplementSubClass(AlembicImporter, AssetImporter, "AlembicImporter")

	AlembicImporter::AlembicImporter() noexcept
	{
	}

	AlembicImporter::~AlembicImporter()
	{
	}

	void
	AlembicImporter::onImportAsset(AssetImporterContext& context) noexcept(false)
	{
		auto filepath = AssetDatabase::instance()->getAbsolutePath(context.getAssetPath());

		auto model = std::make_shared<GameObject>();
		if (model)
		{
			auto alembic = model->addComponent<MeshAnimationComponent>();
			alembic->setFilePath(filepath);
			context.setMainObject(model);
		}
	}
}