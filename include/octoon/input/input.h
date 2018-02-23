#ifndef OCTOON_INPUT_H_
#define OCTOON_INPUT_H_

#include <octoon/input/iinput.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT DefaultInput : public IInput
		{
			OctoonDeclareSubClass(DefaultInput, IInput)
		public:
			DefaultInput() noexcept;
			virtual ~DefaultInput() noexcept;

			virtual bool open() noexcept override;
			virtual bool open(const IInputDevicePtr& device) noexcept override;
			virtual bool open(IInputDevicePtr&& device) noexcept override;
			virtual void close() noexcept override;

			virtual void set_capture_object(WindHandle window) noexcept override;
			virtual WindHandle get_capture_object() const noexcept override;

			virtual float get_axis(InputAxis::Code axis) const noexcept override;

			virtual void set_mouse_pos(InputButton::Type x, InputButton::Type y) noexcept override;
			virtual void get_mouse_pos(InputButton::Type& x, InputButton::Type& y) const noexcept override;

			virtual bool is_key_down(InputKey::Code key) const noexcept override;
			virtual bool is_key_up(InputKey::Code key) const noexcept override;
			virtual bool is_key_pressed(InputKey::Code key) const noexcept override;

			virtual bool is_button_down(InputButton::Code key) const noexcept override;
			virtual bool is_button_up(InputButton::Code key) const noexcept override;
			virtual bool is_button_pressed(InputButton::Code key) const noexcept override;

			virtual void show_cursor(bool show) noexcept override;
			virtual bool is_show_cursor() const noexcept override;

			virtual void lock_cursor(bool lock) noexcept override;
			virtual bool is_locked_cursor() const noexcept override;

			virtual void obtain_mouse_capture() noexcept override;
			virtual void obtain_keyboard_capture() noexcept override;

			virtual void obtain_mouse_capture(const IInputMousePtr& mouse) noexcept override;
			virtual void obtain_mouse_capture(IInputMousePtr&& mouse) noexcept override;
			virtual void obtain_keyboard_capture(const IInputKeyboardPtr& key) noexcept override;
			virtual void obtain_keyboard_capture(IInputKeyboardPtr&& key) noexcept override;
			virtual void obtain_capture() noexcept override;

			virtual void release_mouse_capture() noexcept override;
			virtual void release_keyboard_capture() noexcept override;
			virtual void release_capture() noexcept override;

			virtual void reset() noexcept override;

			virtual void add_input_listener(const IInputListenerPtr& listener) noexcept override;
			virtual void add_input_listener(IInputListenerPtr&& listener) noexcept override;
			virtual void remove_input_listener(const IInputListenerPtr& listener) noexcept override;
			virtual void remove_input_listener(IInputListenerPtr&& listener) noexcept override;
			virtual void clear_input_listener() noexcept override;

			virtual bool send_input_event(const InputEvent& event) noexcept override;
			virtual bool post_input_event(const InputEvent& event) noexcept override;

			virtual void update_begin() noexcept override;
			virtual void update() noexcept override;
			virtual void update_end() noexcept override;

			virtual IInputPtr clone() const noexcept override;

		private:
			DefaultInput(const DefaultInput&) noexcept = delete;
			DefaultInput& operator=(const DefaultInput&) noexcept = delete;

		private:
			IInputDevicePtr input_device_;
			IInputMousePtr mouse_capture_device_;
			IInputKeyboardPtr keyboard_capture_device_;
		};
	}
}

#endif