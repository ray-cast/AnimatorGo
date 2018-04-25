#if defined(OCTOON_FEATURE_UI_ENABLE)
#include <octoon/gui_feature.h>

#include <octoon/input_feature.h>
#include <octoon/input/input_event.h>

#include <octoon/game_server.h>
#include <octoon/game_application.h>
#include <octoon/game_object_manager.h>

#include <octoon/ui/imgui.h>
#include <octoon/ui/imgui_system.h>

#include <octoon/runtime/except.h>
#include <octoon/runtime/rtti_factory.h>

#include <octoon/timer_feature.h>
#include <octoon/graphics_feature.h>

namespace octoon
{
	OctoonImplementSubClass(GuiFeature, GameFeature, "GuiFeature")

	GuiFeature::GuiFeature() noexcept
		: window_(0)
		, width_(0)
		, height_(0)
		, framebuffer_w_(0)
		, framebuffer_h_(0)
	{
	}

	GuiFeature::GuiFeature(WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept
		: window_(window)
		, width_(w)
		, height_(h)
		, framebuffer_w_(framebuffer_w)
		, framebuffer_h_(framebuffer_h)
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

			system_->set_viewport(w, h);
		}
	}

	void
	GuiFeature::getViewport(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		system_->get_viewport(w, h);
	}

	void
	GuiFeature::setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (framebuffer_w_ != w || framebuffer_h_ != h)
		{
			framebuffer_w_ = w;
			framebuffer_h_ = h;

			system_->set_framebuffer_scale(w, h);
		}
	}

	void
	GuiFeature::getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		system_->set_framebuffer_scale(w, h);
	}

	void
	GuiFeature::onActivate() except
	{
		system_ = std::make_unique<imgui::System>();

		auto graphics = this->getFeature<GraphicsFeature>();
		if (!graphics)
			throw runtime::runtime_error::create("failure to get feature with graphics", runtime::error_code::none);

		if (!system_->open(window_, graphics->getDevice()))
			throw runtime::runtime_error::create("GuiSystem::open() fail", runtime::error_code::none);

		if (!system_->load_font("../../system/fonts/DroidSansFallback.ttf", 15.0f * float(width_) / framebuffer_w_))
			throw runtime::runtime_error::create("GuiSystem::loadFont() fail", runtime::error_code::none);

		system_->set_viewport(width_, height_);
		system_->set_framebuffer_scale(framebuffer_w_, framebuffer_h_);
	}

	void
	GuiFeature::onDeactivate() noexcept
	{
		system_->get_viewport(width_, height_);
		system_->set_framebuffer_scale(framebuffer_w_, framebuffer_h_);
		system_.reset();
	}

	void
	GuiFeature::onInputEvent(const input::InputEvent& event) noexcept
	{
		switch (event.event)
		{
		case input::InputEvent::MouseMotion:
			system_->inject_mouse_move(event.motion.x, event.motion.y);
			break;
		case input::InputEvent::MouseButtonDown:
			system_->inject_mouse_press(event.button.x, event.button.y, (input::InputButton::Code)event.button.button);
			break;
		case input::InputEvent::MouseButtonUp:
			system_->inject_mouse_release(event.button.x, event.button.y, (input::InputButton::Code)event.button.button);
			break;
		case input::InputEvent::KeyDown:
			system_->inject_key_press((input::InputKey::Code)event.key.keysym.sym, event.key.keysym.unicode);
			break;
		case input::InputEvent::KeyUp:
			system_->inject_key_release((input::InputKey::Code)event.key.keysym.sym);
			break;
		case input::InputEvent::Character:
			system_->inject_key_press(input::InputKey::Code::None, event.key.keysym.unicode);
			break;
		case input::InputEvent::LostFocus:
			system_->inject_window_focus(false);
			break;
		case input::InputEvent::GetFocus:
			system_->inject_window_focus(true);
			break;
		case input::InputEvent::MouseWheelUp:
			system_->inject_mouse_wheel(1.0f);
			break;
		case input::InputEvent::MouseWheelDown:
			system_->inject_mouse_wheel(-1.0f);
			break;
		case input::InputEvent::SizeChange:
			system_->set_viewport(event.change.w, event.change.h);
			break;
		case input::InputEvent::SizeChangeDPI:
			system_->set_framebuffer_scale(event.change.w, event.change.h);
			break;
		default:
			return;
		}
	}

	void
	GuiFeature::onFrameBegin() noexcept
	{
		system_->newFrame();
	}

	void
	GuiFeature::onFrame() noexcept
	{
		GameObjectManager::instance()->onGui();

		auto graphics = this->getFeature<GraphicsFeature>();
		if (graphics)
			system_->render(*graphics->getContext());
	}

	void
	GuiFeature::onFrameEnd() noexcept
	{
	}
}
#endif