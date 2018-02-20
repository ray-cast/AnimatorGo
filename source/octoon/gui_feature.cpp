#include <octoon/gui_feature.h>

#include <octoon/input_feature.h>
#include <octoon/input/input_event.h>

#include <octoon/game_server.h>
#include <octoon/game_application.h>
#include <octoon/game_object_manager.h>

#include <octoon/ui/imgui.h>
#include <octoon/ui/imgui_system.h>

#include <octoon/runtime/rtti_factory.h>

#include <octoon/timer_feature.h>

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
	GuiFeature::set_viewport(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (width_ != w || height_ != h)
		{
			width_ = w;
			height_ = h;

			imgui::System::instance()->set_viewport(w, h);
		}
	}

	void
	GuiFeature::get_viewport(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		imgui::System::instance()->get_viewport(w, h);
	}

	void
	GuiFeature::set_framebuffer_scale(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (framebuffer_w_ != w || framebuffer_h_ != h)
		{
			framebuffer_w_ = w;
			framebuffer_h_ = h;

			imgui::System::instance()->set_framebuffer_scale(w, h);
		}
	}

	void
	GuiFeature::get_framebuffer_scale(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		imgui::System::instance()->get_framebuffer_scale(w, h);
	}

	void
	GuiFeature::on_activate() except
	{
		if (!imgui::System::instance()->open(window_))
			throw std::exception("GuiSystem::open() fail");

		if (!imgui::System::instance()->load_font("../../system/fonts/DroidSansFallback.ttf", 15.0f * float(width_) / framebuffer_w_))
			throw std::exception("GuiSystem::load_font() fail");

		imgui::System::instance()->set_viewport(width_, height_);
		imgui::System::instance()->set_framebuffer_scale(framebuffer_w_, framebuffer_h_);
	}

	void
	GuiFeature::on_deactivate() noexcept
	{
		imgui::System::instance()->get_viewport(width_, height_);
		imgui::System::instance()->get_framebuffer_scale(framebuffer_w_, framebuffer_h_);
		imgui::System::instance()->close();
	}

	void
	GuiFeature::on_input_event(const input::InputEvent& event) noexcept
	{
		switch (event.event)
		{
		case input::InputEvent::MouseMotion:
			imgui::System::instance()->inject_mouse_move(event.motion.x, event.motion.y);
			break;
		case input::InputEvent::MouseButtonDown:
			imgui::System::instance()->inject_mouse_press(event.button.x, event.button.y, (input::InputButton::Code)event.button.button);
			break;
		case input::InputEvent::MouseButtonUp:
			imgui::System::instance()->inject_mouse_release(event.button.x, event.button.y, (input::InputButton::Code)event.button.button);
			break;
		case input::InputEvent::KeyDown:
			imgui::System::instance()->inject_key_press((input::InputKey::Code)event.key.keysym.sym, event.key.keysym.unicode);
			break;
		case input::InputEvent::KeyUp:
			imgui::System::instance()->inject_key_release((input::InputKey::Code)event.key.keysym.sym);
			break;
		case input::InputEvent::Character:
			imgui::System::instance()->inject_key_press(input::InputKey::Code::None, event.key.keysym.unicode);
			break;
		case input::InputEvent::LostFocus:
			imgui::System::instance()->inject_window_focus(false);
			break;
		case input::InputEvent::GetFocus:
			imgui::System::instance()->inject_window_focus(true);
			break;
		case input::InputEvent::MouseWheelUp:
			imgui::System::instance()->inject_mouse_wheel(1.0f);
			break;
		case input::InputEvent::MouseWheelDown:
			imgui::System::instance()->inject_mouse_wheel(-1.0f);
			break;
		case input::InputEvent::SizeChange:
			imgui::System::instance()->set_viewport(event.change.w, event.change.h);
			break;
		case input::InputEvent::SizeChangeDPI:
			imgui::System::instance()->set_framebuffer_scale(event.change.w, event.change.h);
			break;
		default:
			return;
		}
	}

	void
	GuiFeature::on_frame_begin() noexcept
	{
		imgui::new_frame();
	}

	void
	GuiFeature::on_frame() noexcept
	{
		static bool isOpened = true;
		if (isOpened)
			imgui::show_test_window(&isOpened);

		GameObjectManager::instance()->on_gui();
	}

	void
	GuiFeature::on_frame_end() noexcept
	{
		imgui::render();
		imgui::System::instance()->render();
	}
}