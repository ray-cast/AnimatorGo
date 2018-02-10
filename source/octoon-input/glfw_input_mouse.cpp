#include "glfw_input_mouse.h"
#include <GLFW\glfw3.h>

namespace octoon
{
	namespace input
	{
		GLFWInputMouse::GLFWInputMouse() noexcept
			: focusWindow_(false)
			, window_(nullptr)
		{
		}

		GLFWInputMouse::~GLFWInputMouse() noexcept
		{
		}

		void
		GLFWInputMouse::onShowMouse() noexcept
		{
			if (focusWindow_ && window_)
			   ::glfwSetInputMode((GLFWwindow*)window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		void
		GLFWInputMouse::onHideMouse() noexcept
		{
			if (focusWindow_ && window_)
			   ::glfwSetInputMode((GLFWwindow*)window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}

		void
		GLFWInputMouse::onChangePosition(int x, int y) noexcept
		{
			if (focusWindow_ && window_)
			   ::glfwSetCursorPos((GLFWwindow*)window_, x, y);
		}

		void
		GLFWInputMouse::onInputEvent(const InputEvent& event) noexcept
		{
			DefaultInputMouse::onInputEvent(event);

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

		InputMousePtr
		GLFWInputMouse::clone() const noexcept
		{
			return std::make_shared<GLFWInputMouse>();
		}
	}
}