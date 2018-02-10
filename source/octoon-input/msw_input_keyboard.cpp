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
		MSWInputKeyboard::isKeyDown(InputKey::Code key) const noexcept
		{
			return DefaultInputKeyboard::isKeyDown(key);
		}

		bool
		MSWInputKeyboard::isKeyUp(InputKey::Code key) const noexcept
		{
			return DefaultInputKeyboard::isKeyUp(key);
		}

		bool
		MSWInputKeyboard::isKeyPressed(InputKey::Code key) const noexcept
		{
			return DefaultInputKeyboard::isKeyPressed(key);
		}

		InputKeyboardPtr
		MSWInputKeyboard::clone() const noexcept
		{
			return std::make_shared<MSWInputKeyboard>();
		}
	}
}