#include "material_importer.h"
#include <octoon/runtime/guid.h>
#include <octoon/asset_database.h>

namespace unreal
{
	OctoonImplementSingleton(MaterialImporter)

	MaterialImporter::MaterialImporter() noexcept
	{
	}

	MaterialImporter::~MaterialImporter() noexcept
	{
	}

	void
	MaterialImporter::clear() noexcept(false)
	{
		this->materialMap_.clear();
		this->sceneList_.clear();
		this->assetGuidList_.clear();
		this->packageList_.clear();
	}

	const std::vector<nlohmann::json>&
	MaterialImporter::getSceneList() const noexcept
	{
		return this->sceneList_;
	}

	std::shared_ptr<octoon::Material>
	MaterialImporter::getMaterial(std::string_view uuid) noexcept(false)
	{
		if (materialMap_.contains(std::string(uuid)))
			return materialMap_[std::string(uuid)];

		return nullptr;
	}

	nlohmann::json
	MaterialImporter::getPackage(std::string_view uuid) noexcept
	{
		if (packageList_.contains((std::string)uuid))
			return packageList_[(std::string)uuid];

		return nlohmann::json();
	}

	bool
	MaterialImporter::addMaterial(const std::shared_ptr<octoon::Material>& mat)
	{
		if (!assetGuidList_.contains(mat))
		{
			auto guid = octoon::make_guid();
			auto standard = mat->downcast_pointer<octoon::MeshStandardMaterial>();

			nlohmann::json package;
			package["uuid"] = guid;
			package["name"] = mat->getName();
			package["color"] = standard->getColor().to_array();

			this->sceneList_.push_back(guid);
			this->materialMap_[guid] = mat;
			this->assetGuidList_[mat] = guid;
			this->packageList_[guid] = package;

			return true;
		}

		return false;
	}
}