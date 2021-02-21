#include <octoon/video/forward_pipeline.h>
#include <octoon/video/forward_scene.h>

#include <octoon/runtime/except.h>

#include <octoon/hal/graphics_device.h>
#include <octoon/hal/graphics_texture.h>
#include <octoon/hal/graphics_framebuffer.h>

#include <octoon/camera/ortho_camera.h>
#include <octoon/camera/perspective_camera.h>

#include <octoon/light/ambient_light.h>
#include <octoon/light/directional_light.h>
#include <octoon/light/point_light.h>
#include <octoon/light/spot_light.h>
#include <octoon/light/disk_light.h>
#include <octoon/light/rectangle_light.h>
#include <octoon/light/environment_light.h>
#include <octoon/light/tube_light.h>

#include <octoon/mesh/plane_mesh.h>

#include <octoon/material/mesh_copy_material.h>
#include <octoon/material/mesh_depth_material.h>
#include <octoon/material/mesh_standard_material.h>

namespace octoon
{
	ForwardPipeline::ForwardPipeline() noexcept
		: width_(0)
		, height_(0)
	{
		screenGeometry_ = std::make_shared<Geometry>();
		screenGeometry_->setMesh(octoon::PlaneMesh::create(2.0f, 2.0f));
	}

	ForwardPipeline::~ForwardPipeline() noexcept
	{
	}

	const hal::GraphicsFramebufferPtr&
	ForwardPipeline::getFramebuffer() const noexcept
	{
		return this->fbo_;
	}

	void
	ForwardPipeline::clear(const math::float4& val)
	{
	}

	void
	ForwardPipeline::setupFramebuffers(std::uint32_t w, std::uint32_t h) except
	{
		if (width_ == w && height_ == h)
			return;

		this->width_ = w;
		this->height_ = h;

		hal::GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
		framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(0, hal::GraphicsImageLayout::ColorAttachmentOptimal, hal::GraphicsFormat::R32G32B32SFloat));
		framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(1, hal::GraphicsImageLayout::DepthStencilAttachmentOptimal, hal::GraphicsFormat::X8_D24UNormPack32));

		try
		{
			hal::GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexMultisample(4);
			colorTextureDesc.setTexDim(hal::GraphicsTextureDim::Texture2DMultisample);
			colorTextureDesc.setTexFormat(hal::GraphicsFormat::R32G32B32SFloat);
			colorTexture_ = this->cmd_->createTexture(colorTextureDesc);
			if (!colorTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexMultisample(4);
			depthTextureDesc.setTexDim(hal::GraphicsTextureDim::Texture2DMultisample);
			depthTextureDesc.setTexFormat(hal::GraphicsFormat::X8_D24UNormPack32);
			depthTexture_ = this->cmd_->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setFramebufferLayout(this->cmd_->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(hal::GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(hal::GraphicsAttachmentBinding(colorTexture_, 0, 0));

			fbo_ = this->cmd_->createFramebuffer(framebufferDesc);
			if (!fbo_)
				throw runtime::runtime_error::create("createFramebuffer() failed");

			hal::GraphicsTextureDesc colorTextureDesc2;
			colorTextureDesc2.setWidth(w);
			colorTextureDesc2.setHeight(h);
			colorTextureDesc2.setTexDim(hal::GraphicsTextureDim::Texture2D);
			colorTextureDesc2.setTexFormat(hal::GraphicsFormat::R32G32B32SFloat);
			colorTexture2_ = this->cmd_->createTexture(colorTextureDesc2);
			if (!colorTexture2_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsTextureDesc depthTextureDesc2;
			depthTextureDesc2.setWidth(w);
			depthTextureDesc2.setHeight(h);
			depthTextureDesc2.setTexDim(hal::GraphicsTextureDim::Texture2D);
			depthTextureDesc2.setTexFormat(hal::GraphicsFormat::X8_D24UNormPack32);
			depthTexture2_ = this->cmd_->createTexture(depthTextureDesc2);
			if (!depthTexture2_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsFramebufferDesc framebufferDesc2;
			framebufferDesc2.setWidth(w);
			framebufferDesc2.setHeight(h);
			framebufferDesc2.setFramebufferLayout(this->cmd_->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc2.setDepthStencilAttachment(hal::GraphicsAttachmentBinding(depthTexture2_, 0, 0));
			framebufferDesc2.addColorAttachment(hal::GraphicsAttachmentBinding(colorTexture2_, 0, 0));

			fbo2_ = this->cmd_->createFramebuffer(framebufferDesc2);
			if (!fbo2_)
				throw runtime::runtime_error::create("createFramebuffer() failed");
		}
		catch (...)
		{
			hal::GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexFormat(hal::GraphicsFormat::R32G32B32SFloat);
			colorTexture_ = this->cmd_->createTexture(colorTextureDesc);
			if (!colorTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexFormat(hal::GraphicsFormat::X8_D24UNormPack32);
			depthTexture_ = this->cmd_->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setFramebufferLayout(this->cmd_->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(hal::GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(hal::GraphicsAttachmentBinding(colorTexture_, 0, 0));

			fbo_ = this->cmd_->createFramebuffer(framebufferDesc);
			if (!fbo_)
				throw runtime::runtime_error::create("createFramebuffer() failed");
		}
	}

	void
	ForwardPipeline::setMaterial(const ForwardScene& scene, const std::shared_ptr<Material>& material, const Camera& camera, const Geometry& geometry)
	{
		assert(material);
		
		auto& pipeline = scene.materials_.at(material.get());
		pipeline->update(scene, camera, geometry);

		this->cmd_->setRenderPipeline(pipeline->getPipeline());
		this->cmd_->setDescriptorSet(pipeline->getDescriptorSet());
	}

	void
	ForwardPipeline::renderObject(const ForwardScene& scene, const Geometry& geometry, const Camera& camera, const std::shared_ptr<Material>& overrideMaterial) noexcept
	{
		if (camera.getLayer() != geometry.getLayer())
			return;

		if (geometry.getVisible())
		{
			for (std::size_t i = 0; i < geometry.getMaterials().size(); i++)
			{
				auto mesh = geometry.getMesh();
				auto material = geometry.getMaterials()[i];
				if (material && overrideMaterial)
				{
					if (material->getPrimitiveType() == overrideMaterial->getPrimitiveType())
						material = overrideMaterial;
				}

				if (mesh && material)
				{
					this->setMaterial(scene, overrideMaterial ? overrideMaterial : material, camera, geometry);
					this->renderBuffer(scene, mesh, i);
				}
			}
		}
	}

	void
	ForwardPipeline::renderObjects(const ForwardScene& scene, const std::vector<Geometry*>& geometries, const Camera& camera, const std::shared_ptr<Material>& overrideMaterial) noexcept
	{
		for (auto& geometry : geometries)
			this->renderObject(scene , *geometry, camera, overrideMaterial);
	}

	void
	ForwardPipeline::renderShadowMaps(const ForwardScene& scene, const std::vector<Light*>& lights, const std::vector<Geometry*>& geometries) noexcept
	{
		for (auto& light : lights)
		{
			if (!light->getVisible())
				continue;

			std::uint32_t faceCount = 0;
			std::shared_ptr<Camera> camera;

			if (light->isA<DirectionalLight>())
			{
				auto directionalLight = light->cast<DirectionalLight>();
				if (directionalLight->getShadowEnable())
				{
					faceCount = 1;
					camera = directionalLight->getCamera();
				}
			}
			else if (light->isA<SpotLight>())
			{
				auto spotLight = light->cast<SpotLight>();
				if (spotLight->getShadowEnable())
				{
					faceCount = 1;
					camera = spotLight->getCamera();
				}
			}
			else if (light->isA<PointLight>())
			{
				auto pointLight = light->cast<PointLight>();
				if (pointLight->getShadowEnable())
				{
					faceCount = 6;
					camera = pointLight->getCamera();
				}
			}

			for (std::uint32_t face = 0; face < faceCount; face++)
			{
				auto framebuffer = camera->getFramebuffer() ? camera->getFramebuffer() : fbo_;

				this->cmd_->setFramebuffer(framebuffer);
				this->cmd_->clearFramebuffer(0, camera->getClearFlags(), camera->getClearColor(), 1.0f, 0);
				this->cmd_->setViewport(0, camera->getPixelViewport());

				for (auto& geometry : geometries)
					this->renderObject(scene, *geometry, *camera, scene.depthMaterial);

				if (camera->getRenderToScreen())
				{
					auto& v = camera->getPixelViewport();
					this->cmd_->blitFramebuffer(framebuffer, v, nullptr, v);
				}

				auto& swapFramebuffer = camera->getSwapFramebuffer();
				if (swapFramebuffer && framebuffer)
				{
					math::float4 v1(0, 0, (float)framebuffer->getFramebufferDesc().getWidth(), (float)framebuffer->getFramebufferDesc().getHeight());
					math::float4 v2(0, 0, (float)swapFramebuffer->getFramebufferDesc().getWidth(), (float)swapFramebuffer->getFramebufferDesc().getHeight());
					this->cmd_->blitFramebuffer(framebuffer, v1, swapFramebuffer, v2);
				}

				this->cmd_->discardFramebuffer(framebuffer, hal::GraphicsClearFlagBits::DepthStencilBit);
			}
		}
	}

	void
	ForwardPipeline::render(const std::shared_ptr<ScriptableRenderContext>& context, const CompiledScene& scene)
	{
		cmd_ = context;
		auto compiled = dynamic_cast<const ForwardScene*>(&scene);
		auto vp = compiled->camera->getPixelViewport();
		this->renderTile(scene, math::int2((int)vp.x, (int)vp.y), math::int2((int)vp.width, (int)vp.height));
	}

	void
	ForwardPipeline::renderTile(const CompiledScene& scene, const math::int2& tileOrigin, const math::int2& tileSize)
	{
		auto compiled = dynamic_cast<const ForwardScene*>(&scene);

		this->renderShadowMaps(*compiled, compiled->lights, compiled->geometries);

		auto camera = compiled->camera;
		auto viewport = math::float4((float)tileOrigin.x, (float)tileOrigin.y, (float)tileSize.x, (float)tileSize.y);
		auto& framebuffer = camera->getFramebuffer();
		auto& swapFramebuffer = camera->getSwapFramebuffer();

		auto fbo = camera->getFramebuffer();
		if (!fbo)
		{
			auto vp = compiled->camera->getPixelViewport();
			this->setupFramebuffers((int)vp.width, (int)vp.height);
			fbo = fbo_;
		}

		this->cmd_->setFramebuffer(fbo);
		this->cmd_->setViewport(0, viewport);
		this->cmd_->clearFramebuffer(0, camera->getClearFlags(), camera->getClearColor(), 1.0f, 0);

		this->renderObjects(*compiled, compiled->geometries, *camera, this->overrideMaterial_);

		if (framebuffer && swapFramebuffer)
		{
			math::float4 v1(0, 0, (float)framebuffer->getFramebufferDesc().getWidth(), (float)framebuffer->getFramebufferDesc().getHeight());
			math::float4 v2(0, 0, (float)swapFramebuffer->getFramebufferDesc().getWidth(), (float)swapFramebuffer->getFramebufferDesc().getHeight());
			this->cmd_->blitFramebuffer(framebuffer, v1, swapFramebuffer, v2);
		}

		if (camera->getRenderToScreen())
		{
			this->cmd_->setFramebuffer(nullptr);
			this->cmd_->clearFramebuffer(0, hal::GraphicsClearFlagBits::AllBit, math::float4::Zero, 1.0f, 0);

			if (!fbo->getFramebufferDesc().getColorAttachments().empty())
			{
				auto texture = fbo->getFramebufferDesc().getColorAttachment().getBindingTexture();
				if (texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Texture2DMultisample)
				{
					if (framebuffer && swapFramebuffer)
					{
						this->cmd_->blitFramebuffer(swapFramebuffer, viewport, nullptr, viewport);
						this->cmd_->discardFramebuffer(swapFramebuffer, hal::GraphicsClearFlagBits::AllBit);
					}
					else if (fbo == fbo_)
					{
						this->cmd_->blitFramebuffer(fbo, viewport, fbo2_, viewport);
						this->cmd_->discardFramebuffer(fbo, hal::GraphicsClearFlagBits::AllBit);
						this->cmd_->blitFramebuffer(fbo2_, viewport, nullptr, viewport);
						this->cmd_->discardFramebuffer(fbo2_, hal::GraphicsClearFlagBits::AllBit);
					}
				}
				else
				{
					this->cmd_->blitFramebuffer(fbo, viewport, nullptr, viewport);
					this->cmd_->discardFramebuffer(fbo, hal::GraphicsClearFlagBits::AllBit);
				}
			}
		}
	}

	void
	ForwardPipeline::renderBuffer(const ForwardScene& scene, const std::shared_ptr<Mesh>& mesh, std::size_t subset)
	{
		auto& buffer = scene.buffers_.at(mesh.get());
		this->cmd_->setVertexBufferData(0, buffer->getVertexBuffer(), 0);
		this->cmd_->setIndexBufferData(buffer->getIndexBuffer(), 0, hal::GraphicsIndexType::UInt32);

		if (buffer->getIndexBuffer())
			this->cmd_->drawIndexed((std::uint32_t)buffer->getNumIndices(subset), 1, (std::uint32_t)buffer->getStartIndices(subset), 0, 0);
		else
			this->cmd_->draw((std::uint32_t)buffer->getNumVertices(), 1, 0, 0);
	}
}