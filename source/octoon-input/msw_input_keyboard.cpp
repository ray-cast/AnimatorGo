#include "msw_input_keyboard.h"

namespace octoon
{
	namespace input
	{
		OctoonImplementSubClass(MSWInputKeyboard, DefaultInputKeyboard, "MSWInputKeyboard")

		MSWInputKeyboard::MSWInputKeyboard() noexcept
		{
		}

		MSWInputKeyboard::~MSWInputKeyboard() noexcept
		{
		}

		bool
		MSWInputKeyboard::is_key_down(InputKey::Code key) const noexcept
		{
			return DefaultInputKeyboard::is_key_down(key);
		}

		bool
		MSWInputKeyboard::is_key_up(InputKey::Code key) const noexcept
		{
			return DefaultInputKeyboard::is_key_up(key);
		}

		bool
		MSWInputKeyboard::is_key_pressed(InputKey::Code key) const noexcept
		{
			return DefaultInputKeyboard::is_key_pressed(key);
		}

		IInputKeyboardPtr
		MSWInputKeyboard::clone() const noexcept
		{
			return std::make_shared<MSWInputKeyboard>();
		}
	}
}