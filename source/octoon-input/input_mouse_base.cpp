#include <octoon/input/input_mouse_base.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(InputMouse, runtime::RttiInterface, "InputMouse")

		InputMouse::InputMouse() noexcept
		{
		}

		InputMouse::~InputMouse() noexcept
		{
		}
	}
}