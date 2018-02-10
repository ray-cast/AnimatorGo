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

			InputMousePtr clone() const noexcept;

		private:
			void onShowMouse() noexcept;
			void onHideMouse() noexcept;

			void onChangePosition(int x, int y) noexcept;

			void onInputEvent(const InputEvent& event) noexcept;

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