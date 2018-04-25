#include <octoon/video/render_system.h>
#include <octoon/video/camera.h>
#include <octoon/video/geometry.h>
#include <octoon/video/material.h>
#include <octoon/video/render_scene.h>
#include <octoon/video/text_system.h>

#include <octoon/runtime/except.h>

#include <png.h>

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
		RenderSystem::setup(WindHandle hwnd, std::uint32_t w, std::uint32_t h) except
		{
			TextSystem::instance()->setup();

			width_ = w;
			height_ = h;

			graphics::GraphicsDeviceDesc deviceDesc;
			deviceDesc.setDeviceType(graphics::GraphicsDeviceType::OpenGL);
			device_ = graphics::GraphicsSystem::instance()->createDevice(deviceDesc);
			if (!device_)
				return;

			graphics::GraphicsSwapchainDesc swapchainDesc;
			swapchainDesc.setWindHandle(hwnd);
			swapchainDesc.setWidth(w);
			swapchainDesc.setHeight(h);
			swapchainDesc.setSwapInterval(graphics::GraphicsSwapInterval::Vsync);
			swapchainDesc.setImageNums(2);
			swapchainDesc.setColorFormat(graphics::GraphicsFormat::B8G8R8A8UNorm);
			swapchainDesc.setDepthStencilFormat(graphics::GraphicsFormat::D24UNorm_S8UInt);
			swapchain_ = device_->createSwapchain(swapchainDesc);
			if (!swapchain_)
				return;

			graphics::GraphicsContextDesc contextDesc;
			contextDesc.setSwapchain(swapchain_);
			context_ = device_->createDeviceContext(contextDesc);
			if (!context_)
				return;

			graphics::GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexFormat(graphics::GraphicsFormat::R8G8B8A8UNorm);
			colorTexture_ = device_->createTexture(colorTextureDesc);

			graphics::GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexFormat(graphics::GraphicsFormat::X8_D24UNormPack32);
			depthTexture_ = device_->createTexture(depthTextureDesc);

			graphics::GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::ColorAttachmentOptimal, graphics::GraphicsFormat::R8G8B8A8UNorm));
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::DepthStencilAttachmentOptimal, graphics::GraphicsFormat::X8_D24UNormPack32));

			graphics::GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setGraphicsFramebufferLayout(device_->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(graphics::GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(graphics::GraphicsAttachmentBinding(colorTexture_, 0, 0));

			fbo_ = device_->createFramebuffer(framebufferDesc);

#if !defined(__linux)
			graphics::GraphicsTextureDesc colorTextureDescMSAA;
			colorTextureDescMSAA.setWidth(w);
			colorTextureDescMSAA.setHeight(h);
			colorTextureDescMSAA.setTexMultisample(4);
			colorTextureDescMSAA.setTexFormat(graphics::GraphicsFormat::R8G8B8A8UNorm);
			colorTextureMSAA_ = device_->createTexture(colorTextureDescMSAA);

			graphics::GraphicsTextureDesc depthTextureDescMSAA;
			depthTextureDescMSAA.setWidth(w);
			depthTextureDescMSAA.setHeight(h);
			depthTextureDescMSAA.setTexMultisample(4);
			depthTextureDescMSAA.setTexFormat(graphics::GraphicsFormat::X8_D24UNormPack32);
			depthTextureMSAA_ = device_->createTexture(depthTextureDescMSAA);

			graphics::GraphicsFramebufferDesc framebufferDescMSAA;
			framebufferDescMSAA.setWidth(w);
			framebufferDescMSAA.setHeight(h);
			framebufferDescMSAA.setGraphicsFramebufferLayout(device_->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDescMSAA.setDepthStencilAttachment(graphics::GraphicsAttachmentBinding(depthTextureMSAA_, 0, 0));
			framebufferDescMSAA.addColorAttachment(graphics::GraphicsAttachmentBinding(colorTextureMSAA_, 0, 0));

			fboMSAA_ = device_->createFramebuffer(framebufferDescMSAA);
#endif
		}

		void
		RenderSystem::close() noexcept
		{
			TextSystem::instance()->close();
		}

		void
		RenderSystem::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
		{
			width_ = w;
			height_ = h;
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
		RenderSystem::render() noexcept
		{
			context_->renderBegin();

			for (auto& camera : video::RenderScene::instance()->getCameraList())
			{
#if (__linux)
				context_->setFramebuffer(fbo_);
#else
				context_->setFramebuffer(fboMSAA_);
#endif
				context_->setViewport(0, camera->getPixelViewport());
				context_->clearFramebuffer(0, octoon::graphics::GraphicsClearFlagBits::ColorDepthBit, camera->getClearColor(), 1.0f, 0);

				for (auto& object : video::RenderScene::instance()->getRenderObjects())
				{
					auto geometry = object->downcast<video::Geometry>();
					auto mesh = geometry->getMesh();
					if (!mesh)
						continue;

					auto material = geometry->getMaterial();
					if (!material)
						continue;

					material->setTransform(geometry->getTransform());
					material->setViewProjection(camera->getViewProjection());

					context_->setRenderPipeline(material->getPipeline());
					context_->setDescriptorSet(material->getDescriptorSet());
					context_->setVertexBufferData(0, geometry->getVertexBuffer(), 0);

					context_->draw(mesh->getVertexArray().size(), 1, 0, 0);
				}

				if (camera->getCameraOrder() == CameraOrder::Main)
				{
						auto& v = camera->getPixelViewport();
#if (__linux)
						context_->blitFramebuffer(fbo_, v, nullptr, v);
#else
						context_->blitFramebuffer(fboMSAA_, v, nullptr, v);
#endif
				}
			}

			context_->renderEnd();
			context_->present();
		}

		void
		RenderSystem::saveAsPNG(const char* filepath, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept(false)
		{
			context_->renderBegin();

#if !defined(__linux)
			math::float4 v(x, y, width, height);
			context_->blitFramebuffer(fboMSAA_, v, fbo_, v);
#endif

			auto texture = fbo_->getGraphicsFramebufferDesc().getColorAttachments().at(0).getBindingTexture();
			std::vector<std::uint32_t> pixels((width - x) * (height - y));

			void* data = nullptr;
			if (texture->map(x, y, width, height, 0, &data))
			{
				std::memcpy(pixels.data(), data, pixels.size() * sizeof(std::uint32_t));
				texture->unmap();
			}

			for (std::uint32_t y = 0; y < height / 2; y++)
			{
				for (std::uint32_t x = 0; x < width; x++)
					std::swap(pixels[y * width + x], pixels[(height - y - 1) * width + x]);
			}

			context_->renderEnd();

			png_infop info_ptr = nullptr;
			png_structp png_ptr = nullptr;

			FILE* png_file = ::fopen(filepath, "wb");
			if (!png_file)
				throw runtime::runtime_error::create(std::string("failed to create file with ") + filepath);

			try
			{
				png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
				if (!png_ptr)
					throw runtime::runtime_error::create(std::string("png_create_write_struct() failed"));

				info_ptr = png_create_info_struct(png_ptr);
				if (!info_ptr)
					throw runtime::runtime_error::create(std::string("png_create_info_struct() failed"));

				if (setjmp(png_jmpbuf(png_ptr)))
					throw runtime::runtime_error::create(std::string("setjmp() failed"));

				png_init_io(png_ptr, png_file);
				png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

				auto palette = std::make_unique<png_color[]>(PNG_MAX_PALETTE_LENGTH);
				if (!palette)
					throw runtime::runtime_error::create(std::string("png_malloc() failed"));

				png_set_strip_16(png_ptr);
				png_set_packing(png_ptr);
				png_set_PLTE(png_ptr, info_ptr, palette.get(), PNG_MAX_PALETTE_LENGTH);

				png_write_info(png_ptr, info_ptr);

				auto rows = std::make_unique<png_bytep[]>(height);
				for (std::uint32_t i = 0; i < height; ++i)
					rows[i] = (png_bytep)(pixels.data() + (height - i - 1) * width);

				png_write_image(png_ptr, rows.get());
				png_write_end(png_ptr, info_ptr);

				png_destroy_write_struct(&png_ptr, &info_ptr);
				fclose(png_file);
			}
			catch (const std::exception& e)
			{
				::fclose(png_file);

				if (info_ptr)
					png_destroy_write_struct(&png_ptr, nullptr);
				else
					png_destroy_write_struct(&png_ptr, &info_ptr);

				throw e;
			}
		}
	}
}