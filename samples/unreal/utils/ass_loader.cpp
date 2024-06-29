#include "ass_loader.h"
#include <octoon/ass_importer.h>

namespace unreal
{
	AssLoader::AssLoader() noexcept
	{
	}

	AssLoader::~AssLoader() noexcept
	{
	}
	
	void
	AssLoader::load(UnrealProfile& profile, const std::filesystem::path& path) noexcept(false)
	{
		auto importer = std::make_shared<octoon::ASSImporter>();

		auto context = std::make_shared<octoon::AssetImporterContext>(path);
		importer->onImportAsset(*context);
		
		if (context->getMainObject())
		{
			for (auto& it : context->getMainObject()->downcast<octoon::GameObject>()->getChildren())
			{
				if (it->getComponent<octoon::CameraComponent>())
					profile.cameraModule->camera = it;
				else
					profile.entitiesModule->objects.getValue().push_back(it);
			}
		}
	}
}