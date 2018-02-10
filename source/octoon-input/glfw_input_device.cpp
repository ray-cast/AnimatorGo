#include "glfw_input_device.h"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubClass(GLFWInputDevice, IInputDevice, "GLFWInputDevice")

		InputKey::Code VirtualKeyToInputKey(int key) noexcept
		{
			switch (key)
			{
			case GLFW_KEY_UNKNOWN: return InputKey::Code::None;
			case GLFW_KEY_SPACE: return InputKey::Code::Space;
			case GLFW_KEY_APOSTROPHE: return InputKey::Code::Apostrophe;
			case GLFW_KEY_COMMA: return InputKey::Code::Comma;
			case GLFW_KEY_MINUS: return InputKey::Code::Minus;
			case GLFW_KEY_PERIOD: return InputKey::Code::Period;
			case GLFW_KEY_SLASH: return InputKey::Code::Slash;
			case GLFW_KEY_0: return InputKey::Code::Key0;
			case GLFW_KEY_1: return InputKey::Code::Key1;
			case GLFW_KEY_2: return InputKey::Code::Key2;
			case GLFW_KEY_3: return InputKey::Code::Key3;
			case GLFW_KEY_4: return InputKey::Code::Key4;
			case GLFW_KEY_5: return InputKey::Code::Key5;
			case GLFW_KEY_6: return InputKey::Code::Key6;
			case GLFW_KEY_7: return InputKey::Code::Key7;
			case GLFW_KEY_8: return InputKey::Code::Key8;
			case GLFW_KEY_9: return InputKey::Code::Key9;
			case GLFW_KEY_SEMICOLON: return InputKey::Code::Semicolon;
			case GLFW_KEY_EQUAL: return InputKey::Code::Equal;
			case GLFW_KEY_A: return InputKey::Code::A;
			case GLFW_KEY_B: return InputKey::Code::B;
			case GLFW_KEY_C: return InputKey::Code::C;
			case GLFW_KEY_D: return InputKey::Code::D;
			case GLFW_KEY_E: return InputKey::Code::E;
			case GLFW_KEY_F: return InputKey::Code::F;
			case GLFW_KEY_G: return InputKey::Code::G;
			case GLFW_KEY_H: return InputKey::Code::H;
			case GLFW_KEY_I: return InputKey::Code::I;
			case GLFW_KEY_J: return InputKey::Code::J;
			case GLFW_KEY_K: return InputKey::Code::K;
			case GLFW_KEY_L: return InputKey::Code::L;
			case GLFW_KEY_M: return InputKey::Code::M;
			case GLFW_KEY_N: return InputKey::Code::N;
			case GLFW_KEY_O: return InputKey::Code::O;
			case GLFW_KEY_P: return InputKey::Code::P;
			case GLFW_KEY_Q: return InputKey::Code::Q;
			case GLFW_KEY_R: return InputKey::Code::R;
			case GLFW_KEY_S: return InputKey::Code::S;
			case GLFW_KEY_T: return InputKey::Code::T;
			case GLFW_KEY_U: return InputKey::Code::U;
			case GLFW_KEY_V: return InputKey::Code::V;
			case GLFW_KEY_W: return InputKey::Code::W;
			case GLFW_KEY_X: return InputKey::Code::X;
			case GLFW_KEY_Y: return InputKey::Code::Y;
			case GLFW_KEY_Z: return InputKey::Code::Z;
			case GLFW_KEY_LEFT_BRACKET: return InputKey::Code::LeftBracket;
			case GLFW_KEY_BACKSLASH: return InputKey::Code::Backslash;
			case GLFW_KEY_RIGHT_BRACKET: return InputKey::Code::RightBracket;
			case GLFW_KEY_GRAVE_ACCENT: return InputKey::Code::GraveAccent;
			case GLFW_KEY_WORLD_1: return InputKey::Code::World1;
			case GLFW_KEY_WORLD_2: return InputKey::Code::World2;
			case GLFW_KEY_ESCAPE: return InputKey::Code::Escape;
			case GLFW_KEY_ENTER: return InputKey::Code::Enter;
			case GLFW_KEY_TAB: return InputKey::Code::Tab;
			case GLFW_KEY_BACKSPACE: return InputKey::Code::Backspace;
			case GLFW_KEY_INSERT: return InputKey::Code::Insert;
			case GLFW_KEY_DELETE: return InputKey::Code::Delete;
			case GLFW_KEY_RIGHT: return InputKey::Code::ArrowRight;
			case GLFW_KEY_LEFT: return InputKey::Code::ArrowLeft;
			case GLFW_KEY_DOWN: return InputKey::Code::ArrowDown;
			case GLFW_KEY_UP: return InputKey::Code::ArrowUp;
			case GLFW_KEY_PAGE_UP: return InputKey::Code::PageUp;
			case GLFW_KEY_PAGE_DOWN: return InputKey::Code::PageDown;
			case GLFW_KEY_HOME: return InputKey::Code::Home;
			case GLFW_KEY_END: return InputKey::Code::End;
			case GLFW_KEY_CAPS_LOCK: return InputKey::Code::CapsLock;
			case GLFW_KEY_SCROLL_LOCK: return InputKey::Code::ScrollLock;
			case GLFW_KEY_NUM_LOCK: return InputKey::Code::NumLock;
			case GLFW_KEY_PRINT_SCREEN: return InputKey::Code::PrintScreen;
			case GLFW_KEY_PAUSE: return InputKey::Code::Pause;
			case GLFW_KEY_F1: return InputKey::Code::F1;
			case GLFW_KEY_F2: return InputKey::Code::F2;
			case GLFW_KEY_F3: return InputKey::Code::F3;
			case GLFW_KEY_F4: return InputKey::Code::F4;
			case GLFW_KEY_F5: return InputKey::Code::F5;
			case GLFW_KEY_F6: return InputKey::Code::F6;
			case GLFW_KEY_F7: return InputKey::Code::F7;
			case GLFW_KEY_F8: return InputKey::Code::F8;
			case GLFW_KEY_F9: return InputKey::Code::F9;
			case GLFW_KEY_F10: return InputKey::Code::F10;
			case GLFW_KEY_F11: return InputKey::Code::F11;
			case GLFW_KEY_F12: return InputKey::Code::F12;
			case GLFW_KEY_F13: return InputKey::Code::F13;
			case GLFW_KEY_F14: return InputKey::Code::F14;
			case GLFW_KEY_F15: return InputKey::Code::F15;
			case GLFW_KEY_F16: return InputKey::Code::F16;
			case GLFW_KEY_F17: return InputKey::Code::F17;
			case GLFW_KEY_F18: return InputKey::Code::F18;
			case GLFW_KEY_F19: return InputKey::Code::F19;
			case GLFW_KEY_F20: return InputKey::Code::F20;
			case GLFW_KEY_F21: return InputKey::Code::F21;
			case GLFW_KEY_F22: return InputKey::Code::F22;
			case GLFW_KEY_F23: return InputKey::Code::F23;
			case GLFW_KEY_F24: return InputKey::Code::F24;
			case GLFW_KEY_F25: return InputKey::Code::F25;
			case GLFW_KEY_KP_0: return InputKey::Code::KP_0;
			case GLFW_KEY_KP_1: return InputKey::Code::KP_1;
			case GLFW_KEY_KP_2: return InputKey::Code::KP_2;
			case GLFW_KEY_KP_3: return InputKey::Code::KP_3;
			case GLFW_KEY_KP_4: return InputKey::Code::KP_4;
			case GLFW_KEY_KP_5: return InputKey::Code::KP_5;
			case GLFW_KEY_KP_6: return InputKey::Code::KP_6;
			case GLFW_KEY_KP_7: return InputKey::Code::KP_7;
			case GLFW_KEY_KP_8: return InputKey::Code::KP_8;
			case GLFW_KEY_KP_9: return InputKey::Code::KP_9;
			case GLFW_KEY_KP_DECIMAL: return InputKey::Code::KP_Decimal;
			case GLFW_KEY_KP_DIVIDE: return InputKey::Code::KP_Divide;
			case GLFW_KEY_KP_MULTIPLY: return InputKey::Code::KP_Multiply;
			case GLFW_KEY_KP_SUBTRACT: return InputKey::Code::KP_Subtract;
			case GLFW_KEY_KP_ADD: return InputKey::Code::KP_Add;
			case GLFW_KEY_KP_ENTER: return InputKey::Code::KP_Enter;
			case GLFW_KEY_KP_EQUAL: return InputKey::Code::KP_Equal;
			case GLFW_KEY_LEFT_SHIFT: return InputKey::Code::LeftShift;
			case GLFW_KEY_LEFT_CONTROL: return InputKey::Code::LeftControl;
			case GLFW_KEY_LEFT_ALT: return InputKey::Code::LeftAlt;
			case GLFW_KEY_LEFT_SUPER: return InputKey::Code::LeftSuper;
			case GLFW_KEY_RIGHT_SHIFT: return InputKey::Code::RightShift;
			case GLFW_KEY_RIGHT_CONTROL: return InputKey::Code::RightControl;
			case GLFW_KEY_RIGHT_ALT: return InputKey::Code::RightAlt;
			case GLFW_KEY_RIGHT_SUPER: return InputKey::Code::RightSuper;
			case GLFW_KEY_MENU: return InputKey::Code::Menu;
			default:
				return InputKey::Code::None;
			}
		}

		void onWindowResize(GLFWwindow* window, int w, int h)
		{
			auto input = (GLFWInputDevice*)glfwGetWindowUserPointer(window);
			if (input)
			{
				octoon::input::InputEvent event;
				event.event = octoon::input::InputEvent::SizeChange;
				event.change.w = w;
				event.change.h = h;
				event.change.windowID = (std::uint64_t)window;
				event.change.timestamp = ::glfwGetTimerFrequency();
				input->sendEvent(event);
			}
		}

		void onWindowFramebufferResize(GLFWwindow* window, int w, int h)
		{
			auto input = (GLFWInputDevice*)glfwGetWindowUserPointer(window);
			if (input)
			{
				octoon::input::InputEvent event;
				event.event = octoon::input::InputEvent::SizeChangeDPI;
				event.change.w = w;
				event.change.h = h;
				event.change.windowID = (std::uint64_t)window;
				event.change.timestamp = ::glfwGetTimerFrequency();
				input->sendEvent(event);
			}
		}

		void onWindowClose(GLFWwindow* window)
		{
			auto input = (GLFWInputDevice*)glfwGetWindowUserPointer(window);
			if (input)
			{
				octoon::input::InputEvent event;
				event.event = octoon::input::InputEvent::AppQuit;
				event.window.windowID = (std::uint64_t)window;
				event.window.timestamp = ::glfwGetTimerFrequency();
				input->sendEvent(event);
			}
		}

		void onWindowFocus(GLFWwindow* window, int focus)
		{
			auto input = (GLFWInputDevice*)glfwGetWindowUserPointer(window);
			if (input)
			{
				octoon::input::InputEvent event;
				event.event = focus ? octoon::input::InputEvent::GetFocus : octoon::input::InputEvent::LostFocus;
				event.window.windowID = (std::uint64_t)window;
				event.window.timestamp = ::glfwGetTimerFrequency();
				input->sendEvent(event);
			}
		}

		void onWindowKey(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			auto input = (GLFWInputDevice*)glfwGetWindowUserPointer(window);
			if (input)
			{
				octoon::input::InputEvent event;
				event.event = (action == GLFW_PRESS || action == GLFW_REPEAT) ? octoon::input::InputEvent::KeyDown : octoon::input::InputEvent::KeyUp;
				event.key.windowID = (std::uint64_t)window;
				event.key.timestamp = ::glfwGetTimerFrequency();
				event.key.padding2 = 0;
				event.key.padding3 = 0;
				event.key.repeat = (action == GLFW_REPEAT) ? true : false;
				event.key.state = ::glfwGetKey(window, key) == GLFW_PRESS ? true : false;
				event.key.keysym.raw = scancode;
				event.key.keysym.sym = VirtualKeyToInputKey(key);
				event.key.keysym.mod = mods;
				event.key.keysym.unicode = 0;

				input->sendEvent(event);
			}
		}

		void onWindowKeyChar(GLFWwindow* window, unsigned int unicode, int mods)
		{
			auto input = (GLFWInputDevice*)glfwGetWindowUserPointer(window);
			if (input)
			{
				octoon::input::InputEvent event;
				event.event = octoon::input::InputEvent::Character;
				event.key.windowID = (std::uint64_t)window;
				event.key.timestamp = ::glfwGetTimerFrequency();
				event.key.padding2 = 0;
				event.key.padding3 = 0;
				event.key.repeat = 0;
				event.key.state = true;
				event.key.keysym.raw = 0;
				event.key.keysym.sym = 0;
				event.key.keysym.mod = mods;
				event.key.keysym.unicode = unicode;

				input->sendEvent(event);
			}
		}

		void onWindowMouseButton(GLFWwindow* window, int button, int action, int mods)
		{
			auto input = (GLFWInputDevice*)glfwGetWindowUserPointer(window);
			if (input)
			{
				double mouseX, mouseY;
				::glfwGetCursorPos(window, &mouseX, &mouseY);

				octoon::input::InputEvent event;
				event.event = action == GLFW_PRESS ? octoon::input::InputEvent::MouseButtonDown : octoon::input::InputEvent::MouseButtonUp;
				event.button.button = octoon::input::InputButton::MOUSE0 + button;
				event.button.clicks = action == GLFW_PRESS ? true : false;
				event.button.x = mouseX;
				event.button.y = mouseY;
				event.button.timestamp = glfwGetTimerFrequency();
				event.button.windowID = (std::uint64_t)window;
				event.button.padding1 = 0;
				event.button.which = 0;

				input->sendEvent(event);

				if (action == GLFW_RELEASE)
				{
					static auto clicks = false;
					static auto before = std::chrono::system_clock::now();

					if (!clicks)
					{
						before = std::chrono::system_clock::now();
						clicks = true;
					}
					else
					{
						auto now = std::chrono::system_clock::now();
						double diff_ms = std::chrono::duration <double, std::milli>(now - before).count();
						if (diff_ms > 10 && diff_ms < 200)
						{
							octoon::input::InputEvent doubleClick;
							doubleClick.event = octoon::input::InputEvent::MouseButtonDoubleClick;
							doubleClick.button.button = octoon::input::InputButton::MOUSE0 + button;
							doubleClick.button.clicks = true;
							doubleClick.button.x = mouseX;
							doubleClick.button.y = mouseY;
							doubleClick.button.timestamp = glfwGetTimerFrequency();
							doubleClick.button.windowID = (std::uint64_t)window;
							doubleClick.button.padding1 = 0;
							doubleClick.button.which = 0;

							input->sendEvent(doubleClick);
						}

						clicks = false;
					}
				}
			}
		}

		void onWindowMouseMotion(GLFWwindow* window, double x, double y)
		{
			auto input = (GLFWInputDevice*)glfwGetWindowUserPointer(window);
			if (input)
			{
				octoon::input::InputEvent event;
				event.event = octoon::input::InputEvent::MouseMotion;
				event.motion.x = x;
				event.motion.y = y;
				event.motion.xrel = x;
				event.motion.yrel = y;
				event.motion.timestamp = glfwGetTimerFrequency();
				event.motion.state = false;
				event.motion.windowID = (std::uint64_t)window;

#if defined(OCTOON_BUILD_PLATFORM_WINDOWS)
				POINT pt;
				GetCursorPos(&pt);

				event.motion.xrel = pt.x;
				event.motion.yrel = pt.y;
#endif

				input->sendEvent(event);
			}
		}

		void onWindowSchool(GLFWwindow* window, double x, double y)
		{
			auto input = (GLFWInputDevice*)glfwGetWindowUserPointer(window);
			if (input)
			{
				octoon::input::InputEvent event;
				event.event = y > 0 ? octoon::input::InputEvent::MouseWheelUp : octoon::input::InputEvent::MouseWheelDown;
				event.wheel.timestamp = glfwGetTimerFrequency();
				event.wheel.windowID = (std::uint64_t)window;

				input->sendEvent(event);
			}
		}

		void onWindowDrop(GLFWwindow* window, int count, const char** file_utf8)
		{
			auto input = (GLFWInputDevice*)glfwGetWindowUserPointer(window);
			if (input)
			{
				octoon::input::InputEvent event;
				event.event = octoon::input::InputEvent::Drop;
				event.drop.timestamp = glfwGetTimerFrequency();
				event.drop.count = count;
				event.drop.files = file_utf8;
				event.drop.windowID = (std::uint64_t)window;

				input->sendEvent(event);
			}
		}

		GLFWInputDevice::GLFWInputDevice() noexcept
			: window_(nullptr)
		{
		}

		GLFWInputDevice::~GLFWInputDevice() noexcept
		{
		}

		void
		GLFWInputDevice::setCaptureObject(WindHandle window) noexcept
		{
			if (window_ != window)
			{
				GLFWwindow* hwnd = (GLFWwindow*)window;
				::glfwSetWindowUserPointer(hwnd, this);
				::glfwSetWindowFocusCallback(hwnd, &onWindowFocus);
				::glfwSetWindowCloseCallback(hwnd, &onWindowClose);
				::glfwSetWindowSizeCallback(hwnd, &onWindowResize);
				::glfwSetFramebufferSizeCallback(hwnd, &onWindowFramebufferResize);
				::glfwSetMouseButtonCallback(hwnd, &onWindowMouseButton);
				::glfwSetCursorPosCallback(hwnd, &onWindowMouseMotion);
				::glfwSetKeyCallback(hwnd, &onWindowKey);
				::glfwSetCharModsCallback(hwnd, &onWindowKeyChar);
				::glfwSetScrollCallback(hwnd, &onWindowSchool);
				::glfwSetDropCallback(hwnd, &onWindowDrop);

				window_ = window;
			}
		}

		WindHandle
		GLFWInputDevice::getCaptureObject() const noexcept
		{
			return window_;
		}

		void
		GLFWInputDevice::update() noexcept
		{
		}

		bool
		GLFWInputDevice::peekEvents(InputEvent& event) noexcept
		{
			this->update();
			return DefaultInputDevice::peekEvents(event);
		}

		bool
		GLFWInputDevice::pollEvents(InputEvent& event) noexcept
		{
			this->update();
			return DefaultInputDevice::pollEvents(event);
		}

		bool
		GLFWInputDevice::waitEvents(InputEvent& event) noexcept
		{
			this->update();
			return DefaultInputDevice::pollEvents(event);
		}

		bool
		GLFWInputDevice::waitEvents(InputEvent& event, int time) noexcept
		{
			this->update();
			return DefaultInputDevice::waitEvents(event, time);
		}

		void
		GLFWInputDevice::flushEvent() noexcept
		{
			this->update();
			DefaultInputDevice::flushEvent();
		}

		InputDevicePtr
		GLFWInputDevice::clone() const noexcept
		{
			return std::make_shared<GLFWInputDevice>();
		}
	}
}