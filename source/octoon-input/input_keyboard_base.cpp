#include <octoon/input/input_keyboard_base.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(InputKeyboard, InputController, "InputKeyboard")

		InputKeyboard::InputKeyboard() noexcept
		{
		}

		InputKeyboard::~InputKeyboard() noexcept
		{
		}
	}
}