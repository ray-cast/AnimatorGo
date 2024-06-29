#include "config_manager.h"
#include <radeon_rays.h>
#include <octoon/runtime/except.h>
#include <octoon/hal/graphics_device.h>
#include <octoon/hal/graphics_context.h>
#include <octoon/hal/graphics_texture.h>
#include <octoon/hal/graphics_framebuffer.h>
#include "monte_carlo_renderer.h"

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

#include <spdlog/spdlog.h>

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

namespace octoon
{
	ConfigManager::ConfigManager(std::string_view deviceName, const std::filesystem::path& cachePath) noexcept(false)
		: width_(0)
		, height_(0)
		, framebufferWidth_(0)
		, framebufferHeight_(0)
		, dirty_(true)
		, currentRenderDeviceName(deviceName)
		, cachePath_(cachePath)
	{
		this->init();
	}

	ConfigManager::~ConfigManager()
	{
		this->colorTexture_.reset();
		this->normalTexture_.reset();
		this->albedoTexture_.reset();
		this->framebuffer_.reset();

		this->colorImage_.reset();
		this->normalImage_.reset();
		this->albedoImage_.reset();

		for (auto& it : configs_)
		{
			it.controller.reset();
			it.pipeline.reset();
			it.factory.reset();
		}

		this->configs_.clear();
	}

	void
	ConfigManager::init()
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

		std::vector<std::pair<CLWPlatform, CLWDevice>> deviceList;
		for (std::size_t i = 0; i < platforms.size(); ++i)
		{
			for (unsigned int d = 0; d < platforms[i].GetDeviceCount(); ++d)
			{
				spdlog::info("Platform name: {}. Device name: {}.", platforms[i].GetName(), platforms[i].GetDevice(d).GetName());
				deviceList.push_back(std::make_pair(platforms[i], platforms[i].GetDevice(d)));
			}
		}

		// reorder configs
		int maxIdx = 0;
		int maxValue = INT_MIN;
		for (int i = 0; i < deviceList.size(); i++)
		{
			const CLWDevice& device = deviceList[i].second;
			std::string deviceName = device.GetName();

			int lcsNum = this->longestCommonSubsequence(deviceName, currentRenderDeviceName);
			if (lcsNum > maxValue)
			{
				maxValue = lcsNum;
				maxIdx = i;
			}
		}

		deviceList = std::vector<std::pair<CLWPlatform, CLWDevice>>{ deviceList[maxIdx] };

		for (std::size_t i = 0; i < deviceList.size(); ++i)
		{
			cl_device_type device_type = deviceList[i].second.GetType();
			if (device_type == CL_DEVICE_TYPE_GPU)
			{
				bool comp = (kGpuFlags[gpu_counter] & flags) == kGpuFlags[gpu_counter];
				++gpu_counter;
				if (!comp)
					continue;
			}

			if (!use_cpu && (device_type == CL_DEVICE_TYPE_CPU))
				continue;

			Config cfg;
			cfg.caninterop = false;

			if (deviceList[i].second.HasGlInterop() && !hasprimary && interop)
			{
#ifdef WIN32
				cl_context_properties props[] =
				{
					CL_CONTEXT_PLATFORM, (cl_context_properties)((cl_platform_id)deviceList[i].first),
					CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
					CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
					0
				};
#elif __linux__
				cl_context_properties props[] =
				{
					CL_CONTEXT_PLATFORM, (cl_context_properties)((cl_platform_id)deviceList[i].first),
					CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
					CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
					0
				};
#elif __APPLE__
				CGLContextObj kCGLContext = CGLGetCurrentContext();
				CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
				cl_context_properties props[] = { CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup, 0 };
#endif
				cfg.context = CLWContext::Create(deviceList[i].second, props);
				cfg.type = kPrimary;
				cfg.caninterop = true;
				hasprimary = true;
			}
			else
			{
				cfg.context = CLWContext::Create(deviceList[i].second);
				cfg.type = kSecondary;
			}

			this->configs_.push_back(std::move(cfg));
		}

		if (this->configs_.size() == 0)
		{
			use_cpu = true;

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
							continue;
					}

					if (!use_cpu && (device_type == CL_DEVICE_TYPE_CPU))
						continue;

					Config cfg;
					cfg.caninterop = false;

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
						cl_context_properties props[] = { CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup, 0 };
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
					break;
				}
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
			int count = static_cast<int>(configs_[i].context.GetDeviceCount());

			for (int deviceIdx = 0; deviceIdx < count; ++deviceIdx)
			{
				spdlog::info("Config ID: {}. Config Name: {}",
					reinterpret_cast<std::uint32_t>(configs_[i].context.GetDevice(deviceIdx).GetID()),
					configs_[i].context.GetDevice(deviceIdx).GetName());
			}

			this->configs_[i].factory = std::make_unique<ClwRenderFactory>(this->configs_[i].context, cachePath_);
			this->configs_[i].controller = this->configs_[i].factory->createSceneController();
			this->configs_[i].pipeline = this->configs_[i].factory->createPipeline();
		}
	}

	void
	ConfigManager::setOutput(OutputType type, Output* output)
	{
		auto idx = static_cast<std::size_t>(type);
		if (idx >= static_cast<std::size_t>(OutputType::kMax))
			throw std::out_of_range("Output type is out of supported range");

		for (auto& it : configs_)
			it.pipeline->setOutput(type, output);

		outputs_[idx] = output;
	}
	
	Output*
	ConfigManager::getOutput(OutputType type) const
	{
		auto idx = static_cast<std::size_t>(type);
		if (idx >= static_cast<std::size_t>(OutputType::kMax))
			throw std::out_of_range("Output type is out of supported range");
		return outputs_[idx];
	}

	void
	ConfigManager::setMaxBounces(std::uint32_t num_bounces)
	{
		if (!configs_.empty())
		{
			this->dirty_ = true;
			dynamic_cast<MonteCarloRenderer*>(configs_.front().pipeline.get())->setMaxBounces(num_bounces);
		}
	}

	std::uint32_t
	ConfigManager::getMaxBounces() const
	{
		if (!configs_.empty())
			return dynamic_cast<MonteCarloRenderer*>(configs_.front().pipeline.get())->getMaxBounces();
		return 0;
	}

	void
	ConfigManager::setCachePath(const std::filesystem::path& path)
	{
		cachePath_ = path;
	}

	const std::filesystem::path&
	ConfigManager::getCachePath() const
	{
		return cachePath_;
	}

	std::uint32_t
	ConfigManager::getSampleCounter() const
	{
		if (!configs_.empty())
			return dynamic_cast<MonteCarloRenderer*>(configs_.front().pipeline.get())->getSampleCounter();
		return 0;
	}

	void
	ConfigManager::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
	{
		framebufferWidth_ = w;
		framebufferHeight_ = h;
	}

	void
	ConfigManager::getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept
	{
		w = framebufferWidth_;
		h = framebufferHeight_;
	}

	void
	ConfigManager::readColorBuffer(math::float3 colorBuffer[])
	{
		auto& desc = colorTexture_->getTextureDesc();

		void* data = nullptr;
		if (colorTexture_->map(0, 0, desc.getWidth(), desc.getHeight(), 0, &data))
		{
			if (desc.getTexFormat() == octoon::GraphicsFormat::R32G32B32A32SFloat)
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
	ConfigManager::readAlbedoBuffer(math::float3 albedoBuffer[])
	{
		auto& desc = albedoTexture_->getTextureDesc();

		void* data = nullptr;
		if (albedoTexture_->map(0, 0, desc.getWidth(), desc.getHeight(), 0, &data))
		{
			if (desc.getTexFormat() == octoon::GraphicsFormat::R32G32B32A32SFloat)
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
	ConfigManager::readNormalBuffer(math::float3 normalBuffer[])
	{
		auto& desc = normalTexture_->getTextureDesc();

		void* data = nullptr;
		if (normalTexture_->map(0, 0, desc.getWidth(), desc.getHeight(), 0, &data))
		{
			if (desc.getTexFormat() == octoon::GraphicsFormat::R32G32B32A32SFloat)
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

	const GraphicsFramebufferPtr&
	ConfigManager::getFramebuffer() const
	{
		return this->framebuffer_;
	}

	void
	ConfigManager::generateWorkspace(Config& config, const GraphicsContext& context, std::uint32_t width, std::uint32_t height)
	{
		if (width_ != width || height_ != height)
		{
			this->width_ = width;
			this->height_ = height;

			GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(this->width_);
			colorTextureDesc.setHeight(this->height_);
			colorTextureDesc.setTexDim(TextureDimension::Texture2D);
			colorTextureDesc.setTexFormat(GraphicsFormat::R32G32B32A32SFloat);
			colorTexture_ = context.getDevice()->createTexture(colorTextureDesc);
			if (!colorTexture_)
				throw runtime_error::create("createTexture() failed");

			normalTexture_ = context.getDevice()->createTexture(colorTextureDesc);
			if (!normalTexture_)
				throw runtime_error::create("createTexture() failed");

			albedoTexture_ = context.getDevice()->createTexture(colorTextureDesc);
			if (!albedoTexture_)
				throw runtime_error::create("createTexture() failed");

			GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(this->width_);
			depthTextureDesc.setHeight(this->height_);
			depthTextureDesc.setTexDim(TextureDimension::Texture2D);
			depthTextureDesc.setTexFormat(GraphicsFormat::D32_SFLOAT);
			auto depthTexture_ = context.getDevice()->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime_error::create("createTexture() failed");

			GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::ColorAttachmentOptimal, GraphicsFormat::R32G32B32SFloat));
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::DepthStencilAttachmentOptimal, GraphicsFormat::D32_SFLOAT));

			GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(this->width_);
			framebufferDesc.setHeight(this->height_);
			framebufferDesc.setFramebufferLayout(context.getDevice()->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(GraphicsAttachmentBinding(colorTexture_, 0, 0));

			this->framebuffer_ = context.getDevice()->createFramebuffer(framebufferDesc);
			if (!this->framebuffer_)
				throw runtime_error::create("createFramebuffer() failed");

			this->colorImage_ = config.factory->createTextureOutput(static_cast<std::uint32_t>(colorTexture_->handle()), this->width_, this->height_);
			this->normalImage_ = config.factory->createTextureOutput(static_cast<std::uint32_t>(normalTexture_->handle()), this->width_, this->height_);
			this->albedoImage_ = config.factory->createTextureOutput(static_cast<std::uint32_t>(albedoTexture_->handle()), this->width_, this->height_);

			this->setOutput(OutputType::kColor, this->colorImage_.get());
			this->setOutput(OutputType::kWorldShadingNormal, this->normalImage_.get());
			this->setOutput(OutputType::kAlbedo, this->albedoImage_.get());
		}
	}

	void
	ConfigManager::prepareScene(const std::shared_ptr<RenderScene>& scene) noexcept
	{
		for (auto& c : configs_)
		{
			c.controller->cleanCache();
			c.controller->compileScene(scene);
		}
	}

	void
	ConfigManager::render(const GraphicsContextPtr& context, const std::shared_ptr<RenderScene>& scene)
	{
		assert(scene->getMainCamera());

		this->prepareScene(scene);

		for (auto& config : configs_)
		{
			auto mainCamera = scene->getMainCamera();
			auto viewport = mainCamera->getPixelViewport();

			this->generateWorkspace(config, *context, (std::uint32_t)viewport.width, (std::uint32_t)viewport.height);

			CompiledScene& compiledScene = config.controller->getCachedScene(scene);

			auto& clwscene = dynamic_cast<ClwScene&>(compiledScene);
			if (clwscene.dirty || this->dirty_)
			{
				config.pipeline->clear(math::float4::Zero);
				this->dirty_ = clwscene.dirty = false;
			}

			config.pipeline->render(compiledScene);

			float viewportRatio = viewport.width / viewport.height;

			float framebufferHeight = std::min((float)framebufferHeight_, (float)framebufferWidth_ / viewportRatio);
			float framebufferWidth = framebufferHeight * viewportRatio;

			float framebufferX = (framebufferWidth_ - framebufferWidth) / 2;
			float framebufferY = (framebufferHeight_ - framebufferHeight) / 2;

			auto mode = viewport.width == framebufferWidth_ && viewport.height == framebufferHeight_ ? SamplerFilter::Nearest : SamplerFilter::Linear;

			context->setFramebuffer(nullptr);
			context->clearFramebuffer(0, ClearFlagBits::AllBit, mainCamera->getClearColor(), 1.0f, 0);
			context->blitFramebuffer(framebuffer_, viewport, nullptr, math::float4(framebufferX, framebufferY, framebufferX + framebufferWidth, framebufferY + framebufferHeight), mode);
		}
	}

	std::string
	ConfigManager::getCurrentRenderDeviceName() const
	{
		return this->currentRenderDeviceName;
	}

	int
	ConfigManager::longestCommonSubsequence(std::string text1, std::string text2) const
	{
		int n = text1.size(), m = text2.size();
		std::vector<std::vector<int>> f(n + 1, std::vector<int>(m + 1, 0));
		for (int i = 1; i <= n; ++i) {
			for (int j = 1; j <= m; ++j) {
				if (text1[i - 1] == text2[j - 1]) {
					f[i][j] = f[i - 1][j - 1] + 1;
				}
				else {
					f[i][j] = (f[i - 1][j] > f[i][j - 1] ? f[i - 1][j]: f[i][j - 1]);
				}
			}
		}
		return f[n][m];
	}
}