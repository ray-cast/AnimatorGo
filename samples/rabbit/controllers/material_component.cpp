#include "material_component.h"
#include <fstream>
#include "../rabbit_profile.h"
#include "../rabbit_behaviour.h"
#include <qstring.h>
#include <quuid>
#include <octoon/game_base_features.h>
#include <filesystem>
#include <fstream>
#include <qimage.h>

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
		try
		{
			std::ifstream ifs(this->getModel()->path + "/material.json");
			if (ifs)
			{
				auto uuids = nlohmann::json::parse(ifs);
				for (auto& uuid : uuids)
				{
					try
					{
						auto path = this->getModel()->path + "/" + uuid.get<nlohmann::json::string_t>() + "/data.json";
						this->initMaterials(path);
					}
					catch (...)
					{
					}
				}
			}
		}
		catch (...)
		{
		}
	}

	void
	MaterialComponent::onDisable() noexcept
	{
		std::ofstream ifs(this->getModel()->path + "/material.json");
		if (ifs)
		{
			nlohmann::json uuids;
			for (auto& mat : this->materialList_)
				uuids.push_back(mat.first);

			auto data = uuids.dump();
			ifs.write(data.c_str(), data.size());
		}
	}

	const std::map<std::string, nlohmann::json, std::less<>>&
	MaterialComponent::getMaterialList() const noexcept
	{
		return this->materialList_;
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

	const std::shared_ptr<octoon::material::Material>
	MaterialComponent::getMaterial(std::string_view uuid) noexcept
	{
		auto material = this->materials_.find(uuid);
		if (material == this->materials_.end())
		{
			auto it = this->materialList_.find(uuid);
			if (it != this->materialList_.end())
			{
				auto mat = (*it).second;

				auto standard = std::make_shared<octoon::material::MeshStandardMaterial>();
				standard->setName(mat["name"].get<nlohmann::json::string_t>());
				standard->setColor(octoon::math::float3::One);
				standard->setColorTexture(octoon::TextureLoader::load(mat["map"].get<nlohmann::json::string_t>()));

				this->materials_[std::string(uuid)] = standard;

				return standard;
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			return (*material).second;
		}
	}

	void
	MaterialComponent::initMaterials(std::string_view path)
	{
		std::ifstream ifs(path);
		if (ifs)
		{
			auto json = nlohmann::json::parse(ifs);
			this->materialList_[json["uuid"]] = json;
		}
	}

	void
	MaterialComponent::initMaterials(const std::vector<tinyobj::material_t>& materials, std::string_view rootPath)
	{
		for (auto& it : materials)
		{
			auto id = QUuid::createUuid().toString();
			auto uuid = id.toStdString().substr(1, id.length() - 2);
			auto path = this->getModel()->path + "/" + uuid;

			if (std::filesystem::create_directory(path))
			{
				try {
					auto from = QString::fromStdString(std::string(rootPath) + it.diffuse_texname).toStdWString();
					auto to = QString::fromStdString(path + "/" + it.diffuse_texname).toStdWString();
					std::filesystem::copy_file(from, to);
				}
				catch (std::filesystem::filesystem_error & e) {
					std::cout << "Could not copy sandbox/abc: " << e.what() << '\n';
				}

				std::ofstream ifs(path + "/data.json");
				if (ifs)
				{
					nlohmann::json item;
					item["uuid"] = uuid;
					item["name"] = it.name;
					item["color"] = { 1.0, 1.0, 1.0 };
					item["map"] = path + "/" + it.diffuse_texname;

					if (!item["map"].is_null()) {
						auto image = QImage();
						image.load(QString::fromStdString(std::string(rootPath) + it.diffuse_texname));
						image.scaled(QSize(130, 130)).save(QString::fromStdString(path + "/preview.jpeg"));

						item["preview"] = path + "/preview.jpeg";
					}

					auto data = item.dump();
					ifs.write(data.c_str(), data.size());

					this->materialList_[uuid] = item;
				}
			}
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