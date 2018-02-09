#include "glfw_input_mouse.h"
#include <GLFW\glfw3.h>

namespace octoon
{
	namespace input
	{
		GLFWInputMouse::GLFWInputMouse() noexcept
			: _window(window)
		{
		}

		GLFWInputMouse::~GLFWInputMouse() noexcept
		{
		}

		void
		GLFWInputMouse::onShowMouse() noexcept
		{
			if (_focusWindow)
			{
			   ::glfwSetInputMode((GLFWwindow*)_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}

		void
		GLFWInputMouse::onHideMouse() noexcept
		{
			if (_focusWindow)
			{
			   ::glfwSetInputMode((GLFWwindow*)_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			}
		}

		void
		GLFWInputMouse::onChangePosition(int x, int y) noexcept
		{
			if (_focusWindow)
			{
			   ::glfwSetCursorPos((GLFWwindow*)_window, x, y);
			}
		}

		void
		GLFWInputMouse::onInputEvent(const InputEvent& event) noexcept
		{
			DefaultInputMouse::onInputEvent(event);

			switch (event.event)
			{
			case InputEvent::GetFocus:
			{
				_window = event.window.windowID;
				_focusWindow = true;
			}
			break;
			case InputEvent::LostFocus:
			{
				_window = event.window.windowID;
				_focusWindow = false;
			}
			break;
			default:
				break;
			}
		}

		InputMousePtr
		GLFWInputMouse::clone() const noexcept
		{
			return std::make_shared<GLFWInputMouse>(_window);
		}
	}
}