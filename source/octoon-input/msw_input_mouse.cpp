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
		MSWInputMouse::getPosition(InputButton::mouse_t& x, InputButton::mouse_t& y) const noexcept
		{
			POINT pt;
			::GetCursorPos(&pt);
			::ScreenToClient(window_ ? (HWND)window_ : GetActiveWindow(), &pt);

			x = pt.x;
			y = pt.y;
		}

		bool
		MSWInputMouse::isButtonDown(InputButton::Code key) const noexcept
		{
			bool down = DefaultInputMouse::isButtonDown(key);
			if (down)
			{
				switch (key)
				{
				case InputButton::LEFT:
					return KEYUP(VK_LBUTTON) ? false : true;
				case InputButton::RIGHT:
					return KEYUP(VK_RBUTTON) ? false : true;
				case InputButton::MIDDLE:
					return KEYUP(VK_MBUTTON) ? false : true;
				default:
					return down;
				}
			}

			return down;
		}

		bool
		MSWInputMouse::isButtonUp(InputButton::Code key) const noexcept
		{
			switch (key)
			{
			case InputButton::LEFT:
				return KEYUP(VK_LBUTTON);
			case InputButton::RIGHT:
				return KEYUP(VK_RBUTTON);
			case InputButton::MIDDLE:
				return KEYUP(VK_MBUTTON);
			default:
				return DefaultInputMouse::isButtonUp(key);
			}
		}

		bool
		MSWInputMouse::isButtonPressed(InputButton::Code key) const noexcept
		{
			switch (key)
			{
			case InputButton::LEFT:
				return KEYDOWN(VK_LBUTTON);
			case InputButton::RIGHT:
				return KEYDOWN(VK_RBUTTON);
			case InputButton::MIDDLE:
				return KEYDOWN(VK_MBUTTON);
			default:
				return DefaultInputMouse::isButtonDown(key);
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
		MSWInputMouse::onChangePosition(InputButton::mouse_t x, InputButton::mouse_t y) noexcept
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
		MSWInputMouse::onInputEvent(const InputEvent& event) noexcept
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

			DefaultInputMouse::onInputEvent(event);
		}

		InputMousePtr
		MSWInputMouse::clone() const noexcept
		{
			return std::make_shared<MSWInputMouse>();
		}
	}
}