#ifndef OCTOON_IINPUT_KEYBOARD_H_
#define OCTOON_IINPUT_KEYBOARD_H_

#include <octoon/input/input_controller.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT IInputKeyboard : public InputController
		{
			OctoonDeclareSubInterface(IInputKeyboard, InputController)
		public:
			IInputKeyboard() noexcept = default;
			virtual ~IInputKeyboard() = default;

			virtual bool is_key_down(InputKey::Code key) const noexcept = 0;
			virtual bool is_key_up(InputKey::Code key) const noexcept = 0;
			virtual bool is_key_pressed(InputKey::Code key) const noexcept = 0;

			virtual InputKeyboardPtr clone() const noexcept = 0;

		private:
			IInputKeyboard(const IInputKeyboard&) noexcept = delete;
			IInputKeyboard& operator=(const IInputKeyboard&) noexcept = delete;
		};
	}
}

#endif