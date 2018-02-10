#include "glfw_input_keyboard.h"

#include <GLFW\glfw3.h>

namespace octoon
{
	namespace input
	{
		int InputKeyToVirtualKey(InputKey::Code key) noexcept
		{
			switch (key)
			{
			case InputKey::Code::None: return GLFW_KEY_UNKNOWN;
			case InputKey::Code::Space: return GLFW_KEY_SPACE;
			case InputKey::Code::Apostrophe: return GLFW_KEY_APOSTROPHE;
			case InputKey::Code::Comma: return GLFW_KEY_COMMA;
			case InputKey::Code::Minus: return GLFW_KEY_MINUS;
			case InputKey::Code::Period: return GLFW_KEY_PERIOD;
			case InputKey::Code::Slash: return GLFW_KEY_SLASH;
			case InputKey::Code::Key0: return GLFW_KEY_0;
			case InputKey::Code::Key1: return GLFW_KEY_1;
			case InputKey::Code::Key2: return GLFW_KEY_2;
			case InputKey::Code::Key3: return GLFW_KEY_3;
			case InputKey::Code::Key4: return GLFW_KEY_4;
			case InputKey::Code::Key5: return GLFW_KEY_5;
			case InputKey::Code::Key6: return GLFW_KEY_6;
			case InputKey::Code::Key7: return GLFW_KEY_7;
			case InputKey::Code::Key8: return GLFW_KEY_8;
			case InputKey::Code::Key9: return GLFW_KEY_9;
			case InputKey::Code::Semicolon: return GLFW_KEY_SEMICOLON;
			case InputKey::Code::Equal: return GLFW_KEY_EQUAL;
			case InputKey::Code::A: return GLFW_KEY_A;
			case InputKey::Code::B: return GLFW_KEY_B;
			case InputKey::Code::C: return GLFW_KEY_C;
			case InputKey::Code::D: return GLFW_KEY_D;
			case InputKey::Code::E: return GLFW_KEY_E;
			case InputKey::Code::F: return GLFW_KEY_F;
			case InputKey::Code::G: return GLFW_KEY_G;
			case InputKey::Code::H: return GLFW_KEY_H;
			case InputKey::Code::I: return GLFW_KEY_I;
			case InputKey::Code::J: return GLFW_KEY_J;
			case InputKey::Code::K: return GLFW_KEY_K;
			case InputKey::Code::L: return GLFW_KEY_L;
			case InputKey::Code::M: return GLFW_KEY_M;
			case InputKey::Code::N: return GLFW_KEY_N;
			case InputKey::Code::O: return GLFW_KEY_O;
			case InputKey::Code::P: return GLFW_KEY_P;
			case InputKey::Code::Q: return GLFW_KEY_Q;
			case InputKey::Code::R: return GLFW_KEY_R;
			case InputKey::Code::S: return GLFW_KEY_S;
			case InputKey::Code::T: return GLFW_KEY_T;
			case InputKey::Code::U: return GLFW_KEY_U;
			case InputKey::Code::V: return GLFW_KEY_V;
			case InputKey::Code::W: return GLFW_KEY_W;
			case InputKey::Code::X: return GLFW_KEY_X;
			case InputKey::Code::Y: return GLFW_KEY_Y;
			case InputKey::Code::Z: return GLFW_KEY_Z;
			case InputKey::Code::LeftBracket: return GLFW_KEY_LEFT_BRACKET;
			case InputKey::Code::Backslash: return GLFW_KEY_BACKSLASH;
			case InputKey::Code::RightBracket: return GLFW_KEY_RIGHT_BRACKET;
			case InputKey::Code::GraveAccent: return GLFW_KEY_GRAVE_ACCENT;
			case InputKey::Code::World1: return GLFW_KEY_WORLD_1;
			case InputKey::Code::World2: return GLFW_KEY_WORLD_2;
			case InputKey::Code::Escape: return GLFW_KEY_ESCAPE;
			case InputKey::Code::Enter: return GLFW_KEY_ENTER;
			case InputKey::Code::Tab: return GLFW_KEY_TAB;
			case InputKey::Code::Backspace: return GLFW_KEY_BACKSPACE;
			case InputKey::Code::Insert: return GLFW_KEY_INSERT;
			case InputKey::Code::Delete: return GLFW_KEY_DELETE;
			case InputKey::Code::ArrowRight: return GLFW_KEY_RIGHT;
			case InputKey::Code::ArrowLeft: return GLFW_KEY_LEFT;
			case InputKey::Code::ArrowDown: return GLFW_KEY_DOWN;
			case InputKey::Code::ArrowUp: return GLFW_KEY_UP;
			case InputKey::Code::PageUp: return GLFW_KEY_PAGE_UP;
			case InputKey::Code::PageDown: return GLFW_KEY_PAGE_DOWN;
			case InputKey::Code::Home: return GLFW_KEY_HOME;
			case InputKey::Code::End: return GLFW_KEY_END;
			case InputKey::Code::CapsLock: return GLFW_KEY_CAPS_LOCK;
			case InputKey::Code::ScrollLock: return GLFW_KEY_SCROLL_LOCK;
			case InputKey::Code::NumLock: return GLFW_KEY_NUM_LOCK;
			case InputKey::Code::PrintScreen: return GLFW_KEY_PRINT_SCREEN;
			case InputKey::Code::Pause: return GLFW_KEY_PAUSE;
			case InputKey::Code::F1: return GLFW_KEY_F1;
			case InputKey::Code::F2: return GLFW_KEY_F2;
			case InputKey::Code::F3: return GLFW_KEY_F3;
			case InputKey::Code::F4: return GLFW_KEY_F4;
			case InputKey::Code::F5: return GLFW_KEY_F5;
			case InputKey::Code::F6: return GLFW_KEY_F6;
			case InputKey::Code::F7: return GLFW_KEY_F7;
			case InputKey::Code::F8: return GLFW_KEY_F8;
			case InputKey::Code::F9: return GLFW_KEY_F9;
			case InputKey::Code::F10: return GLFW_KEY_F10;
			case InputKey::Code::F11: return GLFW_KEY_F11;
			case InputKey::Code::F12: return GLFW_KEY_F12;
			case InputKey::Code::F13: return GLFW_KEY_F13;
			case InputKey::Code::F14: return GLFW_KEY_F14;
			case InputKey::Code::F15: return GLFW_KEY_F15;
			case InputKey::Code::F16: return GLFW_KEY_F16;
			case InputKey::Code::F17: return GLFW_KEY_F17;
			case InputKey::Code::F18: return GLFW_KEY_F18;
			case InputKey::Code::F19: return GLFW_KEY_F19;
			case InputKey::Code::F20: return GLFW_KEY_F20;
			case InputKey::Code::F21: return GLFW_KEY_F21;
			case InputKey::Code::F22: return GLFW_KEY_F22;
			case InputKey::Code::F23: return GLFW_KEY_F23;
			case InputKey::Code::F24: return GLFW_KEY_F24;
			case InputKey::Code::F25: return GLFW_KEY_F25;
			case InputKey::Code::KP_0: return GLFW_KEY_KP_0;
			case InputKey::Code::KP_1: return GLFW_KEY_KP_1;
			case InputKey::Code::KP_2: return GLFW_KEY_KP_2;
			case InputKey::Code::KP_3: return GLFW_KEY_KP_3;
			case InputKey::Code::KP_4: return GLFW_KEY_KP_4;
			case InputKey::Code::KP_5: return GLFW_KEY_KP_5;
			case InputKey::Code::KP_6: return GLFW_KEY_KP_6;
			case InputKey::Code::KP_7: return GLFW_KEY_KP_7;
			case InputKey::Code::KP_8: return GLFW_KEY_KP_8;
			case InputKey::Code::KP_9: return GLFW_KEY_KP_9;
			case InputKey::Code::KP_Decimal: return GLFW_KEY_KP_DECIMAL;
			case InputKey::Code::KP_Divide: return GLFW_KEY_KP_DIVIDE;
			case InputKey::Code::KP_Multiply: return GLFW_KEY_KP_MULTIPLY;
			case InputKey::Code::KP_Subtract: return GLFW_KEY_KP_SUBTRACT;
			case InputKey::Code::KP_Add: return GLFW_KEY_KP_ADD;
			case InputKey::Code::KP_Enter: return GLFW_KEY_KP_ENTER;
			case InputKey::Code::KP_Equal: return GLFW_KEY_KP_EQUAL;
			case InputKey::Code::LeftShift: return GLFW_KEY_LEFT_SHIFT;
			case InputKey::Code::LeftControl: return GLFW_KEY_LEFT_CONTROL;
			case InputKey::Code::LeftAlt: return GLFW_KEY_LEFT_ALT;
			case InputKey::Code::LeftSuper: return GLFW_KEY_LEFT_SUPER;
			case InputKey::Code::RightShift: return GLFW_KEY_RIGHT_SHIFT;
			case InputKey::Code::RightControl: return GLFW_KEY_RIGHT_CONTROL;
			case InputKey::Code::RightAlt: return GLFW_KEY_RIGHT_ALT;
			case InputKey::Code::RightSuper: return GLFW_KEY_RIGHT_SUPER;
			case InputKey::Code::Menu: return GLFW_KEY_MENU;
			default:
				return InputKey::Code::None;
			}
		}

		GLFWInputKeyboard::GLFWInputKeyboard() noexcept
			: window_(nullptr)
			, focusWindow_(false)
		{
		}

		GLFWInputKeyboard::~GLFWInputKeyboard() noexcept
		{
		}

		bool
		GLFWInputKeyboard::getKeyState(InputKey::Code key) const noexcept
		{
			if (!window_)
				return false;
			return ::glfwGetKey((GLFWwindow*)window_, InputKeyToVirtualKey(key)) == GLFW_PRESS ? true : false;;
		}

		void
		GLFWInputKeyboard::onInputEvent(const InputEvent& event) noexcept
		{
			GLFWInputKeyboard::onInputEvent(event);

			switch (event.event)
			{
			case InputEvent::GetFocus:
			{
				window_ = (GLFWwindow*)event.window.windowID;
				focusWindow_ = true;
			}
			break;
			case InputEvent::LostFocus:
			{
				window_ = (GLFWwindow*)event.window.windowID;
				focusWindow_ = false;
			}
			break;
			default:
				break;
			}
		}

		InputKeyboardPtr
		GLFWInputKeyboard::clone() const noexcept
		{
			return std::make_shared<GLFWInputKeyboard>();
		}
	}
}