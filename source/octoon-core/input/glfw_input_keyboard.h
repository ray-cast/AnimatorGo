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

			virtual bool is_key_down(InputKey::Code key) const noexcept override;
			virtual bool is_key_up(InputKey::Code key) const noexcept override;
			virtual bool is_key_pressed(InputKey::Code key) const noexcept override;

			virtual IInputKeyboardPtr clone() const noexcept override;

		private:
			void on_input_event(const InputEvent& event) noexcept override;

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