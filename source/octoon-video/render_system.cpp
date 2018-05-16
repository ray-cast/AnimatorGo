#include <octoon/video/render_system.h>
#include <octoon/video/camera.h>
#include <octoon/video/geometry.h>
#include <octoon/video/material.h>
#include <octoon/video/render_scene.h>

#include <octoon/runtime/except.h>

#include <iostream>

using namespace octoon::graphics;

namespace octoon
{
	namespace video
	{
		OctoonImplementSingleton(RenderSystem)

		RenderSystem::RenderSystem() noexcept
			: width_(0)
			, height_(0)
			, fbo_(0)
			, colorTexture_(0)
			, depthTexture_(0)
		{
		}

		RenderSystem::~RenderSystem() noexcept
		{
			this->close();
		}

		void
		RenderSystem::setup(const GraphicsDevicePtr& device, std::uint32_t w, std::uint32_t h) except
		{
			device_ = device;

			this->setFramebufferSize(w, h);
		}

		void
		RenderSystem::close() noexcept
		{
		}

		void
		RenderSystem::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
		{
			if (width_ != w || height_ != h)
			{
				this->setupFramebuffers(w, h);

				width_ = w;
				height_ = h;
			}
		}

		void
		RenderSystem::getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept
		{
			w = width_;
			h = height_;
		}

		GraphicsInputLayoutPtr
		RenderSystem::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
		{
			assert(device_);
			return device_->createInputLayout(desc);
		}

		GraphicsDataPtr
		RenderSystem::createGraphicsData(const GraphicsDataDesc& desc) noexcept
		{
			assert(device_);
			return device_->createGraphicsData(desc);
		}

		GraphicsTexturePtr
		RenderSystem::createTexture(const GraphicsTextureDesc& desc) noexcept
		{
			assert(device_);
			return device_->createTexture(desc);
		}

		GraphicsSamplerPtr
		RenderSystem::createSampler(const GraphicsSamplerDesc& desc) noexcept
		{
			assert(device_);
			return device_->createSampler(desc);
		}

		GraphicsFramebufferPtr
		RenderSystem::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
		{
			assert(device_);
			return device_->createFramebuffer(desc);
		}

		GraphicsFramebufferLayoutPtr
		RenderSystem::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
		{
			assert(device_);
			return device_->createFramebufferLayout(desc);
		}

		GraphicsShaderPtr
		RenderSystem::createShader(const GraphicsShaderDesc& desc) noexcept
		{
			assert(device_);
			return device_->createShader(desc);
		}

		GraphicsProgramPtr
		RenderSystem::createProgram(const GraphicsProgramDesc& desc) noexcept
		{
			assert(device_);
			return device_->createProgram(desc);
		}

		GraphicsStatePtr
		RenderSystem::createRenderState(const GraphicsStateDesc& desc) noexcept
		{
			assert(device_);
			return device_->createRenderState(desc);
		}

		GraphicsPipelinePtr
		RenderSystem::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
		{
			assert(device_);
			return device_->createRenderPipeline(desc);
		}

		GraphicsDescriptorSetPtr
		RenderSystem::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
		{
			assert(device_);
			return device_->createDescriptorSet(desc);
		}

		GraphicsDescriptorSetLayoutPtr
		RenderSystem::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
		{
			assert(device_);
			return device_->createDescriptorSetLayout(desc);
		}

		GraphicsDescriptorPoolPtr
		RenderSystem::createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept
		{
			assert(device_);
			return device_->createDescriptorPool(desc);
		}

		void
		RenderSystem::render(graphics::GraphicsContext& context) noexcept
		{
			for (auto& camera : video::RenderScene::instance()->getCameraList())
			{
#if !defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
				auto framebuffer = camera->getFramebuffer();
				if (framebuffer)
					context.setFramebuffer(framebuffer);
				else
					context.setFramebuffer(fbo_);
#endif

				context.setViewport(0, camera->getPixelViewport());
				context.clearFramebuffer(0, camera->getClearFlags(), camera->getClearColor(), 1.0f, 0);

				for (auto& object : video::RenderScene::instance()->getRenderObjects())
				{
					auto geometry = object->downcast<video::Geometry>();
					if (!geometry)
						continue;

					auto material = geometry->getMaterial();
					if (!material)
						continue;

					material->setTransform(geometry->getTransform());
					material->setViewProjection(camera->getViewProjection());

					if (geometry->getVertexBuffer())
					{
						context.setRenderPipeline(material->getPipeline());
						context.setDescriptorSet(material->getDescriptorSet());
						context.setVertexBufferData(0, geometry->getVertexBuffer(), 0);
						context.setIndexBufferData(geometry->getIndexBuffer(), 0, graphics::GraphicsIndexType::UInt32);

						auto indices = geometry->getNumIndices();
						if (indices > 0)
							context.drawIndexed(indices, 1, 0, 0, 0);
						else
							context.draw(geometry->getNumVertices(), 1, 0, 0);
					}
				}

#if !defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
				if (camera->getCameraOrder() == CameraOrder::Main)
				{
					auto& v = camera->getPixelViewport();

					if (framebuffer)
						context.blitFramebuffer(framebuffer, v, nullptr, v);
					else
						context.blitFramebuffer(fbo_, v, nullptr, v);
				}

				if (framebuffer)
				{
					auto& swapFramebuffer = camera->getSwapFramebuffer();
					if (swapFramebuffer)
					{
						math::float4 v1(0, 0, (float)framebuffer->getGraphicsFramebufferDesc().getWidth(), (float)framebuffer->getGraphicsFramebufferDesc().getHeight());
						math::float4 v2(0, 0, (float)swapFramebuffer->getGraphicsFramebufferDesc().getWidth(), (float)swapFramebuffer->getGraphicsFramebufferDesc().getHeight());
						context.blitFramebuffer(framebuffer, v1, swapFramebuffer, v2);
					}
				}
#endif
			}
		}

		void
		RenderSystem::setupFramebuffers(std::uint32_t w, std::uint32_t h) except
		{
			GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::ColorAttachmentOptimal, GraphicsFormat::R8G8B8A8UNorm));
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::DepthStencilAttachmentOptimal, GraphicsFormat::X8_D24UNormPack32));

			try
			{
				GraphicsTextureDesc colorTextureDesc;
				colorTextureDesc.setWidth(w);
				colorTextureDesc.setHeight(h);
				colorTextureDesc.setTexMultisample(4);
				colorTextureDesc.setTexDim(GraphicsTextureDim::Texture2DMultisample);
				colorTextureDesc.setTexFormat(GraphicsFormat::R8G8B8A8UNorm);
				colorTexture_ = device_->createTexture(colorTextureDesc);
				if (!colorTexture_)
					throw runtime::runtime_error::create("createTexture() failed");

				GraphicsTextureDesc depthTextureDesc;
				depthTextureDesc.setWidth(w);
				depthTextureDesc.setHeight(h);
				depthTextureDesc.setTexMultisample(4);
				depthTextureDesc.setTexDim(GraphicsTextureDim::Texture2DMultisample);
				depthTextureDesc.setTexFormat(GraphicsFormat::X8_D24UNormPack32);
				depthTexture_ = device_->createTexture(depthTextureDesc);
				if (!depthTexture_)
					throw runtime::runtime_error::create("createTexture() failed");

				GraphicsFramebufferDesc framebufferDesc;
				framebufferDesc.setWidth(w);
				framebufferDesc.setHeight(h);
				framebufferDesc.setGraphicsFramebufferLayout(device_->createFramebufferLayout(framebufferLayoutDesc));
				framebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture_, 0, 0));
				framebufferDesc.addColorAttachment(GraphicsAttachmentBinding(colorTexture_, 0, 0));

				fbo_ = device_->createFramebuffer(framebufferDesc);
				if (!fbo_)
					throw runtime::runtime_error::create("createFramebuffer() failed");
			}
			catch (...)
			{
				GraphicsTextureDesc colorTextureDesc;
				colorTextureDesc.setWidth(w);
				colorTextureDesc.setHeight(h);
				colorTextureDesc.setTexFormat(GraphicsFormat::R8G8B8A8UNorm);
				colorTexture_ = device_->createTexture(colorTextureDesc);
				if (!colorTexture_)
					throw runtime::runtime_error::create("createTexture() failed");

				GraphicsTextureDesc depthTextureDesc;
				depthTextureDesc.setWidth(w);
				depthTextureDesc.setHeight(h);
				depthTextureDesc.setTexFormat(GraphicsFormat::D16UNorm);
				depthTexture_ = device_->createTexture(depthTextureDesc);
				if (!depthTexture_)
					throw runtime::runtime_error::create("createTexture() failed");

				GraphicsFramebufferDesc framebufferDesc;
				framebufferDesc.setWidth(w);
				framebufferDesc.setHeight(h);
				framebufferDesc.setGraphicsFramebufferLayout(device_->createFramebufferLayout(framebufferLayoutDesc));
				framebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture_, 0, 0));
				framebufferDesc.addColorAttachment(GraphicsAttachmentBinding(colorTexture_, 0, 0));

				fbo_ = device_->createFramebuffer(framebufferDesc);
				if (!fbo_)
					throw runtime::runtime_error::create("createFramebuffer() failed");
			}
		}
	}
}