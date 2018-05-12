#ifndef OCTOON_INPUT_KEYBOARD_H_
#define OCTOON_INPUT_KEYBOARD_H_

#include <octoon/input/iinput_keyboard.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT DefaultInputKeyboard : public IInputKeyboard
		{
			OctoonDeclareSubInterface(DefaultInputKeyboard, IInputKeyboard)
		public:
			DefaultInputKeyboard() noexcept;
			virtual ~DefaultInputKeyboard() noexcept;

			virtual bool isKeyDown(InputKey::Code key) const noexcept override;
			virtual bool isKeyUp(InputKey::Code key) const noexcept override;
			virtual bool isKeyPressed(InputKey::Code key) const noexcept override;

			virtual IInputKeyboardPtr clone() const noexcept;

		protected:
			virtual void onFrameEnd() noexcept override;
			virtual void onObtainCapture() noexcept override;
			virtual void onReset() noexcept override;

			virtual void onInputEvent(const InputEvent& event) noexcept override;

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