#if defined(OCTOON_FEATURE_HAL_ENABLE)
#include <octoon/hal_feature.h>
#include <octoon/hal/graphics_device.h>
#include <octoon/hal/graphics_context.h>
#include <octoon/hal/graphics_framebuffer.h>
#include <octoon/hal/graphics_swapchain.h>
#include <octoon/hal/graphics_system.h>
#include <octoon/input/input.h>
#include <octoon/runtime/except.h>
#include <octoon/game_server.h>

namespace octoon
{
	OctoonImplementSubClass(GraphicsFeature, GameFeature, "GraphicsFeature")

	GraphicsFeature::GraphicsFeature() noexcept
		: window_(0)
		, framebuffer_w_(0)
		, framebuffer_h_(0)
	{
	}

	GraphicsFeature::GraphicsFeature(WindHandle window, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept
		: window_(window)
		, framebuffer_w_(framebuffer_w)
		, framebuffer_h_(framebuffer_h)
	{
	}

	GraphicsFeature::~GraphicsFeature() noexcept
	{
	}

	void
	GraphicsFeature::setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (framebuffer_w_ != w || framebuffer_h_ != h)
		{
			if (swapchain_)
				swapchain_->setWindowResolution(w, h);

			framebuffer_w_ = w;
			framebuffer_h_ = h;
		}
	}

	void
	GraphicsFeature::getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		w = framebuffer_w_;
		h = framebuffer_h_;
	}

	const GraphicsDevicePtr&
	GraphicsFeature::getDevice() const noexcept
	{
		return device_;
	}

	const std::shared_ptr<GraphicsSwapchain>&
	GraphicsFeature::getSwapchain() const noexcept
	{
		return swapchain_;
	}

	const std::shared_ptr<GraphicsContext>&
	GraphicsFeature::getContext() const noexcept
	{
		return context_;
	}

	void
	GraphicsFeature::onActivate() except
	{
		this->addMessageListener("feature:input:event", std::bind(&GraphicsFeature::onInputEvent, this, std::placeholders::_1));

		GraphicsDeviceDesc deviceDesc;
#if defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
		deviceDesc.setDeviceType(GraphicsDeviceType::OpenGL20);
#else
		deviceDesc.setDeviceType(GraphicsDeviceType::OpenGL33);
#endif

#if defined(__DEBUG__)
		deviceDesc.setDebugControl(true);
#else
		deviceDesc.setDebugControl(false);
#endif
		device_ = GraphicsSystem::instance()->createDevice(deviceDesc);
		if (!device_)
			throw runtime_error::create("createDevice() failed");

		try
		{
			GraphicsSwapchainDesc swapchainDesc;
			swapchainDesc.setWindHandle(window_);
			swapchainDesc.setWidth(framebuffer_w_);
			swapchainDesc.setHeight(framebuffer_h_);
			swapchainDesc.setSwapInterval(GraphicsSwapInterval::Free);
			swapchainDesc.setImageNums(2);
			swapchainDesc.setColorFormat(GraphicsFormat::B8G8R8A8UNorm);
			swapchainDesc.setDepthStencilFormat(GraphicsFormat::X8_D24UNormPack32);
			swapchainDesc.setMultiSample(0);
			swapchain_ = device_->createSwapchain(swapchainDesc);
			if (!swapchain_)
				throw runtime_error::create("createSwapchain() failed");
		}
		catch (...)
		{
			GraphicsSwapchainDesc swapchainDesc;
			swapchainDesc.setWindHandle(window_);
			swapchainDesc.setWidth(framebuffer_w_);
			swapchainDesc.setHeight(framebuffer_h_);
			swapchainDesc.setSwapInterval(GraphicsSwapInterval::Free);
			swapchainDesc.setImageNums(2);
			swapchainDesc.setColorFormat(GraphicsFormat::B8G8R8A8UNorm);
			swapchainDesc.setDepthStencilFormat(GraphicsFormat::X8_D24UNormPack32);
			swapchain_ = device_->createSwapchain(swapchainDesc);
			if (!swapchain_)
				throw runtime_error::create("createSwapchain() failed");
		}

		GraphicsContextDesc contextDesc;
		contextDesc.setSwapchain(swapchain_);
		context_ = device_->createDeviceContext(contextDesc);
		if (!context_)
			throw runtime_error::create("createDeviceContext() failed");

		this->getGameServer()->log("GraphicsFeature : Graphics Vendor: " + device_->getSystemInfo().graphicsDeviceVendor);
		this->getGameServer()->log("GraphicsFeature : Graphics Device Name: " + device_->getSystemInfo().graphicsDeviceName);
		this->getGameServer()->log("GraphicsFeature : Graphics Device Version: " + device_->getSystemInfo().graphicsDeviceVersion);
	}

	void
	GraphicsFeature::onDeactivate() noexcept
	{
		context_.reset();
		swapchain_.reset();
		device_.reset();
	}

	void
	GraphicsFeature::onInputEvent(const std::any& data) noexcept
	{
		auto event = std::any_cast<input::InputEvent>(data);
		switch (event.event)
		{
		case input::InputEvent::SizeChangeDPI:
			this->setFramebufferScale(event.change.w, event.change.h);
			break;
		default:
			return;
		}
	}

	void
	GraphicsFeature::onFrameBegin() noexcept
	{
		context_->renderBegin();
		context_->setFramebuffer(nullptr);
		context_->clearFramebuffer(0, ClearFlagBits::AllBit, math::float4::Zero, 1.0, 0);
	}

	void
	GraphicsFeature::onFrame() noexcept
	{
	}

	void
	GraphicsFeature::onFrameEnd() noexcept
	{
		context_->renderEnd();
		context_->present();
	}
}
#endif