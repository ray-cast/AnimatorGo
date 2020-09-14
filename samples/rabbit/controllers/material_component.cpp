#include "material_component.h"

#include "../rabbit_profile.h"
#include "../rabbit_behaviour.h"

#include <octoon/mdl_loader.h>
#include <octoon/PMREM_loader.h>
#include <octoon/texture_loader.h>
#include <octoon/environment_light_component.h>

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
			std::uint32_t width = 256;
			std::uint32_t height = 256;

			octoon::hal::GraphicsTextureDesc textureDesc;
			textureDesc.setSize(width, height);
			textureDesc.setTexDim(octoon::hal::GraphicsTextureDim::Texture2D);
			textureDesc.setTexFormat(octoon::hal::GraphicsFormat::R8G8B8UNorm);
			auto colorTexture = octoon::video::Renderer::instance()->createTexture(textureDesc);
			if (!colorTexture)
				throw std::runtime_error("createTexture() failed");

			octoon::hal::GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setSize(width, height);
			depthTextureDesc.setTexDim(octoon::hal::GraphicsTextureDim::Texture2D);
			depthTextureDesc.setTexFormat(octoon::hal::GraphicsFormat::D16UNorm);
			auto depthTexture = octoon::video::Renderer::instance()->createTexture(depthTextureDesc);
			if (!depthTexture)
				throw std::runtime_error("createTexture() failed");

			octoon::hal::GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(octoon::hal::GraphicsAttachmentLayout(0, octoon::hal::GraphicsImageLayout::ColorAttachmentOptimal, octoon::hal::GraphicsFormat::R8G8B8UNorm));
			framebufferLayoutDesc.addComponent(octoon::hal::GraphicsAttachmentLayout(1, octoon::hal::GraphicsImageLayout::DepthStencilAttachmentOptimal, octoon::hal::GraphicsFormat::D16UNorm));

			octoon::hal::GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(width);
			framebufferDesc.setHeight(height);
			framebufferDesc.setFramebufferLayout(octoon::video::Renderer::instance()->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(octoon::hal::GraphicsAttachmentBinding(depthTexture, 0, 0));
			framebufferDesc.addColorAttachment(octoon::hal::GraphicsAttachmentBinding(colorTexture, 0, 0));

			framebuffer_ = octoon::video::Renderer::instance()->createFramebuffer(framebufferDesc);
			if (!framebuffer_)
				throw std::runtime_error("createFramebuffer() failed");

			camera_ = std::make_shared<octoon::camera::PerspectiveCamera>(60, 1, 100);
			camera_->setClearColor(octoon::math::float4(0.117, 0.117, 0.117, 1));
			camera_->setClearFlags(octoon::hal::GraphicsClearFlagBits::AllBit);
			camera_->setFramebuffer(framebuffer_);
			camera_->setTransform(octoon::math::makeLookatRH(octoon::math::float3(0, 0, 1), octoon::math::float3::Zero, octoon::math::float3::UnitY));

			geometry_ = std::make_shared<octoon::geometry::Geometry>();
			geometry_->setMesh(octoon::mesh::SphereMesh::create(0.5));

			octoon::math::Quaternion q1;
			q1.makeRotation(octoon::math::float3::UnitX, octoon::math::PI / 2.75);
			octoon::math::Quaternion q2;
			q2.makeRotation(octoon::math::float3::UnitY, octoon::math::PI / 4.6);

			light_ = std::make_shared<octoon::light::DirectionalLight>();
			light_->setColor(octoon::math::float3(1, 1, 1));
			light_->setTransform(octoon::math::float4x4(q1 * q2));

			envlight_ = std::make_shared<octoon::light::EnvironmentLight>();
			envlight_->setEnvironmentMap(octoon::PMREMLoader::load("../../system/hdri/Ditch-River_2k.hdr"));

			scene_ = std::make_unique<octoon::video::RenderScene>();
			scene_->addRenderObject(camera_.get());
			scene_->addRenderObject(light_.get());
			scene_->addRenderObject(envlight_.get());
			scene_->addRenderObject(geometry_.get());

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
				auto& colorMap = mat->getColorMap();

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
				standard->setColorMap(octoon::TextureLoader::load(mat["map"].get<nlohmann::json::string_t>()));

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
	MaterialComponent::repaintMaterial(const std::shared_ptr<octoon::material::Material>& material, QPixmap& pixmap)
	{
		assert(material);

		if (scene_)
		{
			geometry_->setMaterial(material);
			octoon::video::Renderer::instance()->render(*scene_);

			auto framebufferDesc = framebuffer_->getFramebufferDesc();
			auto width = framebufferDesc.getWidth();
			auto height = framebufferDesc.getHeight();

			auto colorTexture = framebufferDesc.getColorAttachment(0).getBindingTexture();

			std::uint8_t* data;
			if (colorTexture->map(0, 0, framebufferDesc.getWidth(), framebufferDesc.getHeight(), 0, (void**)&data))
			{
				QImage image(width, height, QImage::Format_RGB888);
				for (std::uint32_t y = 0; y < height; y++)
				{
					for (std::uint32_t x = 0; x < width; x++)
					{
						auto index = (y * height + x) * 3;
						image.setPixelColor((int)x, (int)y, QColor::fromRgb(data[index], data[index + 1], data[index + 2]));
					}
				}

				pixmap.convertFromImage(image);
				pixmap = pixmap.scaled(128, 128, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

				colorTexture->unmap();
			}
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