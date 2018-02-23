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

			virtual void lock_mouse() noexcept = 0;
			virtual void unlock_mouse() noexcept = 0;
			virtual bool is_locked_mouse() const noexcept = 0;

			virtual void show_mouse() noexcept = 0;
			virtual void hide_mouse() noexcept = 0;
			virtual bool is_show_mouse() noexcept = 0;

			virtual float get_axis(InputAxis::Code axis) const noexcept = 0;

			virtual void set_position(InputButton::Type x, InputButton::Type y) noexcept = 0;
			virtual void get_position(InputButton::Type& x, InputButton::Type& y) const noexcept = 0;

			virtual bool is_button_down(InputButton::Code key) const noexcept = 0;
			virtual bool is_button_up(InputButton::Code key) const noexcept = 0;
			virtual bool is_button_pressed(InputButton::Code key) const noexcept = 0;

			virtual IInputMousePtr clone() const noexcept = 0;

		private:
			IInputMouse(const IInputMouse&) noexcept = delete;
			IInputMouse& operator=(const IInputMouse&) noexcept = delete;
		};
	}
}

#endif