#ifndef OCTOON_INPUT_GLFW_KEYBOARD_H_
#define OCTOON_INPUT_GLFW_KEYBOARD_H_

#include <octoon/input/input_keyboard.h>

namespace octoon
{
	namespace input
	{
		class GLFWInputKeyboard final : public DefaultInputKeyboard
		{
		public:
			GLFWInputKeyboard() noexcept;
			virtual ~GLFWInputKeyboard() noexcept;

			virtual bool getKeyState(InputKey::Code key) const noexcept;

			virtual InputKeyboardPtr clone() const noexcept;

		private:
			void onInputEvent(const InputEvent& event) noexcept;

		private:
			GLFWInputKeyboard(const GLFWInputKeyboard&) noexcept = delete;
			GLFWInputKeyboard& operator=(const GLFWInputKeyboard&) noexcept = delete;

		private:
			bool focusWindow_;
			WindHandle window_;
		};
	}
}

#endif