#ifndef OCTOON_INPUT_NDK_KEYBOARD_H_
#define OCTOON_INPUT_NDK_KEYBOARD_H_

#include <octoon/input/input_key.h>
#include <octoon/input/input_keyboard_base.h>

namespace octoon
{
	namespace input
	{
		class DLL_EXPORT NDKInputKeyboard : public InputKeyboard
		{
			__DeclareSubInterface(NDKInputKeyboard, InputKeyboard)
		public:
			NDKInputKeyboard() noexcept;
			virtual ~NDKInputKeyboard() noexcept;

			virtual bool getKeyState(InputKey::Code key) const noexcept;

		private:
			NDKInputKeyboard(const NDKInputKeyboard&) noexcept = delete;
			NDKInputKeyboard& operator=(const NDKInputKeyboard&) noexcept = delete;
		};
	}
}

#endif