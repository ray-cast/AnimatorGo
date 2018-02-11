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

			void lock_mouse() noexcept;
			void unlock_mouse() noexcept;
			bool is_locked_mouse() const noexcept;

			void show_mouse() noexcept;
			void hide_mouse() noexcept;
			bool is_show_mouse() noexcept;

			float get_axis(InputAxis::Code axis) const noexcept;

			void set_position(InputButton::mouse_t x, InputButton::mouse_t y) noexcept;
			void get_position(InputButton::mouse_t& x, InputButton::mouse_t& y) const noexcept;

			bool is_button_down(InputButton::Code key) const noexcept;
			bool is_button_up(InputButton::Code key) const noexcept;
			bool is_button_pressed(InputButton::Code key) const noexcept;

			InputMousePtr clone() const noexcept;

		protected:
			virtual void on_frame_begin() noexcept;
			virtual void on_frame_end() noexcept;

			virtual void on_obtain_capture() noexcept;
			virtual void on_release_capture() noexcept;

			virtual void on_reset() noexcept;

			virtual void on_input_event(const InputEvent& event) noexcept;

			virtual void onShowMouse() noexcept;
			virtual void onHideMouse() noexcept;

			virtual void onChangePosition(InputButton::mouse_t x, InputButton::mouse_t y) noexcept;

		private:
			DefaultInputMouse(const DefaultInputMouse&) noexcept = delete;
			DefaultInputMouse& operator=(const DefaultInputMouse&) noexcept = delete;

		protected:
			bool is_mouse_lock_;
			bool is_mouse_locked_;
			bool is_mouse_hide;

			float mouse_axis_x_;
			float mouse_axis_y_;

			InputButton::mouse_t last_x_;
			InputButton::mouse_t last_y_;

			InputButton::mouse_t mouse_x_;
			InputButton::mouse_t mouse_y_;

			InputButton::mouse_t center_x_;
			InputButton::mouse_t center_y_;

			struct ButtonState
			{
				bool down;
				bool up;
				bool pressed;
				bool doubleClick;
			};

			ButtonState button_state_[InputButton::NumButtonCodes];
		};
	}
}

#endif