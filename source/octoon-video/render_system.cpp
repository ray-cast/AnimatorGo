#include <octoon/video/render_system.h>
#include <octoon/video/camera.h>
#include <octoon/video/geometry.h>
#include <octoon/video/material.h>
#include <octoon/video/render_scene.h>

#include <octoon/runtime/except.h>

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
			, fboMSAA_(0)
			, colorTexture_(0)
			, colorTextureMSAA_(0)
			, depthTexture_(0)
			, depthTextureMSAA_(0)
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
				auto framebuffer = camera->getFramebuffer();
				if (framebuffer)
					context.setFramebuffer(framebuffer);
				else
				{
					if (fboMSAA_)
						context.setFramebuffer(fboMSAA_);
					else
						context.setFramebuffer(fbo_);
				}

				context.setViewport(0, camera->getPixelViewport());
				context.clearFramebuffer(0, camera->getClearFlags(), camera->getClearColor(), 1.0f, 0);

				for (auto& object : video::RenderScene::instance()->getRenderObjects())
				{
					auto geometry = object->downcast<video::Geometry>();
					auto material = geometry->getMaterial();
					if (!material)
						continue;

					material->setTransform(geometry->getTransform());
					material->setViewProjection(camera->getViewProjection());

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

				if (camera->getCameraOrder() == CameraOrder::Main)
				{
					auto& v = camera->getPixelViewport();

					if (framebuffer)
						context.blitFramebuffer(framebuffer, v, nullptr, v);
					else if (fboMSAA_)
						context.blitFramebuffer(fboMSAA_, v, nullptr, v);
					else
						context.blitFramebuffer(fbo_, v, nullptr, v);
				}

				if (framebuffer)
				{
					auto& swapFramebuffer = camera->getSwapFramebuffer();
					if (swapFramebuffer)
					{
						math::float4 v(0, 0, swapFramebuffer->getGraphicsFramebufferDesc().getWidth(), swapFramebuffer->getGraphicsFramebufferDesc().getHeight());
						context.blitFramebuffer(framebuffer, v, swapFramebuffer, v);
					}
				}
			}
		}

		void
		RenderSystem::setupFramebuffers(std::uint32_t w, std::uint32_t h) except
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
			depthTextureDesc.setTexFormat(GraphicsFormat::X8_D24UNormPack32);
			depthTexture_ = device_->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::ColorAttachmentOptimal, GraphicsFormat::R8G8B8A8UNorm));
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::DepthStencilAttachmentOptimal, GraphicsFormat::X8_D24UNormPack32));

			GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setGraphicsFramebufferLayout(device_->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(GraphicsAttachmentBinding(colorTexture_, 0, 0));

			fbo_ = device_->createFramebuffer(framebufferDesc);
			if (!fbo_)
				throw runtime::runtime_error::create("createFramebuffer() failed");

			try
			{
				if (device_->getDeviceProperty().getDeviceProperties().isTextureDimSupport(GraphicsTextureDim::Texture2DMultisample))
				{
					GraphicsTextureDesc colorTextureDescMSAA;
					colorTextureDescMSAA.setWidth(w);
					colorTextureDescMSAA.setHeight(h);
					colorTextureDescMSAA.setTexMultisample(4);
					colorTextureDescMSAA.setTexDim(GraphicsTextureDim::Texture2DMultisample);
					colorTextureDescMSAA.setTexFormat(GraphicsFormat::R8G8B8A8UNorm);
					colorTextureMSAA_ = device_->createTexture(colorTextureDescMSAA);
					if (!colorTextureMSAA_)
						throw runtime::runtime_error::create("createTexture() failed");

					GraphicsTextureDesc depthTextureDescMSAA;
					depthTextureDescMSAA.setWidth(w);
					depthTextureDescMSAA.setHeight(h);
					depthTextureDescMSAA.setTexMultisample(4);
					depthTextureDescMSAA.setTexDim(GraphicsTextureDim::Texture2DMultisample);
					depthTextureDescMSAA.setTexFormat(GraphicsFormat::X8_D24UNormPack32);
					depthTextureMSAA_ = device_->createTexture(depthTextureDescMSAA);
					if (!depthTextureMSAA_)
						throw runtime::runtime_error::create("createTexture() failed");

					GraphicsFramebufferDesc framebufferDescMSAA;
					framebufferDescMSAA.setWidth(w);
					framebufferDescMSAA.setHeight(h);
					framebufferDescMSAA.setGraphicsFramebufferLayout(device_->createFramebufferLayout(framebufferLayoutDesc));
					framebufferDescMSAA.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTextureMSAA_, 0, 0));
					framebufferDescMSAA.addColorAttachment(GraphicsAttachmentBinding(colorTextureMSAA_, 0, 0));

					fboMSAA_ = device_->createFramebuffer(framebufferDescMSAA);
					if (!fboMSAA_)
						throw runtime::runtime_error::create("createFramebuffer() failed");
				}
			}
			catch (...)
			{
			}
		}
	}
}