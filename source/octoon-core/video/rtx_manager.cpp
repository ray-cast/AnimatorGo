#include "rtx_manager.h"
#include <radeon_rays.h>
#include <octoon/runtime/except.h>

#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#include <OpenGL/OpenGL.h>
#elif WIN32
#include <Windows.h>
#include "GL/glew.h"
#else
#include <CL/cl.h>
#include <GL/glew.h>
#include <GL/glx.h>
#endif

#define RPR_CREATION_FLAGS_ENABLE_GPU0 (1 << 0) 
#define RPR_CREATION_FLAGS_ENABLE_GPU1 (1 << 1) 
#define RPR_CREATION_FLAGS_ENABLE_GPU2 (1 << 2) 
#define RPR_CREATION_FLAGS_ENABLE_GPU3 (1 << 3) 
#define RPR_CREATION_FLAGS_ENABLE_CPU (1 << 4) 
#define RPR_CREATION_FLAGS_ENABLE_GL_INTEROP (1 << 5) 
#define RPR_CREATION_FLAGS_ENABLE_GPU4 (1 << 6) 
#define RPR_CREATION_FLAGS_ENABLE_GPU5 (1 << 7) 
#define RPR_CREATION_FLAGS_ENABLE_GPU6 (1 << 8) 
#define RPR_CREATION_FLAGS_ENABLE_GPU7 (1 << 9) 
#define RPR_CREATION_FLAGS_ENABLE_METAL (1 << 10) 

namespace octoon::video
{
	RtxManager::RtxManager()
		: width_(0)
		, height_(0)
	{
		std::vector<CLWPlatform> platforms;

		CLWPlatform::CreateAllPlatforms(platforms);

		if (platforms.size() == 0)
		{
			throw std::runtime_error("No OpenCL platforms installed.");
		}

		this->configs_.clear();

		static const std::vector<std::uint32_t> kGpuFlags =
		{
			RPR_CREATION_FLAGS_ENABLE_GPU0,
			RPR_CREATION_FLAGS_ENABLE_GPU1,
			RPR_CREATION_FLAGS_ENABLE_GPU2,
			RPR_CREATION_FLAGS_ENABLE_GPU3,
			RPR_CREATION_FLAGS_ENABLE_GPU4,
			RPR_CREATION_FLAGS_ENABLE_GPU5,
			RPR_CREATION_FLAGS_ENABLE_GPU6,
			RPR_CREATION_FLAGS_ENABLE_GPU7
		};

		auto flags = RPR_CREATION_FLAGS_ENABLE_GPU0 | RPR_CREATION_FLAGS_ENABLE_GPU1 | RPR_CREATION_FLAGS_ENABLE_GL_INTEROP;
		bool use_cpu = (flags & RPR_CREATION_FLAGS_ENABLE_CPU) == RPR_CREATION_FLAGS_ENABLE_CPU;
		bool interop = (flags & RPR_CREATION_FLAGS_ENABLE_GL_INTEROP) == RPR_CREATION_FLAGS_ENABLE_GL_INTEROP;
		bool hasprimary = false;

		std::uint32_t gpu_counter = 0;

		for (std::size_t i = 0; i < platforms.size(); ++i)
		{
			for (unsigned int d = 0; d < platforms[i].GetDeviceCount(); ++d)
			{
				cl_device_type device_type = platforms[i].GetDevice(d).GetType();

				if (device_type == CL_DEVICE_TYPE_GPU)
				{
					bool comp = (kGpuFlags[gpu_counter] & flags) == kGpuFlags[gpu_counter];
					++gpu_counter;
					if (!comp)
					{
						continue;
					}
				}

				if (!use_cpu && (device_type == CL_DEVICE_TYPE_CPU))
				{
					continue;
				}

				Config cfg;
				cfg.caninterop = false;

				if (interop)
				{
					if (platforms[i].GetName().find("Intel") != std::string::npos)
						continue;
				}

				if (platforms[i].GetDevice(d).HasGlInterop() && !hasprimary && interop)
				{
#ifdef WIN32
					cl_context_properties props[] =
					{
						CL_CONTEXT_PLATFORM, (cl_context_properties)((cl_platform_id)platforms[i]),
						CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
						CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
						0
					};
#elif __linux__
					cl_context_properties props[] =
					{
						CL_CONTEXT_PLATFORM, (cl_context_properties)((cl_platform_id)platforms[i]),
						CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
						CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
						0
					};
#elif __APPLE__
					CGLContextObj kCGLContext = CGLGetCurrentContext();
					CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
					cl_context_properties props[] = { CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup, 0};
#endif
					cfg.context = CLWContext::Create(platforms[i].GetDevice(d), props);
					cfg.type = kPrimary;
					cfg.caninterop = true;
					hasprimary = true;
				}
				else
				{
					cfg.context = CLWContext::Create(platforms[i].GetDevice(d));
					cfg.type = kSecondary;
				}

				this->configs_.push_back(std::move(cfg));
			}
		}

		if (this->configs_.size() == 0)
		{
			throw std::runtime_error("No devices was selected (probably device index type does not correspond with real device type).");
		}

		if (!hasprimary)
		{
			this->configs_[0].type = kPrimary;
		}

		for (std::size_t i = 0; i < this->configs_.size(); ++i)
		{
			this->configs_[i].factory = std::make_unique<ClwRenderFactory>(this->configs_[i].context, "cache");
			this->configs_[i].controller = this->configs_[i].factory->createSceneController();
			this->configs_[i].pipeline = this->configs_[i].factory->createPipeline();
		}
	}

	void
	RtxManager::setGraphicsContext(const hal::GraphicsContextPtr& context) noexcept(false)
	{
		this->context_ = context;
	}

	const hal::GraphicsContextPtr&
	RtxManager::getGraphicsContext() const noexcept(false)
	{
		return this->context_;
	}

	void
	RtxManager::setRenderScene(RenderScene* scene) noexcept
	{
		this->scene_ = scene;
	}

	const RenderScene*
	RtxManager::getRenderScene() const noexcept
	{
		return this->scene_;
	}

	void
	RtxManager::setOutput(OutputType type, Output* output)
	{
		auto idx = static_cast<std::size_t>(type);
		if (idx >= static_cast<std::size_t>(OutputType::kMax))
			throw std::out_of_range("Output type is out of supported range");

		for (auto& it : configs_)
			it.pipeline->setOutput(type, output);

		outputs_[idx] = output;
	}
	
	Output*
	RtxManager::getOutput(OutputType type) const
	{
		auto idx = static_cast<std::size_t>(type);
		if (idx >= static_cast<std::size_t>(OutputType::kMax))
			throw std::out_of_range("Output type is out of supported range");
		return outputs_[idx];
	}

	void
	RtxManager::readColorBuffer(math::float3 colorBuffer[])
	{
		auto& desc = colorTexture_->getTextureDesc();

		void* data = nullptr;
		if (colorTexture_->map(0, 0, desc.getWidth(), desc.getHeight(), 0, &data))
		{
			if (desc.getTexFormat() == octoon::hal::GraphicsFormat::R32G32B32A32SFloat)
			{
				for (std::size_t i = 0; i < desc.getWidth() * desc.getHeight(); ++i)
					colorBuffer[i] = (((octoon::math::float4*)data) + i)->xyz();
			}
			else
			{
				std::memcpy(colorBuffer, data, desc.getWidth() * desc.getHeight() * 3 * sizeof(float));
			}

			colorTexture_->unmap();
		}
	}

	void
	RtxManager::readAlbedoBuffer(math::float3 albedoBuffer[])
	{
		auto& desc = albedoTexture_->getTextureDesc();

		void* data = nullptr;
		if (albedoTexture_->map(0, 0, desc.getWidth(), desc.getHeight(), 0, &data))
		{
			if (desc.getTexFormat() == octoon::hal::GraphicsFormat::R32G32B32A32SFloat)
			{
				for (std::size_t i = 0; i < desc.getWidth() * desc.getHeight(); ++i)
					albedoBuffer[i] = (((octoon::math::float4*)data) + i)->xyz();
			}
			else
			{
				std::memcpy(albedoBuffer, data, desc.getWidth() * desc.getHeight() * 3 * sizeof(float));
			}

			albedoTexture_->unmap();
		}
	}
	
	void
	RtxManager::readNormalBuffer(math::float3 normalBuffer[])
	{
		auto& desc = normalTexture_->getTextureDesc();

		void* data = nullptr;
		if (normalTexture_->map(0, 0, desc.getWidth(), desc.getHeight(), 0, &data))
		{
			if (desc.getTexFormat() == octoon::hal::GraphicsFormat::R32G32B32A32SFloat)
			{
				for (std::size_t i = 0; i < desc.getWidth() * desc.getHeight(); ++i)
					normalBuffer[i] = (((octoon::math::float4*)data) + i)->xyz();
			}
			else
			{
				std::memcpy(normalBuffer, data, desc.getWidth() * desc.getHeight() * 3 * sizeof(float));
			}

			normalTexture_->unmap();
		}
	}

	const hal::GraphicsFramebufferPtr&
	RtxManager::getFramebuffer() const
	{
		return this->framebuffer_;
	}

	void
	RtxManager::generateWorkspace(std::uint32_t width, std::uint32_t height)
	{
		if (width_ != width || height_ != height)
		{
			this->width_ = width;
			this->height_ = height;

			hal::GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(this->width_);
			colorTextureDesc.setHeight(this->height_);
			colorTextureDesc.setTexDim(hal::GraphicsTextureDim::Texture2D);
			colorTextureDesc.setTexFormat(hal::GraphicsFormat::R32G32B32A32SFloat);
			colorTexture_ = context_->getDevice()->createTexture(colorTextureDesc);
			if (!colorTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			normalTexture_ = context_->getDevice()->createTexture(colorTextureDesc);
			if (!normalTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			albedoTexture_ = context_->getDevice()->createTexture(colorTextureDesc);
			if (!albedoTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(this->width_);
			depthTextureDesc.setHeight(this->height_);
			depthTextureDesc.setTexDim(hal::GraphicsTextureDim::Texture2D);
			depthTextureDesc.setTexFormat(hal::GraphicsFormat::D32_SFLOAT);
			auto depthTexture_ = context_->getDevice()->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(0, hal::GraphicsImageLayout::ColorAttachmentOptimal, hal::GraphicsFormat::R32G32B32SFloat));
			framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(1, hal::GraphicsImageLayout::DepthStencilAttachmentOptimal, hal::GraphicsFormat::D32_SFLOAT));

			hal::GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(this->width_);
			framebufferDesc.setHeight(this->height_);
			framebufferDesc.setFramebufferLayout(context_->getDevice()->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(hal::GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(hal::GraphicsAttachmentBinding(colorTexture_, 0, 0));

			this->framebuffer_ = context_->getDevice()->createFramebuffer(framebufferDesc);
			if (!this->framebuffer_)
				throw runtime::runtime_error::create("createFramebuffer() failed");

			if (configs_.size() != 1)
			{
				throw std::runtime_error("ContextObject: invalid config count.");
			}

			auto& c = configs_[0];
			this->colorImage_ = c.factory->createTextureOutput(static_cast<std::uint32_t>(colorTexture_->handle()), this->width_, this->height_);
			this->normalImage_ = c.factory->createTextureOutput(static_cast<std::uint32_t>(normalTexture_->handle()), this->width_, this->height_);
			this->albedoImage_ = c.factory->createTextureOutput(static_cast<std::uint32_t>(albedoTexture_->handle()), this->width_, this->height_);

			this->setOutput(OutputType::kColor, this->colorImage_.get());
			this->setOutput(OutputType::kWorldShadingNormal, this->normalImage_.get());
			this->setOutput(OutputType::kAlbedo, this->albedoImage_.get());
		}
	}

	void
	RtxManager::prepareScene(const std::shared_ptr<RenderScene>& scene) noexcept
	{
		for (auto& c : configs_)
		{
			c.controller->cleanCache();
			c.controller->compileScene(scene);
		}
	}

	void
	RtxManager::render(const std::shared_ptr<RenderScene>& scene)
	{
		this->prepareScene(scene);

		for (auto& c : configs_)
		{
			auto mainCamera = scene->getMainCamera();
			auto viewport = mainCamera->getPixelViewport();
			this->generateWorkspace((std::uint32_t)viewport.width, (std::uint32_t)viewport.height);

			CompiledScene& compiledScene = c.controller->getCachedScene(scene);

			auto& clwscene = dynamic_cast<ClwScene&>(compiledScene);
			if (clwscene.dirty)
			{
				c.pipeline->clear(math::float4::Zero);
				clwscene.dirty = false;
			}

			if (clwscene.shapes.GetElementCount() > 0)
				c.pipeline->render(compiledScene);
		}

		math::float4 viewport(0, 0, static_cast<float>(this->width_), static_cast<float>(this->height_));
		this->context_->blitFramebuffer(framebuffer_, viewport, nullptr, viewport);
	}
}