#include <octoon/input/input.h>
#include <octoon/input/input_device.h>

#if defined(OCTOON_FEATURE_INPUT_API_WINDOWS)
#	include "msw_input_device.h"
#	include "msw_input_keyboard.h"
#	include "msw_input_mouse.h"
#elif defined(OCTOON_FEATURE_INPUT_API_GLFW)
#	include "glfw_input_device.h"
#	include "glfw_input_keyboard.h"
#	include "glfw_input_mouse.h"
#else
#	include <octoon/input/input_device.h>
#	include <octoon/input/input_keyboard.h>
#	include <octoon/input/input_mouse.h>
#endif

namespace octoon
{
	namespace input
	{
		OctoonImplementSubClass(DefaultInput, IInput, "DefaultInput")

		DefaultInput::DefaultInput() noexcept
		{
		}

		DefaultInput::~DefaultInput() noexcept
		{
			this->close();
		}

		bool
		DefaultInput::open() noexcept
		{
#if defined(OCTOON_FEATURE_INPUT_API_WINDOWS)
			input_device_ = std::make_shared<MSWInputDevice>();

			this->obtain_mouse_capture(std::make_shared<MSWInputMouse>());
			this->obtain_keyboard_capture(std::make_shared<MSWInputKeyboard>());
#elif defined(OCTOON_FEATURE_INPUT_API_GLFW)
			input_device_ = std::make_shared<GLFWInputDevice>();

			this->obtain_mouse_capture(std::make_shared<GLFWInputMouse>());
			this->obtain_keyboard_capture(std::make_shared<GLFWInputKeyboard>());
#else
			input_device_ = std::make_shared<DefaultInputDevice>();

			this->obtain_mouse_capture(std::make_shared<DefaultInputMouse>());
			this->obtain_keyboard_capture(std::make_shared<DefaultInputKeyboard>());
#endif
			return true;
		}

		bool
		DefaultInput::open(InputDevicePtr& device) noexcept
		{
#if defined(OCTOON_FEATURE_INPUT_API_WINDOWS)
			input_device_ = device ? device : std::make_shared<MSWInputDevice>();

			this->obtain_mouse_capture(std::make_shared<MSWInputMouse>());
			this->obtain_keyboard_capture(std::make_shared<MSWInputKeyboard>());
#elif defined(OCTOON_FEATURE_INPUT_API_GLFW)
			input_device_ = device ? device : std::make_shared<GLFWInputDevice>();

			this->obtain_mouse_capture(std::make_shared<GLFWInputMouse>());
			this->obtain_keyboard_capture(std::make_shared<GLFWInputKeyboard>());
#else
			input_device_ = device ? device : std::make_shared<DefaultInputDevice>();

			this->obtain_mouse_capture(std::make_shared<DefaultInputMouse>());
			this->obtain_keyboard_capture(std::make_shared<DefaultInputKeyboard>());
#endif
			return true;
		}

		bool
		DefaultInput::open(InputDevicePtr&& device) noexcept
		{
#if defined(OCTOON_FEATURE_INPUT_API_WINDOWS)
			input_device_ = device ? std::move(device) : std::make_shared<MSWInputDevice>();

			this->obtain_mouse_capture(std::make_shared<MSWInputMouse>());
			this->obtain_keyboard_capture(std::make_shared<MSWInputKeyboard>());
#elif defined(OCTOON_FEATURE_INPUT_API_GLFW)
			input_device_ = device ? std::move(device) : std::make_shared<GLFWInputDevice>();

			this->obtain_mouse_capture(std::make_shared<GLFWInputMouse>());
			this->obtain_keyboard_capture(std::make_shared<GLFWInputKeyboard>());
#else
			input_device_ = device ? std::move(device) : std::make_shared<DefaultInputDevice>();

			this->obtain_mouse_capture(std::make_shared<DefaultInputMouse>());
			this->obtain_keyboard_capture(std::make_shared<DefaultInputKeyboard>());
#endif
			return true;
		}

		void
		DefaultInput::close() noexcept
		{
			if (input_device_)
			{
				input_device_.reset();
				input_device_ = nullptr;
			}

			if (mouse_capture_device_)
			{
				mouse_capture_device_->release_capture();
				mouse_capture_device_.reset();
				mouse_capture_device_ = nullptr;
			}

			if (keyboard_capture_device_)
			{
				keyboard_capture_device_->release_capture();
				keyboard_capture_device_.reset();
				keyboard_capture_device_ = nullptr;
			}
		}

		void
		DefaultInput::set_capture_object(WindHandle window) noexcept
		{
			if (input_device_)
				input_device_->set_capture_object(window);
		}

		WindHandle
		DefaultInput::get_capture_object() const noexcept
		{
			if (input_device_)
				return input_device_->get_capture_object();
			return nullptr;
		}

		float
		DefaultInput::get_axis(InputAxis::Code axis) const noexcept
		{
			if (mouse_capture_device_)
				return mouse_capture_device_->get_axis(axis);
			return 0.0f;
		}

		void
		DefaultInput::set_mouse_pos(InputButton::mouse_t x, InputButton::mouse_t y) noexcept
		{
			if (mouse_capture_device_)
				mouse_capture_device_->set_position(x, y);
		}

		void
		DefaultInput::get_mouse_pos(InputButton::mouse_t& x, InputButton::mouse_t& y) const noexcept
		{
			if (mouse_capture_device_)
				mouse_capture_device_->get_position(x, y);
		}

		bool
		DefaultInput::is_key_down(InputKey::Code key) const noexcept
		{
			if (keyboard_capture_device_)
				return keyboard_capture_device_->is_key_down(key);

			return false;
		}

		bool
		DefaultInput::is_key_up(InputKey::Code key) const noexcept
		{
			if (keyboard_capture_device_)
				return keyboard_capture_device_->is_key_up(key);

			return false;
		}

		bool
		DefaultInput::is_key_pressed(InputKey::Code key) const noexcept
		{
			if (keyboard_capture_device_)
				return keyboard_capture_device_->is_key_pressed(key);

			return false;
		}

		bool
		DefaultInput::is_button_down(InputButton::Code key) const noexcept
		{
			if (mouse_capture_device_)
				return mouse_capture_device_->is_button_down(key);

			return false;
		}

		bool
		DefaultInput::is_button_up(InputButton::Code key) const noexcept
		{
			if (mouse_capture_device_)
				return mouse_capture_device_->is_button_up(key);

			return false;
		}

		bool
		DefaultInput::is_button_pressed(InputButton::Code key) const noexcept
		{
			if (mouse_capture_device_)
				return mouse_capture_device_->is_button_pressed(key);

			return false;
		}

		void
		DefaultInput::show_cursor(bool show) noexcept
		{
			if (mouse_capture_device_)
			{
				if (show)
					mouse_capture_device_->show_mouse();
				else
					mouse_capture_device_->hide_mouse();
			}
		}

		bool
		DefaultInput::is_show_cursor() const noexcept
		{
			if (mouse_capture_device_)
				return mouse_capture_device_->is_show_mouse();
			return false;
		}

		void
		DefaultInput::lock_cursor(bool lock) noexcept
		{
			if (mouse_capture_device_)
			{
				if (lock)
					mouse_capture_device_->lock_mouse();
				else
					mouse_capture_device_->unlock_mouse();
			}
		}

		bool
		DefaultInput::is_locked_cursor() const noexcept
		{
			if (mouse_capture_device_)
				return mouse_capture_device_->is_locked_mouse();
			return false;
		}

		void
		DefaultInput::obtain_mouse_capture() noexcept
		{
			if (mouse_capture_device_ && !mouse_capture_device_->capture())
				mouse_capture_device_->obtain_capture();
		}

		void
		DefaultInput::obtain_keyboard_capture() noexcept
		{
			if (keyboard_capture_device_ && !keyboard_capture_device_->capture())
				keyboard_capture_device_->obtain_capture();
		}

		void
		DefaultInput::obtain_mouse_capture(InputMousePtr& mouse) noexcept
		{
			if (mouse_capture_device_ != mouse)
			{
				if (mouse_capture_device_)
				{
					mouse_capture_device_->release_capture();

					if (input_device_)
						input_device_->remove_input_listener(mouse_capture_device_);
				}

				mouse_capture_device_ = mouse;

				if (mouse_capture_device_)
				{
					mouse_capture_device_->obtain_capture();

					if (input_device_)
						input_device_->add_input_listener(mouse_capture_device_);
				}
			}
		}

		void
		DefaultInput::obtain_mouse_capture(InputMousePtr&& mouse) noexcept
		{
			if (mouse_capture_device_ != mouse)
			{
				if (mouse_capture_device_)
				{
					mouse_capture_device_->release_capture();

					if (input_device_)
						input_device_->remove_input_listener(mouse_capture_device_);
				}

				mouse_capture_device_ = std::move(mouse);

				if (mouse_capture_device_)
				{
					mouse_capture_device_->obtain_capture();

					if (input_device_)
						input_device_->add_input_listener(mouse_capture_device_);
				}
			}
		}

		void
		DefaultInput::obtain_keyboard_capture(InputKeyboardPtr& keyboard) noexcept
		{
			if (keyboard_capture_device_ != keyboard)
			{
				if (keyboard_capture_device_)
				{
					keyboard_capture_device_->release_capture();

					if (input_device_)
						input_device_->remove_input_listener(keyboard);
				}

				keyboard_capture_device_ = keyboard;

				if (keyboard_capture_device_)
				{
					keyboard_capture_device_->obtain_capture();

					if (input_device_)
						input_device_->add_input_listener(keyboard_capture_device_);
				}
			}
		}

		void
		DefaultInput::obtain_keyboard_capture(InputKeyboardPtr&& keyboard) noexcept
		{
			if (keyboard_capture_device_ != keyboard)
			{
				if (keyboard_capture_device_)
				{
					keyboard_capture_device_->release_capture();

					if (input_device_)
						input_device_->remove_input_listener(keyboard);
				}

				keyboard_capture_device_ = std::move(keyboard);

				if (keyboard_capture_device_)
				{
					keyboard_capture_device_->obtain_capture();

					if (input_device_)
						input_device_->add_input_listener(keyboard_capture_device_);
				}
			}
		}

		void
		DefaultInput::obtain_capture() noexcept
		{
			this->obtain_mouse_capture();
			this->obtain_keyboard_capture();
		}

		void
		DefaultInput::release_mouse_capture() noexcept
		{
			if (mouse_capture_device_ && mouse_capture_device_->capture())
				mouse_capture_device_->release_capture();
		}

		void
		DefaultInput::release_keyboard_capture() noexcept
		{
			if (keyboard_capture_device_ && keyboard_capture_device_->capture())
				keyboard_capture_device_->release_capture();
		}

		void
		DefaultInput::release_capture() noexcept
		{
			this->release_mouse_capture();
			this->release_keyboard_capture();
		}

		void
		DefaultInput::reset() noexcept
		{
			if (mouse_capture_device_)
				mouse_capture_device_->on_reset();

			if (keyboard_capture_device_)
				keyboard_capture_device_->on_reset();
		}

		void
		DefaultInput::add_input_listener(InputListenerPtr& listener) noexcept
		{
			if (input_device_)
				input_device_->add_input_listener(listener);
		}

		void
		DefaultInput::add_input_listener(InputListenerPtr&& listener) noexcept
		{
			if (input_device_)
				input_device_->add_input_listener(std::move(listener));
		}

		void
		DefaultInput::remove_input_listener(InputListenerPtr& listener) noexcept
		{
			if (input_device_)
				input_device_->remove_input_listener(listener);
		}

		void
		DefaultInput::remove_input_listener(InputListenerPtr&& listener) noexcept
		{
			if (input_device_)
				input_device_->remove_input_listener(std::move(listener));
		}

		void
		DefaultInput::clear_input_listener() noexcept
		{
			if (input_device_)
				input_device_->clear_input_listener();
		}

		bool
		DefaultInput::send_input_event(const InputEvent& event) noexcept
		{
			if (input_device_)
				return input_device_->send_event(event);
			return false;
		}

		bool
		DefaultInput::post_input_event(const InputEvent& event) noexcept
		{
			if (input_device_)
				return input_device_->post_event(event);
			return true;
		}

		void
		DefaultInput::update_begin() noexcept
		{
			if (mouse_capture_device_)
				mouse_capture_device_->on_frame_begin();

			if (keyboard_capture_device_)
				keyboard_capture_device_->on_frame_begin();
		}

		void
		DefaultInput::update() noexcept
		{
			if (!input_device_)
				return;

			InputEvent event;
			while (input_device_->poll_events(event))
			{
				this->send_input_event(event);
			}
		}

		void
		DefaultInput::update_end() noexcept
		{
			if (mouse_capture_device_ && mouse_capture_device_->capture())
				mouse_capture_device_->on_frame_end();

			if (keyboard_capture_device_ && keyboard_capture_device_->capture())
				keyboard_capture_device_->on_frame_end();
		}

		InputPtr
		DefaultInput::clone() const noexcept
		{
			auto input = std::make_shared<DefaultInput>();
			if (input_device_)
				input->open(input_device_->clone());

			if (keyboard_capture_device_)
				input->obtain_keyboard_capture(keyboard_capture_device_->clone());

			if (mouse_capture_device_)
				input->obtain_mouse_capture(mouse_capture_device_->clone());

			return input;
		}
	}
}