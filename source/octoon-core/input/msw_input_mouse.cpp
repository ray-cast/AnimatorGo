#include "msw_input_mouse.h"

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(MSWInputMouse, DefaultInputMouse, "MSWInputMouse")

		#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
		#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

		MSWInputMouse::MSWInputMouse() noexcept
			: focusWindow_(false)
			, window_(0)
		{
		}

		MSWInputMouse::MSWInputMouse(WindHandle window) noexcept
			: focusWindow_(false)
			, window_(window)
		{
		}

		MSWInputMouse::~MSWInputMouse() noexcept
		{
		}

		void
		MSWInputMouse::get_position(InputButton::Type& x, InputButton::Type& y) const noexcept
		{
			POINT pt;
			::GetCursorPos(&pt);
			::ScreenToClient(window_ ? (HWND)window_ : GetActiveWindow(), &pt);

			x = pt.x;
			y = pt.y;
		}

		bool
		MSWInputMouse::is_button_down(InputButton::Code key) const noexcept
		{
			bool down = DefaultInputMouse::is_button_down(key);
			if (down)
			{
				switch (key)
				{
				case InputButton::Left:
					return KEYUP(VK_LBUTTON) ? false : true;
				case InputButton::Right:
					return KEYUP(VK_RBUTTON) ? false : true;
				case InputButton::Middle:
					return KEYUP(VK_MBUTTON) ? false : true;
				default:
					return down;
				}
			}

			return down;
		}

		bool
		MSWInputMouse::is_button_up(InputButton::Code key) const noexcept
		{
			switch (key)
			{
			case InputButton::Left:
				return KEYUP(VK_LBUTTON);
			case InputButton::Right:
				return KEYUP(VK_RBUTTON);
			case InputButton::Middle:
				return KEYUP(VK_MBUTTON);
			default:
				return DefaultInputMouse::is_button_up(key);
			}
		}

		bool
		MSWInputMouse::is_button_pressed(InputButton::Code key) const noexcept
		{
			switch (key)
			{
			case InputButton::Left:
				return KEYDOWN(VK_LBUTTON);
			case InputButton::Right:
				return KEYDOWN(VK_RBUTTON);
			case InputButton::Middle:
				return KEYDOWN(VK_MBUTTON);
			default:
				return DefaultInputMouse::is_button_pressed(key);
			}
		}

		void
		MSWInputMouse::onShowMouse() noexcept
		{
			::ShowCursor(TRUE);
		}

		void
		MSWInputMouse::onHideMouse() noexcept
		{
			::ShowCursor(FALSE);
		}

		void
		MSWInputMouse::onChangePosition(InputButton::Type x, InputButton::Type y) noexcept
		{
			if (focusWindow_)
			{
				POINT pt;
				pt.x = static_cast<LONG>(x);
				pt.y = static_cast<LONG>(y);

				::ClientToScreen((HWND)window_, &pt);
				::SetCursorPos(pt.x, pt.y);
			}
		}

		void
		MSWInputMouse::on_input_event(const InputEvent& event) noexcept
		{
			switch (event.event)
			{
			case InputEvent::GetFocus:
			{
				window_ = (WindHandle)event.window.windowID;
				focusWindow_ = true;
			}
			break;
			case InputEvent::LostFocus:
			{
				window_ = (WindHandle)event.window.windowID;
				focusWindow_ = false;
			}
			break;
			default:
				break;
			}

			DefaultInputMouse::on_input_event(event);
		}

		IInputMousePtr
		MSWInputMouse::clone() const noexcept
		{
			return std::make_shared<MSWInputMouse>();
		}
	}
}