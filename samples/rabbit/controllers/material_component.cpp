#include "material_component.h"

#include "../rabbit_profile.h"
#include "../rabbit_behaviour.h"

#include <octoon/mdl_loader.h>

#include <filesystem>
#include <fstream>

#include <qimage.h>
#include <qstring.h>
#include <quuid.h>

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
	MaterialComponent::importMtl(std::string_view path) noexcept(false)
	{
		std::ifstream stream(QString::fromStdString(std::string(path)).toStdWString());
		if (stream)
		{
			std::map<std::string, int> materialMap;
			std::vector<tinyobj::material_t> materials;
			auto err = tinyobj::LoadMtl(materialMap, materials, stream);
			auto rootPath = path.substr(0, path.find_last_of("/") + 1);

			for (auto& it : materials)
			{
				auto id = QUuid::createUuid().toString();
				auto uuid = id.toStdString().substr(1, id.length() - 2);
				auto directory = this->getModel()->path + "/" + uuid;

				if (std::filesystem::create_directory(directory))
				{
					try
					{
						auto from = QString::fromStdString(std::string(rootPath) + it.diffuse_texname).toStdWString();
						auto to = QString::fromStdString(directory + "/" + it.diffuse_texname).toStdWString();
						std::filesystem::copy_file(from, to);
					}
					catch (std::filesystem::filesystem_error & e)
					{
						std::cout << "Could not copy sandbox/abc: " << e.what() << '\n';
					}

					std::ofstream ifs(directory + "/data.json");
					if (ifs)
					{
						nlohmann::json item;
						item["uuid"] = uuid;
						item["name"] = it.name;
						item["color"] = { 1.0, 1.0, 1.0 };
						item["map"] = directory + "/" + it.diffuse_texname;

						if (!item["map"].is_null()) {
							auto image = QImage();
							image.load(QString::fromStdString(std::string(rootPath) + it.diffuse_texname));
							image.scaled(QSize(130, 130)).save(QString::fromStdString(directory + "/preview.jpeg"));

							item["preview"] = directory + "/preview.jpeg";
						}

						auto data = item.dump();
						ifs.write(data.c_str(), data.size());

						this->materialList_[uuid] = item;
					}
				}
			}

			this->sendMessage("editor:material:change");
		}
	}

	void
	MaterialComponent::importMdl(std::string_view path) noexcept(false)
	{
		octoon::io::ifstream stream(QString::fromStdString(std::string(path)).toStdWString());
		if (stream)
		{
			octoon::MDLLoader loader;
			loader.load("resource", stream);
			auto& materials = loader.getMaterials();

			for (auto& mat : materials)
			{
				auto id = QUuid::createUuid().toString();
				auto uuid = id.toStdString().substr(1, id.length() - 2);

				auto& color = mat->getColor();
				auto& colorMap = mat->getColorTexture();

				nlohmann::json item;
				item["uuid"] = uuid;
				item["name"] = mat->getName();
				item["color"] =  { color.x, color.y, color.z};

				if (colorMap)
					item["map"] = colorMap->getTextureDesc().getName();

				this->materialList_[uuid] = item;
				this->materials_[std::string(uuid)] = mat;
			}

			this->sendMessage("editor:material:change");
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
}