#ifndef OCTOON_INPUT_KEYBOARD_H_
#define OCTOON_INPUT_KEYBOARD_H_

#include <octoon/input/input_keyboard_base.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT DefaultInputKeyboard : public InputKeyboard
		{
			OctoonDeclareSubInterface(DefaultInputKeyboard, InputKeyboard)
		public:
			DefaultInputKeyboard() noexcept;
			virtual ~DefaultInputKeyboard() noexcept;

			virtual bool isKeyDown(InputKey::Code key) const noexcept;
			virtual bool isKeyUp(InputKey::Code key) const noexcept;
			virtual bool isKeyPressed(InputKey::Code key) const noexcept;

			virtual InputKeyboardPtr clone() const noexcept;

		private:
			virtual void onFrameEnd() noexcept;
			virtual void onObtainCapture() noexcept;
			virtual void onReset() noexcept;

			virtual void onInputEvent(const InputEvent& event) noexcept;

		private:
			DefaultInputKeyboard(const DefaultInputKeyboard&) noexcept = delete;
			DefaultInputKeyboard& operator=(const DefaultInputKeyboard&) noexcept = delete;

		private:
			struct KeyState
			{
				bool pressed;
				bool down;
				bool up;
			};

			KeyState keyState_[InputKey::NumKeyCodes];
		};
	}
}

#endif