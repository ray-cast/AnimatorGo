#ifndef OCTOON_INPUT_MSW_KEYBOARD_H_
#define OCTOON_INPUT_MSW_KEYBOARD_H_

#include <octoon/input/input_key.h>
#include <octoon/input/input_keyboard.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT MSWInputKeyboard final : public DefaultInputKeyboard
		{
			OctoonDeclareSubClass(MSWInputKeyboard, IInputKeyboard)
		public:
			MSWInputKeyboard() noexcept;
			virtual ~MSWInputKeyboard() noexcept;

			virtual bool isKeyDown(InputKey::Code key) const noexcept;
			virtual bool isKeyUp(InputKey::Code key) const noexcept;
			virtual bool isKeyPressed(InputKey::Code key) const noexcept;

			virtual InputKeyboardPtr clone() const noexcept;

		private:
			MSWInputKeyboard(const MSWInputKeyboard&) noexcept = delete;
			MSWInputKeyboard& operator=(const MSWInputKeyboard&) noexcept = delete;
		};
	}
}

#endif