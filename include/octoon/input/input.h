#ifndef OCTOON_INPUT_H_
#define OCTOON_INPUT_H_

#include <octoon/input/iinput.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT DefaultInput final : public IInput
		{
			OctoonDeclareSubClass(DefaultInput, IInput)
		public:
			DefaultInput() noexcept;
			~DefaultInput() noexcept;

			virtual bool open() noexcept;
			virtual bool open(InputDevicePtr& device) noexcept;
			virtual bool open(InputDevicePtr&& device) noexcept;
			virtual void close() noexcept;

			virtual void set_capture_object(WindHandle window) noexcept;
			virtual WindHandle get_capture_object() const noexcept;

			virtual float get_axis(InputAxis::Code axis) const noexcept;

			virtual void set_mouse_pos(InputButton::mouse_t x, InputButton::mouse_t y) noexcept;
			virtual void get_mouse_pos(InputButton::mouse_t& x, InputButton::mouse_t& y) const noexcept;

			virtual bool is_key_down(InputKey::Code key) const noexcept;
			virtual bool is_key_up(InputKey::Code key) const noexcept;
			virtual bool is_key_pressed(InputKey::Code key) const noexcept;

			virtual bool is_button_down(InputButton::Code key) const noexcept;
			virtual bool is_button_up(InputButton::Code key) const noexcept;
			virtual bool is_button_pressed(InputButton::Code key) const noexcept;

			virtual void show_cursor(bool show) noexcept;
			virtual bool is_show_cursor() const noexcept;

			virtual void lock_cursor(bool lock) noexcept;
			virtual bool is_locked_cursor() const noexcept;

			virtual void obtain_mouse_capture() noexcept;
			virtual void obtain_keyboard_capture() noexcept;

			virtual void obtain_mouse_capture(InputMousePtr& mouse) noexcept;
			virtual void obtain_mouse_capture(InputMousePtr&& mouse) noexcept;
			virtual void obtain_keyboard_capture(InputKeyboardPtr& key) noexcept;
			virtual void obtain_keyboard_capture(InputKeyboardPtr&& key) noexcept;
			virtual void obtain_capture() noexcept;

			virtual void release_mouse_capture() noexcept;
			virtual void release_keyboard_capture() noexcept;
			virtual void release_capture() noexcept;

			virtual void reset() noexcept;

			virtual void add_input_listener(InputListenerPtr& listener) noexcept;
			virtual void add_input_listener(InputListenerPtr&& listener) noexcept;
			virtual void remove_input_listener(InputListenerPtr& listener) noexcept;
			virtual void remove_input_listener(InputListenerPtr&& listener) noexcept;
			virtual void clear_input_listener() noexcept;

			virtual bool send_input_event(const InputEvent& event) noexcept;
			virtual bool post_input_event(const InputEvent& event) noexcept;

			virtual void update_begin() noexcept;
			virtual void update() noexcept;
			virtual void update_end() noexcept;

			virtual InputPtr clone() const noexcept;

		private:
			DefaultInput(const DefaultInput&) noexcept = delete;
			DefaultInput& operator=(const DefaultInput&) noexcept = delete;

		private:
			InputDevicePtr input_device_;
			InputMousePtr mouse_capture_device_;
			InputKeyboardPtr keyboard_capture_device_;
		};
	}
}

#endif