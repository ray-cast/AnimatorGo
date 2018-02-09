#ifndef OCTOON_INPUT_MOUSE_BASE_H_
#define OCTOON_INPUT_MOUSE_BASE_H_

#include <octoon/input/input_controller.h>
#include <octoon/input/input_button.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT InputMouse : public InputController
		{
			OctoonDeclareSubInterface(InputMouse, InputController)
		public:
			InputMouse() noexcept;
			virtual ~InputMouse() noexcept;

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
			InputMouse(const InputMouse&) noexcept = delete;
			InputMouse& operator=(const InputMouse&) noexcept = delete;
		};
	}
}

#endif