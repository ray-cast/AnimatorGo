#ifndef OCTOON_INPUT_BASE_H_
#define OCTOON_INPUT_BASE_H_

#include <octoon/input/iinput_device.h>
#include <octoon/input/iinput_keyboard.h>
#include <octoon/input/iinput_mouse.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT IInput : public runtime::RttiInterface
		{
			OctoonDeclareSubInterface(IInput, runtime::RttiInterface)
		public:
			IInput() = default;
			virtual ~IInput() = default;

			virtual bool open() noexcept = 0;
			virtual bool open(InputDevicePtr& device) noexcept = 0;
			virtual bool open(InputDevicePtr&& device) noexcept = 0;
			virtual void close() noexcept = 0;

			virtual void set_capture_object(WindHandle window) noexcept = 0;
			virtual WindHandle get_capture_object() const noexcept = 0;

			virtual float get_axis(InputAxis::Code axis) const noexcept = 0;

			virtual void set_mouse_pos(InputButton::Type x, InputButton::Type y) noexcept = 0;
			virtual void get_mouse_pos(InputButton::Type& x, InputButton::Type& y) const noexcept = 0;

			virtual bool is_key_down(InputKey::Code key) const noexcept = 0;
			virtual bool is_key_up(InputKey::Code key) const noexcept = 0;
			virtual bool is_key_pressed(InputKey::Code key) const noexcept = 0;

			virtual bool is_button_down(InputButton::Code key) const noexcept = 0;
			virtual bool is_button_up(InputButton::Code key) const noexcept = 0;
			virtual bool is_button_pressed(InputButton::Code key) const noexcept = 0;

			virtual void show_cursor(bool show) noexcept = 0;
			virtual bool is_show_cursor() const noexcept = 0;

			virtual void lock_cursor(bool lock) noexcept = 0;
			virtual bool is_locked_cursor() const noexcept = 0;

			virtual void obtain_mouse_capture() noexcept = 0;
			virtual void obtain_keyboard_capture() noexcept = 0;

			virtual void obtain_mouse_capture(InputMousePtr& mouse) noexcept = 0;
			virtual void obtain_mouse_capture(InputMousePtr&& mouse) noexcept = 0;
			virtual void obtain_keyboard_capture(InputKeyboardPtr& key) noexcept = 0;
			virtual void obtain_keyboard_capture(InputKeyboardPtr&& key) noexcept = 0;
			virtual void obtain_capture() noexcept = 0;

			virtual void release_mouse_capture() noexcept = 0;
			virtual void release_keyboard_capture() noexcept = 0;
			virtual void release_capture() noexcept = 0;

			virtual void reset() noexcept = 0;

			virtual void add_input_listener(InputListenerPtr& listener) noexcept = 0;
			virtual void add_input_listener(InputListenerPtr&& listener) noexcept = 0;
			virtual void remove_input_listener(InputListenerPtr& listener) noexcept = 0;
			virtual void remove_input_listener(InputListenerPtr&& listener) noexcept = 0;
			virtual void clear_input_listener() noexcept = 0;

			virtual bool send_input_event(const InputEvent& event) noexcept = 0;
			virtual bool post_input_event(const InputEvent& event) noexcept = 0;

			virtual void update_begin() noexcept = 0;
			virtual void update() noexcept = 0;
			virtual void update_end() noexcept = 0;

			virtual InputPtr clone() const noexcept = 0;

		private:
			IInput(const IInput&) noexcept = delete;
			IInput& operator=(const IInput&) noexcept = delete;
		};
	}
}

#endif