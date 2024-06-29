#include <octoon/asset_importer_context.h>
#include <octoon/asset_database.h>
#include <octoon/game_component.h>
#include <fstream>

namespace octoon
{
	AssetImporterContext::AssetImporterContext(const std::filesystem::path& path) noexcept
		: assetPath_(path)
	{
		auto filepath = AssetDatabase::instance()->getAbsolutePath(assetPath_);
		std::ifstream ifs(std::filesystem::path(filepath).concat(L".meta"));
		if (ifs)
			this->metaData_ = nlohmann::json::parse(ifs);
	}

	AssetImporterContext::~AssetImporterContext() noexcept
	{
	}

	void
	AssetImporterContext::setMainObject(const std::shared_ptr<Object>& object) noexcept
	{
		this->mainObject = object;
	}

	const std::shared_ptr<Object>&
	AssetImporterContext::getMainObject() const noexcept
	{
		return this->mainObject;
	}

	void
	AssetImporterContext::addObjectToAsset(std::string_view identifier, const std::shared_ptr<Object>& subAsset)
	{
		assert(subAsset);

		if (subAsset)
		{
			auto globalId = 0;
			if (subAsset->isInstanceOf<Texture>())
				globalId = 100000;
			else if (subAsset->isA<Material>())
				globalId = 200000;
			else if (subAsset->isInstanceOf<Animation>())
				globalId = 300000;
			else if (subAsset->isInstanceOf<Mesh>())
				globalId = 400000;
			else if (subAsset->isInstanceOf<GameObject>())
				globalId = 500000;
			else if (subAsset->isA<GameComponent>())
				globalId = 600000;

			auto& identifiers = identifiers_[subAsset->isA<GameComponent>() ? GameComponent::getRtti()->type_name() : subAsset->type_name()];
			identifiers.insert(std::string(identifier));
			subAsset->setLocalIdentifier(globalId + identifiers.size());
			this->subAssets_.push_back(subAsset);
		}
	}

	const std::vector<std::shared_ptr<Object>>&
	AssetImporterContext::getObjects() const
	{
		return subAssets_;
	}

	const std::filesystem::path&
	AssetImporterContext::getAssetPath() const noexcept
	{
		return assetPath_;
	}

	nlohmann::json
	AssetImporterContext::getMetadata() const noexcept(false)
	{
		return metaData_;
	}
}