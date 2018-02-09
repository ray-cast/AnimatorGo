#include "glfw_input_keyboard.h"

namespace octoon
{
	namespace input
	{
		GLFWInputKeyboard::GLFWInputKeyboard() noexcept
		{
		}

		GLFWInputKeyboard::~GLFWInputKeyboard() noexcept
		{
		}

		bool
		GLFWInputKeyboard::getKeyState(InputKey::Code key) const noexcept
		{
			return false;
		}

		InputKeyboardPtr
		GLFWInputKeyboard::clone() const noexcept
		{
			return std::make_shared<GLFWInputKeyboard>();
		}
	}
}