#ifndef OCTOON_INPUT_DEVICE_BASE_H_
#define OCTOON_INPUT_DEVICE_BASE_H_

#include <octoon/input/input_controller.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT IInputDevice : public runtime::RttiInterface
		{
			OctoonDeclareSubInterface(DefaultInputDevice, runtime::RttiInterface)
		public:
			IInputDevice() noexcept = default;
			virtual ~IInputDevice() = default;

			virtual void set_capture_object(WindHandle window) noexcept = 0;
			virtual WindHandle get_capture_object() const noexcept = 0;

			virtual void add_input_listener(InputListenerPtr& listener) noexcept = 0;
			virtual void add_input_listener(InputListenerPtr&& listener) noexcept = 0;
			virtual void remove_input_listener(InputListenerPtr& listener) noexcept = 0;
			virtual void remove_input_listener(InputListenerPtr&& listener) noexcept = 0;
			virtual void clear_input_listener() noexcept = 0;

			virtual void enable_event_posting(bool enable) noexcept = 0;
			virtual bool enable_event_posting() const noexcept = 0;

			virtual bool send_event(const InputEvent& event) noexcept = 0;
			virtual bool post_event(const InputEvent& event) noexcept = 0;

			virtual bool peek_events(InputEvent& event) noexcept = 0;
			virtual bool poll_events(InputEvent& event) noexcept = 0;
			virtual bool wait_events(InputEvent& event) noexcept = 0;
			virtual bool wait_events(InputEvent& event, int timeout) noexcept = 0;
			virtual void flush_event() noexcept = 0;

			virtual InputDevicePtr clone() const noexcept = 0;

		private:
			IInputDevice(const IInputDevice&) noexcept = delete;
			IInputDevice& operator=(const IInputDevice&) noexcept = delete;
		};
	}
}

#endif