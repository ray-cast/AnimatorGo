#ifndef OCTOON_INPUT_KEYBOARD_BASE_H_
#define OCTOON_INPUT_KEYBOARD_BASE_H_

#include <octoon/input/input_controller.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT InputKeyboard : public InputController
		{
			OctoonDeclareSubInterface(InputKeyboard, InputController)
		public:
			InputKeyboard() noexcept;
			virtual ~InputKeyboard() noexcept;

			virtual bool isKeyDown(InputKey::Code key) const noexcept = 0;
			virtual bool isKeyUp(InputKey::Code key) const noexcept = 0;
			virtual bool isKeyPressed(InputKey::Code key) const noexcept = 0;

			virtual InputKeyboardPtr clone() const noexcept = 0;

		private:
			InputKeyboard(const InputKeyboard&) noexcept = delete;
			InputKeyboard& operator=(const InputKeyboard&) noexcept = delete;
		};
	}
}

#endif