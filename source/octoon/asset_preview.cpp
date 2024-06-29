#include <octoon/asset_preview.h>
#include <octoon/asset_importer.h>
#include <octoon/asset_database.h>
#include <octoon/texture/texture.h>
#include <octoon/material/mesh_standard_material.h>
#include <octoon/animation/animation.h>
#include <octoon/mesh_animation_component.h>
#include <octoon/skinned_mesh_renderer_component.h>
#include <octoon/environment_light_component.h>
#include <octoon/transform_component.h>
#include <octoon/PMREM_loader.h>
#include <octoon/video/renderer.h>
#include <octoon/mesh/sphere_mesh.h>

#include <fstream>
#include <filesystem>

namespace octoon
{
	OctoonImplementSingleton(AssetPreview)

	AssetPreview::AssetPreview() noexcept
		: previewWidth_(256)
		, previewHeight_(256)
	{
	}

	AssetPreview::~AssetPreview() noexcept
	{
		this->close();
	}

	void
	AssetPreview::open() noexcept(false)
	{
		this->initRenderScene();
		this->initMaterialScene();
	}

	void
	AssetPreview::close() noexcept
	{
		camera_.reset();
		geometry_.reset();
		directionalLight_.reset();
		environmentLight_.reset();
		scene_.reset();
		framebuffer_.reset();

		materialCamera_.reset();
		materialGeometry_.reset();
		materialDirectionalLight_.reset();
		materialEnvironmentLight_.reset();
		materialScene_.reset();
	}

	std::shared_ptr<Texture>
	AssetPreview::getAssetPreview(const std::shared_ptr<Texture>& texture)
	{
		auto width = texture->width();
		auto height = texture->height();
		auto data = (float*)texture->data();
		auto format = texture->format();

		if (format == Format::R32G32B32SFloat)
		{
			Texture previewTexutre(Format::R8G8B8SRGB, width, height);

			auto size = width * height * 3;
			auto pixels = previewTexutre.data();

			for (std::size_t i = 0; i < size; i += 3)
			{
				pixels[i] = static_cast<std::uint8_t>(std::clamp(std::pow(data[i], 1.0f / 2.2f) * 255.0f, 0.0f, 255.0f));
				pixels[i + 1] = static_cast<std::uint8_t>(std::clamp(std::pow(data[i + 1], 1.0f / 2.2f) * 255.0f, 0.0f, 255.0f));
				pixels[i + 2] = static_cast<std::uint8_t>(std::clamp(std::pow(data[i + 2], 1.0f / 2.2f) * 255.0f, 0.0f, 255.0f));
			}

			return std::make_shared<Texture>(previewTexutre.resize(256, 128));
		}
		else
		{
			auto ratio = height / (float)width;
			return std::make_shared<Texture>(texture->resize(256, std::uint32_t(256 * ratio)));
		}
	}

	std::shared_ptr<Texture>
	AssetPreview::getAssetPreview(const std::shared_ptr<Material>& material)
	{
		assert(materialScene_);

		auto renderer = Renderer::instance();
		if (renderer)
		{
			materialGeometry_->setMaterial(material);
			renderer->render(materialScene_);
			material->setDirty(true);

			auto colorTexture = materialFramebuffer_->getFramebufferDesc().getColorAttachment(0).getBindingTexture();
			auto width = colorTexture->getTextureDesc().getWidth();
			auto height = colorTexture->getTextureDesc().getHeight();

			std::uint8_t* data;
			if (colorTexture->map(0, 0, width, height, 0, (void**)&data))
			{
				auto texture = std::make_shared<Texture>(Format::R8G8B8A8SRGB, width, height);
				std::memcpy(texture->data(), data, width * height * 4);

				colorTexture->unmap();

				return texture;
			}
		}

		return nullptr;
	}

	std::shared_ptr<Texture>
	AssetPreview::getAssetPreview(const std::shared_ptr<GameObject>& gameObject)
	{
		auto mf = gameObject->getComponent<MeshFilterComponent>();
		if (mf)
			geometry_->setMesh(mf->getMesh());

		auto meshRenderer = gameObject->getComponent<MeshRendererComponent>();
		if (meshRenderer)
			geometry_->setMaterials(meshRenderer->getMaterials());

		bool found = false;

		auto smr = gameObject->getComponent<SkinnedMeshRendererComponent>();
		if (smr)
		{
			for (auto& bone : smr->getBones())
			{
				if (std::strcmp(bone->getName().c_str(), (char*)u8"×óÄ¿") == 0 || std::strcmp(bone->getName().c_str(), (char*)u8"ÓÒÄ¿") == 0)
				{
					found = true;
					auto position = bone->getComponent<TransformComponent>()->getTranslate();
					camera_->setFov(23.9f);
					camera_->setFar(100.0f);
					camera_->setTransform(math::makeLookatRH(math::float3(0, position.y, 10), math::float3(0, position.y, 0), -math::float3::UnitY));
					break;
				}
			}
		}

		if (!found)
		{
			auto mesh = geometry_->getMesh();
			if (mesh)
			{
				auto& bounds = mesh->getBoundingBoxAll();
				auto size = bounds.box().size();
				auto center = bounds.box().center();

				camera_->setFov(45.0f);
				camera_->setFar(math::max3(size) * 2.0f);
				camera_->setTransform(math::makeLookatRH(math::float3(center.x, center.y, std::abs(bounds.box().min.z) + std::max(size.x, size.y)), math::float3::Zero, -math::float3::UnitY));
			}
		}

		auto renderer = Renderer::instance();
		if (renderer)
		{
			Renderer::instance()->render(scene_);
			geometry_->setDirty(true);
		}

		auto framebufferDesc = framebuffer_->getFramebufferDesc();
		auto width = framebufferDesc.getWidth();
		auto height = framebufferDesc.getHeight();

		auto colorTexture = framebufferDesc.getColorAttachment(0).getBindingTexture();

		std::uint8_t* data;
		if (colorTexture->map(0, 0, framebufferDesc.getWidth(), framebufferDesc.getHeight(), 0, (void**)&data))
		{
			auto texture = std::make_shared<Texture>(Format::R8G8B8A8SRGB, width, height);
			std::memcpy(texture->data(), data, width * height * 4);

			colorTexture->unmap();

			return texture;
		}

		return nullptr;
	}

	void
	AssetPreview::initMaterialScene() noexcept(false)
	{
		auto renderer = Renderer::instance();
		if (renderer)
		{
			std::uint32_t width = previewWidth_;
			std::uint32_t height = previewHeight_;

			GraphicsTextureDesc textureDesc;
			textureDesc.setSize(width, height);
			textureDesc.setTexDim(TextureDimension::Texture2D);
			textureDesc.setTexFormat(GraphicsFormat::R8G8B8A8UNorm);
			auto colorTexture = renderer->getGraphicsDevice()->createTexture(textureDesc);
			if (!colorTexture)
				throw std::runtime_error("createTexture() failed");

			GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setSize(width, height);
			depthTextureDesc.setTexDim(TextureDimension::Texture2D);
			depthTextureDesc.setTexFormat(GraphicsFormat::D16UNorm);
			auto depthTexture = renderer->getGraphicsDevice()->createTexture(depthTextureDesc);
			if (!depthTexture)
				throw std::runtime_error("createTexture() failed");

			GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::ColorAttachmentOptimal, GraphicsFormat::R8G8B8A8UNorm));
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::DepthStencilAttachmentOptimal, GraphicsFormat::D16UNorm));

			GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(width);
			framebufferDesc.setHeight(height);
			framebufferDesc.setFramebufferLayout(renderer->getGraphicsDevice()->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture, 0, 0));
			framebufferDesc.addColorAttachment(GraphicsAttachmentBinding(colorTexture, 0, 0));

			materialFramebuffer_ = renderer->getGraphicsDevice()->createFramebuffer(framebufferDesc);
			if (!materialFramebuffer_)
				throw std::runtime_error("createFramebuffer() failed");

			materialCamera_ = std::make_shared<PerspectiveCamera>(60.0f, 1.0f, 100.0f);
			materialCamera_->setClearColor(math::float4::Zero);
			materialCamera_->setClearFlags(ClearFlagBits::AllBit);
			materialCamera_->setFramebuffer(materialFramebuffer_);
			materialCamera_->setTransform(math::makeLookatRH(math::float3(0, 0, 1), math::float3::Zero, math::float3::UnitY));

			materialGeometry_ = std::make_shared<Geometry>();
			materialGeometry_->setMesh(std::make_shared<SphereMesh>(0.5f));

			math::Quaternion q1;
			q1.makeRotation(math::float3::UnitX, math::PI / 2.75f);
			math::Quaternion q2;
			q2.makeRotation(math::float3::UnitY, math::PI / 4.6f);

			materialDirectionalLight_ = std::make_shared<DirectionalLight>();
			materialDirectionalLight_->setColor(math::float3(1, 1, 1));
			materialDirectionalLight_->setTransform(math::float4x4(q1 * q2));

			materialEnvironmentLight_ = std::make_shared<EnvironmentLight>();
			materialEnvironmentLight_->setEnvironmentMap(PMREMLoader::load("../../system/hdri/Ditch-River_1k.hdr"));

			materialScene_ = std::make_unique<RenderScene>();
			materialScene_->addRenderObject(materialCamera_.get());
			materialScene_->addRenderObject(materialDirectionalLight_.get());
			materialScene_->addRenderObject(materialEnvironmentLight_.get());
			materialScene_->addRenderObject(materialGeometry_.get());
		}
	}

	void
	AssetPreview::initRenderScene() noexcept(false)
	{
		auto renderer = Renderer::instance();
		if (renderer)
		{
			std::uint32_t width = previewWidth_;
			std::uint32_t height = previewHeight_;

			GraphicsTextureDesc textureDesc;
			textureDesc.setSize(width, height);
			textureDesc.setTexDim(TextureDimension::Texture2D);
			textureDesc.setTexFormat(GraphicsFormat::R8G8B8A8UNorm);
			auto colorTexture = renderer->getGraphicsDevice()->createTexture(textureDesc);
			if (!colorTexture)
				throw std::runtime_error("createTexture() failed");

			GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setSize(width, height);
			depthTextureDesc.setTexDim(TextureDimension::Texture2D);
			depthTextureDesc.setTexFormat(GraphicsFormat::D16UNorm);
			auto depthTexture = renderer->getGraphicsDevice()->createTexture(depthTextureDesc);
			if (!depthTexture)
				throw std::runtime_error("createTexture() failed");

			GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::ColorAttachmentOptimal, GraphicsFormat::R8G8B8A8UNorm));
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::DepthStencilAttachmentOptimal, GraphicsFormat::D16UNorm));

			GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(width);
			framebufferDesc.setHeight(height);
			framebufferDesc.setFramebufferLayout(renderer->getGraphicsDevice()->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture, 0, 0));
			framebufferDesc.addColorAttachment(GraphicsAttachmentBinding(colorTexture, 0, 0));

			framebuffer_ = renderer->getGraphicsDevice()->createFramebuffer(framebufferDesc);
			if (!framebuffer_)
				throw std::runtime_error("createFramebuffer() failed");

			camera_ = std::make_shared<FilmCamera>(23.9f, 1.0f, 100.0f);
			camera_->setClearColor(math::float4::Zero);
			camera_->setClearFlags(ClearFlagBits::AllBit);
			camera_->setFramebuffer(framebuffer_);

			geometry_ = std::make_shared<Geometry>();
			geometry_->setMesh(std::make_shared<SphereMesh>(0.5f));

			math::Quaternion q1;
			q1.makeRotation(math::float3::UnitX, math::PI / 2.75f);
			math::Quaternion q2;
			q2.makeRotation(math::float3::UnitY, math::PI / 4.6f);

			directionalLight_ = std::make_shared<DirectionalLight>();
			directionalLight_->setColor(math::float3(1, 1, 1));
			directionalLight_->setTransform(math::float4x4(q1 * q2));

			environmentLight_ = std::make_shared<EnvironmentLight>();
			environmentLight_->setColor(math::float3::One * 0.9f);

			scene_ = std::make_unique<RenderScene>();
			scene_->addRenderObject(camera_.get());
			scene_->addRenderObject(environmentLight_.get());
			scene_->addRenderObject(geometry_.get());
		}
	}
}