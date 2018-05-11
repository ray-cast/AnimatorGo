#if defined(OCTOON_FEATURE_GRAPHICS_ENABLE)
#include <octoon/graphics_feature.h>
#include <octoon/graphics/graphics.h>

#include <octoon/runtime/except.h>

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

	const graphics::GraphicsDevicePtr&
	GraphicsFeature::getDevice() const noexcept
	{
		return device_;
	}

	const graphics::GraphicsSwapchainPtr&
	GraphicsFeature::getSwapchain() const noexcept
	{
		return swapchain_;
	}

	const graphics::GraphicsContextPtr&
	GraphicsFeature::getContext() const noexcept
	{
		return context_;
	}

	void
	GraphicsFeature::onActivate() except
	{
		graphics::GraphicsDeviceDesc deviceDesc;
		deviceDesc.setDeviceType(graphics::GraphicsDeviceType::OpenGL33);
		device_ = graphics::GraphicsSystem::instance()->createDevice(deviceDesc);
		if (!device_)
			throw runtime::runtime_error::create("createDevice() failed");

		graphics::GraphicsSwapchainDesc swapchainDesc;
		swapchainDesc.setWindHandle(window_);
		swapchainDesc.setWidth(framebuffer_w_);
		swapchainDesc.setHeight(framebuffer_h_);
		swapchainDesc.setSwapInterval(graphics::GraphicsSwapInterval::Vsync);
		swapchainDesc.setImageNums(2);
		swapchainDesc.setColorFormat(graphics::GraphicsFormat::B8G8R8A8UNorm);
		swapchainDesc.setDepthStencilFormat(graphics::GraphicsFormat::D24UNorm_S8UInt);
		swapchain_ = device_->createSwapchain(swapchainDesc);
		if (!swapchain_)
			throw runtime::runtime_error::create("createSwapchain() failed");

		graphics::GraphicsContextDesc contextDesc;
		contextDesc.setSwapchain(swapchain_);
		context_ = device_->createDeviceContext(contextDesc);
		if (!context_)
			throw runtime::runtime_error::create("createDeviceContext() failed");
	}

	void
	GraphicsFeature::onDeactivate() noexcept
	{
	}

	void
	GraphicsFeature::onInputEvent(const input::InputEvent& event) noexcept
	{
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
		context_->clearFramebuffer(0, graphics::GraphicsClearFlagBits::AllBit, math::float4::Zero, 1.0, 0);
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