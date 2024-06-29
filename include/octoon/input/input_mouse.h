#ifndef OCTOON_INPUT_MOUSE_H_
#define OCTOON_INPUT_MOUSE_H_

#include <octoon/input/iinput_mouse.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT DefaultInputMouse : public IInputMouse
		{
			OctoonDeclareSubClass(DefaultInputMouse, IInputMouse)
		public:
			DefaultInputMouse() noexcept;
			virtual ~DefaultInputMouse() noexcept;

			void lockMouse() noexcept override;
			void unlockMouse() noexcept override;
			bool isLockedMouse() const noexcept override;

			void showMouse() noexcept override;
			void hideMouse() noexcept override;
			bool isShowMouse() noexcept override;

			float getAxis(InputAxis::Code axis) const noexcept override;

			void setPosition(InputButton::Type x, InputButton::Type y) noexcept override;
			void getPosition(InputButton::Type& x, InputButton::Type& y) const noexcept override;

			bool isButtonDown(InputButton::Code key) const noexcept override;
			bool isButtonUp(InputButton::Code key) const noexcept override;
			bool isButtonPressed(InputButton::Code key) const noexcept override;

			IInputMousePtr clone() const noexcept override;

		protected:
			virtual void onFrameBegin() noexcept override;
			virtual void onFrameEnd() noexcept override;

			virtual void onObtainCapture() noexcept override;
			virtual void onReleaseCapture() noexcept override;

			virtual void onReset() noexcept override;

			virtual void onInputEvent(const InputEvent& event) noexcept override;

		protected:
			virtual void onShowMouse() noexcept;
			virtual void onHideMouse() noexcept;

			virtual void onChangePosition(InputButton::Type x, InputButton::Type y) noexcept;

		private:
			DefaultInputMouse(const DefaultInputMouse&) noexcept = delete;
			DefaultInputMouse& operator=(const DefaultInputMouse&) noexcept = delete;

		protected:
			bool isMouseLock_;
			bool isMouseLocked_;
			bool isMouseHide;
			bool isMousePositionUpdated_;

			float mouseAxisX_;
			float mouseAxisY_;

			InputButton::Type lastX_;
			InputButton::Type lastY_;

			InputButton::Type mouseX_;
			InputButton::Type mouseY_;

			InputButton::Type centerX_;
			InputButton::Type centerY_;

			struct ButtonState
			{
				bool down;
				bool up;
				bool pressed;
				bool doubleClick;
			};

			ButtonState buttonState_[InputButton::NumButtonCodes];
		};
	}
}

#endif