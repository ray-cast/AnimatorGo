#include "material_component.h"
#include <fstream>
#include "../rabbit_profile.h"
#include "../rabbit_behaviour.h"
#include <qstring.h>
#include <octoon/game_base_features.h>

namespace rabbit
{
	MaterialComponent::MaterialComponent() noexcept
	{
	}

	MaterialComponent::~MaterialComponent() noexcept
	{
	}

	void
	MaterialComponent::setActive(bool active) noexcept
	{
	}

	bool
	MaterialComponent::getActive() const noexcept
	{
		return true;
	}

	void
	MaterialComponent::onEnable() noexcept
	{
	}

	void
	MaterialComponent::onDisable() noexcept
	{
	}

	void
	MaterialComponent::loadMaterial(std::string_view mtlPath) noexcept(false)
	{
		std::ifstream stream(QString::fromStdString(std::string(mtlPath)).toStdWString());
		if (stream)
		{
			std::map<std::string, int> materialMap;
			std::vector<tinyobj::material_t> materials;
			auto err = tinyobj::LoadMtl(materialMap, materials, stream);
			auto dirpath = mtlPath.substr(0, mtlPath.find_last_of("/") + 1);

			this->initMaterials(materials, dirpath);
		}
	}

	const octoon::material::Materials&
	MaterialComponent::getMaterials() const noexcept
	{
		return this->materials_;
	}

	void
	MaterialComponent::initMaterials(const std::vector<tinyobj::material_t>& materials, std::string_view rootPath)
	{
		for (auto& it : materials)
		{
			auto material = std::make_shared<octoon::material::Material>();
			material->setName(it.name);
			material->set(MATKEY_PATH, std::string(rootPath));
			material->set(MATKEY_COLOR_DIFFUSE, octoon::math::float3::One);
			material->set(MATKEY_TEXTURE_DIFFUSE, it.diffuse_texname);

			this->materials_.emplace_back(std::move(material));
		}
	}

	void
	MaterialComponent::onDrop(std::string_view path) noexcept
	{
		auto ext = path.substr(path.find_last_of("."));
		if (ext == ".mtl")
			this->loadMaterial(path);
	}
}