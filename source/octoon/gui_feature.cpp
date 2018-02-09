#include <octoon/gui_feature.h>

#include <octoon/input_feature.h>

#include <octoon/game_server.h>
#include <octoon/game_application.h>
#include <octoon/input/input_event.h>

//#include <ray/imgui.h>
//#include <ray/imgui_system.h>

namespace octoon
{
	OctoonImplementSubClass(GuiFeature, GameFeature, "GuiFeature")

	GuiFeature::GuiFeature() noexcept
		: _window(0)
		, _width(0)
		, _height(0)
		, _framebuffer_w(0)
		, _framebuffer_h(0)
		, _dpi(1.0)
	{
	}

	GuiFeature::GuiFeature(WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h, float dpi) noexcept
		: _window(window)
		, _width(w)
		, _height(h)
		, _framebuffer_w(framebuffer_w)
		, _framebuffer_h(framebuffer_h)
		, _dpi(dpi)
	{
	}

	GuiFeature::~GuiFeature() noexcept
	{
	}

	void
	GuiFeature::setViewport(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (_width != w || _height != h)
		{
			_width = w;
			_height = h;

			//IMGUISystem::instance()->setViewport(w, h);
		}
	}

	void
	GuiFeature::getViewport(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		//IMGUISystem::instance()->getViewport(w, h);
	}

	void
	GuiFeature::setWindowFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (_framebuffer_w != w || _framebuffer_h != h)
		{
			_framebuffer_w = w;
			_framebuffer_h = h;

			//IMGUISystem::instance()->setFramebufferScale(w, h);
		}
	}

	void
	GuiFeature::getWindowFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		//IMGUISystem::instance()->getFramebufferScale(w, h);
	}

	void
	GuiFeature::onActivate() except
	{
		/*if (!IMGUISystem::instance()->open(_window, _dpi))
			throw failure("GuiSystem::instance() fail");

		IMGUISystem::instance()->setImageLoader(std::make_shared<ImageLoader>());
		IMGUISystem::instance()->setViewport(_width, _height);
		IMGUISystem::instance()->setFramebufferScale(_framebuffer_w, _framebuffer_h);*/
	}

	void
	GuiFeature::onDeactivate() noexcept
	{
		//IMGUISystem::instance()->close();
	}

	void
	GuiFeature::onInputEvent(const input::InputEvent& event) noexcept
	{
		/*switch (event.event)
		{
		case input::InputEvent::MouseMotion:
			IMGUISystem::instance()->injectMouseMove(event.motion.x, event.motion.y);
			break;
		case input::InputEvent::MouseButtonDown:
			IMGUISystem::instance()->injectMousePress(event.button.x, event.button.y, (input::InputButton::Code)event.button.button);
			break;
		case input::InputEvent::MouseButtonUp:
			IMGUISystem::instance()->injectMouseRelease(event.button.x, event.button.y, (input::InputButton::Code)event.button.button);
			break;
		case input::InputEvent::KeyDown:
			IMGUISystem::instance()->injectKeyPress((input::InputKey::Code)event.key.keysym.sym, event.key.keysym.unicode);
			break;
		case input::InputEvent::KeyUp:
			IMGUISystem::instance()->injectKeyRelease((input::InputKey::Code)event.key.keysym.sym);
			break;
		case input::InputEvent::Character:
			IMGUISystem::instance()->injectKeyPress(input::InputKey::Code::None, event.key.keysym.unicode);
			break;
		case input::InputEvent::LostFocus:
			IMGUISystem::instance()->injectWindowFocus(false);
			break;
		case input::InputEvent::GetFocus:
			IMGUISystem::instance()->injectWindowFocus(true);
			break;
		case input::InputEvent::MouseWheelUp:
		case input::InputEvent::MouseWheelDown:
			IMGUISystem::instance()->injectMouseWheel((event.event == input::InputEvent::MouseWheelUp) ? 1.0f : -1.0f);
			break;
		case input::InputEvent::SizeChange:
			IMGUISystem::instance()->setViewport(event.change.w, event.change.h);
			break;
		case input::InputEvent::SizeChangeDPI:
			IMGUISystem::instance()->setFramebufferScale(event.change.w, event.change.h);
			break;
		default:
			return;
		}*/
	}
}