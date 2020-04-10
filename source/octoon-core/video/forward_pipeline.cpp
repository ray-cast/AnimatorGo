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

#include <octoon/material/mesh_depth_material.h>
#include <octoon/material/mesh_standard_material.h>

namespace octoon::video
{
	ForwardPipeline::ForwardPipeline(const hal::GraphicsContextPtr& context) noexcept
		: context_(context)
		, depthMaterial_(material::MeshDepthMaterial::create())
	{
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
	ForwardPipeline::renderObject(ForwardScene& scene, const geometry::Geometry& geometry, const camera::Camera& camera, const std::shared_ptr<material::Material>& overrideMaterial) noexcept
	{
		if (camera.getLayer() != geometry.getLayer())
			return;

		if (geometry.getVisible())
		{
			for (std::size_t i = 0; i < geometry.getMaterials().size(); i++)
			{
				if (!this->setProgram(scene, overrideMaterial ? overrideMaterial : geometry.getMaterials()[i], camera, geometry))
					return;

				if (!this->setBuffer(scene, geometry.getMesh(), i))
					return;

				auto indices = currentBuffer_->getNumIndices(i);
				if (indices > 0)
					this->context_->drawIndexed((std::uint32_t)indices, 1, 0, 0, 0);
				else
					this->context_->draw((std::uint32_t)currentBuffer_->getNumVertices(), 1, 0, 0);
			}
		}
	}

	void
	ForwardPipeline::renderObjects(ForwardScene& scene, const std::vector<geometry::Geometry*>& geometries, const camera::Camera& camera, const std::shared_ptr<material::Material>& overrideMaterial) noexcept
	{
		for (auto& geometry : geometries)
			this->renderObject(scene , *geometry, camera, overrideMaterial);
	}

	void
	ForwardPipeline::prepareShadowMaps(ForwardScene& scene, const std::vector<light::Light*>& lights, const std::vector<geometry::Geometry*>& geometries) noexcept
	{
		for (auto& light : lights)
		{
			if (!light->getVisible())
				continue;

			std::uint32_t faceCount = 0;
			std::shared_ptr<camera::Camera> camera;

			if (light->isA<light::DirectionalLight>())
			{
				auto directionalLight = light->cast<light::DirectionalLight>();
				if (directionalLight->getShadowEnable())
				{
					faceCount = 1;
					camera = directionalLight->getCamera();
				}
			}
			else if (light->isA<light::SpotLight>())
			{
				auto spotLight = light->cast<light::SpotLight>();
				if (spotLight->getShadowEnable())
				{
					faceCount = 1;
					camera = spotLight->getCamera();
				}
			}
			else if (light->isA<light::PointLight>())
			{
				auto pointLight = light->cast<light::PointLight>();
				if (pointLight->getShadowEnable())
				{
					faceCount = 6;
					camera = pointLight->getCamera();
				}
			}

			for (std::uint32_t face = 0; face < faceCount; face++)
			{
				auto framebuffer = camera->getFramebuffer();

				this->context_->setFramebuffer(framebuffer ? framebuffer : fbo_);
				this->context_->clearFramebuffer(0, camera->getClearFlags(), camera->getClearColor(), 1.0f, 0);
				this->context_->setViewport(0, camera->getPixelViewport());

				for (auto& geometry : geometries)
				{
					if (geometry->getMaterial()->getPrimitiveType() == this->depthMaterial_->getPrimitiveType())
						this->renderObject(scene, *geometry, *camera, this->depthMaterial_);
				}

				if (camera->getRenderToScreen())
				{
					auto& v = camera->getPixelViewport();
					this->context_->blitFramebuffer(framebuffer ? framebuffer : fbo_, v, nullptr, v);
				}

				auto& swapFramebuffer = camera->getSwapFramebuffer();
				if (swapFramebuffer && framebuffer)
				{
					math::float4 v1(0, 0, (float)framebuffer->getFramebufferDesc().getWidth(), (float)framebuffer->getFramebufferDesc().getHeight());
					math::float4 v2(0, 0, (float)swapFramebuffer->getFramebufferDesc().getWidth(), (float)swapFramebuffer->getFramebufferDesc().getHeight());
					this->context_->blitFramebuffer(framebuffer, v1, swapFramebuffer, v2);
				}
			}
		}
	}

	void
	ForwardPipeline::render(CompiledScene& scene) noexcept
	{
		auto compiled = dynamic_cast<ForwardScene&>(scene);
		auto camera = compiled.camera;
		auto framebuffer = camera->getFramebuffer();
		this->context_->setFramebuffer(framebuffer ? framebuffer : fbo_);
		this->context_->clearFramebuffer(0, camera->getClearFlags(), camera->getClearColor(), 1.0f, 0);
		this->context_->setViewport(0, camera->getPixelViewport());

		this->renderObjects(compiled, compiled.geometries, *camera, this->overrideMaterial_);

		if (camera->getRenderToScreen())
		{
			auto& v = camera->getPixelViewport();
			this->context_->blitFramebuffer(framebuffer ? framebuffer : fbo_, v, nullptr, v);
		}

		auto& swapFramebuffer = camera->getSwapFramebuffer();
		if (framebuffer && swapFramebuffer)
		{
			math::float4 v1(0, 0, (float)framebuffer->getFramebufferDesc().getWidth(), (float)framebuffer->getFramebufferDesc().getHeight());
			math::float4 v2(0, 0, (float)swapFramebuffer->getFramebufferDesc().getWidth(), (float)swapFramebuffer->getFramebufferDesc().getHeight());
			this->context_->blitFramebuffer(framebuffer, v1, swapFramebuffer, v2);
		}
	}

	void
	ForwardPipeline::setupFramebuffers(std::uint32_t w, std::uint32_t h) except
	{
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
			colorTexture_ = this->context_->getDevice()->createTexture(colorTextureDesc);
			if (!colorTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexMultisample(4);
			depthTextureDesc.setTexDim(hal::GraphicsTextureDim::Texture2DMultisample);
			depthTextureDesc.setTexFormat(hal::GraphicsFormat::X8_D24UNormPack32);
			depthTexture_ = this->context_->getDevice()->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setFramebufferLayout(this->context_->getDevice()->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(hal::GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(hal::GraphicsAttachmentBinding(colorTexture_, 0, 0));

			fbo_ = this->context_->getDevice()->createFramebuffer(framebufferDesc);
			if (!fbo_)
				throw runtime::runtime_error::create("createFramebuffer() failed");
		}
		catch (...)
		{
			hal::GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexFormat(hal::GraphicsFormat::R8G8B8A8UNorm);
			colorTexture_ = this->context_->getDevice()->createTexture(colorTextureDesc);
			if (!colorTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexFormat(hal::GraphicsFormat::D16UNorm);
			depthTexture_ = this->context_->getDevice()->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setFramebufferLayout(this->context_->getDevice()->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(hal::GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(hal::GraphicsAttachmentBinding(colorTexture_, 0, 0));

			fbo_ = this->context_->getDevice()->createFramebuffer(framebufferDesc);
			if (!fbo_)
				throw runtime::runtime_error::create("createFramebuffer() failed");
		}
	}

	bool
	ForwardPipeline::setBuffer(ForwardScene& scene, const std::shared_ptr<mesh::Mesh>& mesh, std::size_t subset)
	{
		if (mesh)
		{
			auto& buffer = buffers_[((std::intptr_t)mesh.get())];
			if (!buffer)
				buffer = std::make_shared<ForwardBuffer>(mesh);

			this->context_->setVertexBufferData(0, buffer->getVertexBuffer(), 0);
			this->context_->setIndexBufferData(buffer->getIndexBuffer(subset), 0, hal::GraphicsIndexType::UInt32);

			this->currentBuffer_ = buffer;

			return true;
		}
		else
		{
			return false;
		}
	}

	bool
	ForwardPipeline::setProgram(ForwardScene& scene, const std::shared_ptr<material::Material>& material, const camera::Camera& camera, const geometry::Geometry& geometry)
	{
		if (material)
		{
			auto& pipeline = materials_[((std::intptr_t)material.get())];
			if (!pipeline)
				pipeline = std::make_shared<ForwardMaterial>(material, dynamic_cast<ForwardScene&>(scene));

			pipeline->update(camera, geometry, scene);

			this->context_->setRenderPipeline(pipeline->getPipeline());
			this->context_->setDescriptorSet(pipeline->getDescriptorSet());

			return true;
		}
		else
		{
			return false;
		}
	}
}