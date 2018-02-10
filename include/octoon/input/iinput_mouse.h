#ifndef OCTOON_IINPUT_MOUSE_H_
#define OCTOON_IINPUT_MOUSE_H_

#include <octoon/input/input_controller.h>
#include <octoon/input/input_button.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT IInputMouse : public InputController
		{
			OctoonDeclareSubInterface(IInputMouse, InputController)
		public:
			IInputMouse() noexcept = default;
			virtual ~IInputMouse() = default;

			virtual void lockMouse() noexcept = 0;
			virtual void unlockMouse() noexcept = 0;
			virtual bool isLockedMouse() const noexcept = 0;

			virtual void showMouse() noexcept = 0;
			virtual void hideMouse() noexcept = 0;
			virtual bool isShowMouse() noexcept = 0;

			virtual float getAxis(InputAxis::Code axis) const noexcept = 0;

			virtual void setPosition(InputButton::mouse_t x, InputButton::mouse_t y) noexcept = 0;
			virtual void getPosition(InputButton::mouse_t& x, InputButton::mouse_t& y) const noexcept = 0;

			virtual bool isButtonDown(InputButton::Code key) const noexcept = 0;
			virtual bool isButtonUp(InputButton::Code key) const noexcept = 0;
			virtual bool isButtonPressed(InputButton::Code key) const noexcept = 0;

			virtual InputMousePtr clone() const noexcept = 0;

		private:
			IInputMouse(const IInputMouse&) noexcept = delete;
			IInputMouse& operator=(const IInputMouse&) noexcept = delete;
		};
	}
}

#endif