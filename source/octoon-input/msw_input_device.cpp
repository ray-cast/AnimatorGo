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
#include "msw_input_device.h"

namespace octoon
{
	namespace input
	{
		OctoonImplementSubClass(MSWInputDevice, InputDevice, "MSWInputDevice")

		InputKey::Code VirtualKeyToScanCode(HWND hwnd, WPARAM wParam, LPARAM lParam)
		{
			UINT flags = HIWORD(lParam);

			if (flags & 0x100)
			{
				switch (MapVirtualKey(flags & 0xFF, 1))
				{
				case VK_INSERT:   return InputKey::Code::Insert;
				case VK_END:      return InputKey::Code::End;
				case VK_DOWN:     return InputKey::Code::ArrowDown;
				case VK_LEFT:     return InputKey::Code::ArrowLeft;
				case VK_RIGHT:    return InputKey::Code::ArrowRight;
				case VK_UP:       return InputKey::Code::ArrowUp;
				case VK_NEXT:     return InputKey::Code::PageDown;
				case VK_CLEAR:    return InputKey::Code::None;
				case VK_HOME:     return InputKey::Code::Home;
				case VK_PRIOR:    return InputKey::Code::PageUp;
				case VK_DIVIDE:   return InputKey::Code::KP_Divide;
				case VK_MULTIPLY: return InputKey::Code::KP_Multiply;
				case VK_SUBTRACT: return InputKey::Code::KP_Subtract;
				case VK_ADD:      return InputKey::Code::KP_Add;
				case VK_DELETE:   return InputKey::Code::Delete;
				}
			}

			DWORD scan_code;

			switch (wParam)
			{
			case VK_SHIFT:
			{
				scan_code = MapVirtualKey(VK_RSHIFT, 0);
				if ((DWORD)(lParam & 0x01ff0000 >> 16) == scan_code)
				{
					return InputKey::Code::RightShift;
				}

				return InputKey::Code::LeftShift;
			}
			case VK_CONTROL:
			{
				if (lParam & 0x01000000)
				{
					return InputKey::Code::RightControl;
				}

				DWORD time = (DWORD)::GetMessageTime();

				MSG msg;
				if (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
				{
					if (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN)
					{
						if (msg.wParam == VK_MENU &&
							msg.lParam & 0x01000000 &&
							msg.time == time)
						{
							return InputKey::Code::None;
						}
					}
				}

				return InputKey::Code::LeftControl;
			}

			case VK_MENU:
			{
				if (lParam & 0x01000000)
					return InputKey::Code::RightAlt;

				return InputKey::Code::LeftAlt;
			}

			case VK_RETURN:
			{
				if (lParam & 0x01000000)
				{
					return InputKey::Code::Enter;
				}

				return InputKey::Code::Enter;
			}
			case VK_ESCAPE:        return InputKey::Code::Escape;
			case VK_TAB:           return InputKey::Code::Tab;
			case VK_BACK:          return InputKey::Code::Backspace;
			case VK_HOME:          return InputKey::Code::Home;
			case VK_END:           return InputKey::Code::End;
			case VK_PRIOR:         return InputKey::Code::PageUp;
			case VK_NEXT:          return InputKey::Code::PageDown;
			case VK_INSERT:        return InputKey::Code::Insert;
			case VK_DELETE:        return InputKey::Code::Delete;
			case VK_LEFT:          return InputKey::Code::ArrowLeft;
			case VK_UP:            return InputKey::Code::ArrowUp;
			case VK_RIGHT:         return InputKey::Code::ArrowRight;
			case VK_DOWN:          return InputKey::Code::ArrowDown;
			case VK_F1:            return InputKey::Code::F1;
			case VK_F2:            return InputKey::Code::F2;
			case VK_F3:            return InputKey::Code::F3;
			case VK_F4:            return InputKey::Code::F4;
			case VK_F5:            return InputKey::Code::F5;
			case VK_F6:            return InputKey::Code::F6;
			case VK_F7:            return InputKey::Code::F7;
			case VK_F8:            return InputKey::Code::F8;
			case VK_F9:            return InputKey::Code::F9;
			case VK_F10:           return InputKey::Code::F10;
			case VK_F11:           return InputKey::Code::F11;
			case VK_F12:           return InputKey::Code::F12;
			case VK_F13:           return InputKey::Code::F13;
			case VK_F14:           return InputKey::Code::F14;
			case VK_F15:           return InputKey::Code::F15;
			case VK_F16:           return InputKey::Code::F16;
			case VK_F17:           return InputKey::Code::F17;
			case VK_F18:           return InputKey::Code::F18;
			case VK_F19:           return InputKey::Code::F19;
			case VK_F20:           return InputKey::Code::F20;
			case VK_F21:           return InputKey::Code::F21;
			case VK_F22:           return InputKey::Code::F22;
			case VK_F23:           return InputKey::Code::F23;
			case VK_F24:           return InputKey::Code::F24;

			case VK_SPACE:         return InputKey::Code::Space;

			case VK_NUMPAD0:       return InputKey::Code::KP_0;
			case VK_NUMPAD1:       return InputKey::Code::KP_1;
			case VK_NUMPAD2:       return InputKey::Code::KP_2;
			case VK_NUMPAD3:       return InputKey::Code::KP_3;
			case VK_NUMPAD4:       return InputKey::Code::KP_4;
			case VK_NUMPAD5:       return InputKey::Code::KP_5;
			case VK_NUMPAD6:       return InputKey::Code::KP_6;
			case VK_NUMPAD7:       return InputKey::Code::KP_7;
			case VK_NUMPAD8:       return InputKey::Code::KP_8;
			case VK_NUMPAD9:       return InputKey::Code::KP_9;

			case VK_DIVIDE:        return InputKey::Code::KP_Divide;
			case VK_MULTIPLY:      return InputKey::Code::KP_Multiply;
			case VK_SUBTRACT:      return InputKey::Code::KP_Subtract;
			case VK_ADD:           return InputKey::Code::KP_Add;
			case VK_DECIMAL:       return InputKey::Code::KP_Decimal;
			case VK_NUMLOCK:       return InputKey::Code::NumLock;

			case VK_CAPITAL:       return InputKey::Code::CapsLock;
			case VK_SCROLL:        return InputKey::Code::ScrollLock;
			case VK_PAUSE:         return InputKey::Code::Pause;

			case VK_LWIN:          return InputKey::Code::Menu;
			case VK_RWIN:          return InputKey::Code::Menu;
			case VK_APPS:          return InputKey::Code::None;
			}

			if (wParam >= 'a' && wParam <= 'z')
			{
				return (InputKey::Code)(InputKey::Code::A + wParam - 'a');
			}

			if (wParam >= 'A' && wParam <= 'Z')
			{
				return (InputKey::Code)(InputKey::Code::A + wParam - 'A');
			}

			if (wParam >= '0' && wParam <= '9')
			{
				return (InputKey::Code)(InputKey::Code::Key0 + wParam - '0');
			}

			return InputKey::Code::None;
		}

		int VirtualKeyToText(WPARAM _virtualKey)
		{
			static WCHAR deadKey = 0;

			BYTE keyState[256];
			HKL  layout = GetKeyboardLayout(0);
			if (GetKeyboardState(keyState) == 0)
				return 0;

			WCHAR buff[3] = { 0, 0, 0 };
			int ascii = ToUnicodeEx((UINT)_virtualKey, 0, keyState, buff, 3, 0, layout);
			if (ascii == 1 && deadKey != '\0')
			{
				// A dead key is stored and we have just converted a character key
				// Combine the two into a single character
				WCHAR wcBuff[3] = { buff[0], deadKey, '\0' };
				WCHAR out[3];

				deadKey = '\0';
				if (FoldStringW(MAP_PRECOMPOSED, (LPWSTR)wcBuff, 3, (LPWSTR)out, 3))
					return out[0];
			}
			else if (ascii == 1)
			{
				// We have a single character
				deadKey = '\0';
				return buff[0];
			}
			else if (ascii == 2)
			{
				// Convert a non-combining diacritical mark into a combining diacritical mark
				// Combining versions range from 0x300 to 0x36F; only 5 (for French) have been mapped below
				// http://www.fileformat.info/info/unicode/block/combining_diacritical_marks/images.htm
				switch (buff[0])
				{
				case 0x5E: // Circumflex accent: §Ó
					deadKey = 0x302;
					break;
				case 0x60: // Grave accent: §Ñ
					deadKey = 0x300;
					break;
				case 0xA8: // Diaeresis: §E
					deadKey = 0x308;
					break;
				case 0xB4: // Acute accent: §Û
					deadKey = 0x301;
					break;
				case 0xB8: // Cedilla: §Ù
					deadKey = 0x327;
					break;
				default:
					deadKey = buff[0];
					break;
				}
			}
			return 0;
		}

		MSWInputDevice::MSWInputDevice() noexcept
			: window_(nullptr)
		{
		}

		MSWInputDevice::~MSWInputDevice() noexcept
		{
		}

		void
		MSWInputDevice::setCaptureObject(WindHandle window) noexcept
		{
			assert(::IsWindow((HWND)window));
			window_ = (HWND)window;
		}

		WindHandle
		MSWInputDevice::getCaptureObject() const noexcept
		{
			return window_;
		}

		void
		MSWInputDevice::update() noexcept
		{
			MSG msg;

			while (::PeekMessage(&msg, window_, 0, 0, PM_REMOVE | PM_QS_INPUT))
			{
				switch (msg.message)
				{
				case WM_KEYDOWN:
				case WM_KEYUP:
				{
					InputEvent inputEvent;
					inputEvent.event = (msg.message == WM_KEYDOWN) ? InputEvent::KeyDown : InputEvent::KeyUp;
					inputEvent.key.windowID = (std::uint64_t)msg.hwnd;
					inputEvent.key.timestamp = ::timeGetTime();
					inputEvent.key.state = (msg.message == WM_KEYDOWN) ? true : false;

					if (msg.wParam == VK_PROCESSKEY)
					{
						UINT virtualKey = ::ImmGetVirtualKey(msg.hwnd);
						if (virtualKey != VK_PROCESSKEY)
						{
							inputEvent.key.keysym.raw = virtualKey;
							inputEvent.key.keysym.sym = VirtualKeyToScanCode(msg.hwnd, virtualKey, msg.lParam);
							if (msg.message == WM_KEYDOWN)
								inputEvent.key.keysym.unicode = VirtualKeyToText(virtualKey);
							else
								inputEvent.key.keysym.unicode = 0;
						}
					}
					else
					{
						inputEvent.key.keysym.raw = msg.wParam;
						inputEvent.key.keysym.sym = VirtualKeyToScanCode(window_, msg.wParam, msg.lParam);
						if (msg.message == WM_KEYDOWN)
							inputEvent.key.keysym.unicode = VirtualKeyToText(msg.wParam);
						else
							inputEvent.key.keysym.unicode = 0;
					}

					this->sendEvent(inputEvent);
				}
				break;
				case WM_MOUSEMOVE:
				{
					POINT pt;
					GetCursorPos(&pt);

					InputEvent inputEvent;
					inputEvent.event = InputEvent::MouseMotion;
					inputEvent.motion.windowID = (std::uint64_t)msg.hwnd;
					inputEvent.motion.x = LOWORD(msg.lParam);
					inputEvent.motion.y = HIWORD(msg.lParam);
					inputEvent.motion.xrel = pt.x;
					inputEvent.motion.yrel = pt.y;
					inputEvent.motion.state = isButtonPress_;
					inputEvent.motion.timestamp = ::timeGetTime();
					inputEvent.button.button = button_;

					mouseX_ = LOWORD(msg.lParam);
					mouseY_ = HIWORD(msg.lParam);

					this->sendEvent(inputEvent);
				}
				break;
				case WM_LBUTTONDOWN:
				case WM_MBUTTONDOWN:
				case WM_RBUTTONDOWN:
				{
					SetCapture(msg.hwnd);

					InputEvent inputEvent;
					inputEvent.event = InputEvent::MouseButtonDown;
					inputEvent.button.windowID = (std::uint64_t)msg.hwnd;
					inputEvent.button.clicks = isButtonPress_ = true;
					inputEvent.button.x = mouseX_;
					inputEvent.button.y = mouseY_;
					inputEvent.button.timestamp = ::timeGetTime();
					switch (msg.message)
					{
					case WM_LBUTTONDOWN:
						button_ = inputEvent.button.button = InputButton::LEFT;
						break;
					case WM_MBUTTONDOWN:
						button_ = inputEvent.button.button = InputButton::MIDDLE;
						break;
					case WM_RBUTTONDOWN:
						button_ = inputEvent.button.button = InputButton::RIGHT;
						break;
					}

					this->sendEvent(inputEvent);
				}
				break;
				case WM_LBUTTONUP:
				case WM_MBUTTONUP:
				case WM_RBUTTONUP:
				{
					ReleaseCapture();

					InputEvent inputEvent;
					inputEvent.event = InputEvent::MouseButtonUp;
					inputEvent.button.clicks = isButtonPress_ = false;
					inputEvent.button.x = mouseX_;
					inputEvent.button.y = mouseY_;
					inputEvent.button.timestamp = ::timeGetTime();
					switch (msg.message)
					{
					case WM_LBUTTONUP:
						button_ = inputEvent.button.button = InputButton::LEFT;
						break;
					case WM_MBUTTONUP:
						button_ = inputEvent.button.button = InputButton::MIDDLE;
						break;
					case WM_RBUTTONUP:
						button_ = inputEvent.button.button = InputButton::RIGHT;
						break;
					default:
						assert(false);
					}

					this->sendEvent(inputEvent);
				}
				break;
				}

				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}

		bool
		MSWInputDevice::peekEvents(InputEvent& event) noexcept
		{
			this->update();
			return DefaultInputDevice::peekEvents(event);
		}

		bool
		MSWInputDevice::pollEvents(InputEvent& event) noexcept
		{
			this->update();
			return DefaultInputDevice::pollEvents(event);
		}

		bool
		MSWInputDevice::waitEvents(InputEvent& event) noexcept
		{
			this->update();
			return DefaultInputDevice::pollEvents(event);
		}

		bool
		MSWInputDevice::waitEvents(InputEvent& event, int time) noexcept
		{
			this->update();
			return DefaultInputDevice::waitEvents(event, time);
		}

		void
		MSWInputDevice::flushEvent() noexcept
		{
			this->update();
			DefaultInputDevice::flushEvent();
		}

		InputDevicePtr
		MSWInputDevice::clone() const noexcept
		{
			return std::make_shared<MSWInputDevice>();
		}
	}
}