#ifndef OCTOON_INPUT_MOUSE_H_
#define OCTOON_INPUT_MOUSE_H_

#include <octoon/input/input_mouse_base.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT DefaultInputMouse : public InputMouse
		{
			OctoonDeclareSubClass(DefaultInputMouse, InputMouse)
		public:
			DefaultInputMouse() noexcept;
			virtual ~DefaultInputMouse() noexcept;

			void lockMouse() noexcept;
			void unlockMouse() noexcept;
			bool isLockedMouse() const noexcept;

			void showMouse() noexcept;
			void hideMouse() noexcept;
			bool isShowMouse() noexcept;

			float getAxis(InputAxis::Code axis) const noexcept;

			void setPosition(InputButton::mouse_t x, InputButton::mouse_t y) noexcept;
			void getPosition(InputButton::mouse_t& x, InputButton::mouse_t& y) const noexcept;

			bool isButtonDown(InputButton::Code key) const noexcept;
			bool isButtonUp(InputButton::Code key) const noexcept;
			bool isButtonPressed(InputButton::Code key) const noexcept;

			InputMousePtr clone() const noexcept;

		protected:
			virtual void onFrameBegin() noexcept;
			virtual void onFrameEnd() noexcept;

			virtual void onObtainCapture() noexcept;
			virtual void onReleaseCapture() noexcept;

			virtual void onReset() noexcept;

			virtual void onInputEvent(const InputEvent& event) noexcept;

			virtual void onShowMouse() noexcept;
			virtual void onHideMouse() noexcept;

			virtual void onChangePosition(InputButton::mouse_t x, InputButton::mouse_t y) noexcept;

		private:
			DefaultInputMouse(const DefaultInputMouse&) noexcept = delete;
			DefaultInputMouse& operator=(const DefaultInputMouse&) noexcept = delete;

		protected:

			bool _isMouseLock;
			bool _isMouseLocked;
			bool _isMouseHide;

			float _mouseAxisX;
			float _mouseAxisY;

			InputButton::mouse_t _lastX;
			InputButton::mouse_t _lastY;

			InputButton::mouse_t mouseX_;
			InputButton::mouse_t mouseY_;

			InputButton::mouse_t _centerX;
			InputButton::mouse_t _centerY;

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