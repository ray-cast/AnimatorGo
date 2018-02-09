// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#include "msw_input_mouse.h"

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(MSWInputMouse, DefaultInputMouse, "MSWInputMouse")

		#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
		#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

		MSWInputMouse::MSWInputMouse() noexcept
			: _focusWindow(false)
			, _window(0)
		{
		}

		MSWInputMouse::MSWInputMouse(WindHandle window) noexcept
			: _focusWindow(false)
			, _window(window)
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
			::ScreenToClient(_window ? (HWND)_window : GetActiveWindow(), &pt);

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
			if (_focusWindow)
			{
				POINT pt;
				pt.x = static_cast<LONG>(x);
				pt.y = static_cast<LONG>(y);

				::ClientToScreen((HWND)_window, &pt);
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
				_window = (WindHandle)event.window.windowID;
				_focusWindow = true;
			}
			break;
			case InputEvent::LostFocus:
			{
				_window = (WindHandle)event.window.windowID;
				_focusWindow = false;
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