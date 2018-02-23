#ifndef OCTOON_INPUT_GLFW_MOUSE_H_
#define OCTOON_INPUT_GLFW_MOUSE_H_

#include <octoon/input/input_mouse.h>

namespace octoon
{
	namespace input
	{
		class GLFWInputMouse final : public DefaultInputMouse
		{
		public:
			GLFWInputMouse() noexcept;
			~GLFWInputMouse() noexcept;

			IInputMousePtr clone() const noexcept;

		private:
			void on_show_mouse() noexcept;
			void on_hide_mouse() noexcept;

			void on_change_position(int x, int y) noexcept;

			void on_input_event(const InputEvent& event) noexcept override;

		private:
			GLFWInputMouse(const GLFWInputMouse&) = delete;
			GLFWInputMouse& operator=(const GLFWInputMouse&) = delete;

		private:
			bool focusWindow_;
			WindHandle window_;
		};
	}
}

#endif