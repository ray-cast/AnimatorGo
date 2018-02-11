#include <octoon/gui_feature.h>

#include <octoon/input_feature.h>
#include <octoon/input/input_event.h>

#include <octoon/game_server.h>
#include <octoon/game_application.h>

#include <octoon/ui/imgui.h>
#include <octoon/ui/imgui_system.h>

#include <octoon/runtime/rtti_factory.h>

namespace octoon
{
	OctoonImplementSubClass(GuiFeature, GameFeature, "GuiFeature")

	GuiFeature::GuiFeature() noexcept
		: window_(0)
		, width_(0)
		, height_(0)
		, framebuffer_w_(0)
		, framebuffer_h_(0)
		, dpi_(1.0)
	{
	}

	GuiFeature::GuiFeature(WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h, float dpi) noexcept
		: window_(window)
		, width_(w)
		, height_(h)
		, framebuffer_w_(framebuffer_w)
		, framebuffer_h_(framebuffer_h)
		, dpi_(dpi)
	{
	}

	GuiFeature::~GuiFeature() noexcept
	{
	}

	void
	GuiFeature::setViewport(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (width_ != w || height_ != h)
		{
			width_ = w;
			height_ = h;

			imgui::System::instance()->setViewport(w, h);
		}
	}

	void
	GuiFeature::getViewport(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		imgui::System::instance()->getViewport(w, h);
	}

	void
	GuiFeature::setWindowFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (framebuffer_w_ != w || framebuffer_h_ != h)
		{
			framebuffer_w_ = w;
			framebuffer_h_ = h;

			imgui::System::instance()->setFramebufferScale(w, h);
		}
	}

	void
	GuiFeature::getWindowFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		imgui::System::instance()->getFramebufferScale(w, h);
	}

	void
	GuiFeature::onActivate() except
	{
		if (!imgui::System::instance()->open(window_, dpi_))
			throw std::exception("GuiSystem::instance() fail");

		imgui::System::instance()->setViewport(width_, height_);
		imgui::System::instance()->setFramebufferScale(framebuffer_w_, framebuffer_h_);
	}

	void
	GuiFeature::onDeactivate() noexcept
	{
		imgui::System::instance()->close();
	}

	void
	GuiFeature::onInputEvent(const input::InputEvent& event) noexcept
	{
		switch (event.event)
		{
		case input::InputEvent::MouseMotion:
			imgui::System::instance()->injectMouseMove(event.motion.x, event.motion.y);
			break;
		case input::InputEvent::MouseButtonDown:
			imgui::System::instance()->injectMousePress(event.button.x, event.button.y, (input::InputButton::Code)event.button.button);
			break;
		case input::InputEvent::MouseButtonUp:
			imgui::System::instance()->injectMouseRelease(event.button.x, event.button.y, (input::InputButton::Code)event.button.button);
			break;
		case input::InputEvent::KeyDown:
			imgui::System::instance()->injectKeyPress((input::InputKey::Code)event.key.keysym.sym, event.key.keysym.unicode);
			break;
		case input::InputEvent::KeyUp:
			imgui::System::instance()->injectKeyRelease((input::InputKey::Code)event.key.keysym.sym);
			break;
		case input::InputEvent::Character:
			imgui::System::instance()->injectKeyPress(input::InputKey::Code::None, event.key.keysym.unicode);
			break;
		case input::InputEvent::LostFocus:
			imgui::System::instance()->injectWindowFocus(false);
			break;
		case input::InputEvent::GetFocus:
			imgui::System::instance()->injectWindowFocus(true);
			break;
		case input::InputEvent::MouseWheelUp:
			imgui::System::instance()->injectMouseWheel(1.0f);
			break;
		case input::InputEvent::MouseWheelDown:
			imgui::System::instance()->injectMouseWheel(-1.0f);
			break;
		case input::InputEvent::SizeChange:
			imgui::System::instance()->setViewport(event.change.w, event.change.h);
			break;
		case input::InputEvent::SizeChangeDPI:
			imgui::System::instance()->setFramebufferScale(event.change.w, event.change.h);
			break;
		default:
			return;
		}
	}

	void
	GuiFeature::onFrameBegin() noexcept
	{
		imgui::newFrame();
		imgui::showTestWindow();
	}

	void
	GuiFeature::onFrame() noexcept
	{
	}

	void
	GuiFeature::onFrameEnd() noexcept
	{
		imgui::render();
		imgui::System::instance()->render();
	}
}