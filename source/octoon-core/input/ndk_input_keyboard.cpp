#include "NDK_input_keyboard.h"

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(NDKInputKeyboard, InputKeyboard, "NDKInputKeyboard")

		NDKInputKeyboard::NDKInputKeyboard() noexcept
		{
		}

		NDKInputKeyboard::~NDKInputKeyboard() noexcept
		{
		}

		bool
		NDKInputKeyboard::getKeyState(InputKey::Code key) const noexcept
		{
			return false;
		}
	}
}

#endif