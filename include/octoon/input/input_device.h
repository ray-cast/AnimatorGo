#ifndef OCTOON_INPUT_DEVICE_H_
#define OCTOON_INPUT_DEVICE_H_

#include <octoon/input/iinput_device.h>

#include <queue>
#include <thread>
#include <condition_variable>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT DefaultInputDevice : public IInputDevice
		{
			OctoonDeclareSubInterface(DefaultInputDevice, IInputDevice)
		public:
			DefaultInputDevice() noexcept;
			virtual ~DefaultInputDevice() noexcept;

			virtual void set_capture_object(WindHandle window) noexcept;
			virtual WindHandle get_capture_object() const noexcept;

			virtual void enable_event_posting(bool enable) noexcept;
			virtual bool enable_event_posting() const noexcept;

			virtual void add_input_listener(InputListenerPtr& listener) noexcept;
			virtual void add_input_listener(InputListenerPtr&& listener) noexcept;
			virtual void remove_input_listener(InputListenerPtr& listener) noexcept;
			virtual void remove_input_listener(InputListenerPtr&& listener) noexcept;
			virtual void clear_input_listener() noexcept;

			virtual bool send_event(const InputEvent& event) noexcept;
			virtual bool post_event(const InputEvent& event) noexcept;

			virtual bool peek_events(InputEvent& event) noexcept;
			virtual bool poll_events(InputEvent& event) noexcept;
			virtual bool wait_events(InputEvent& event) noexcept;
			virtual bool wait_events(InputEvent& event, int timeout) noexcept;
			virtual void flush_event() noexcept;

			virtual InputDevicePtr clone() const noexcept;

		private:
			DefaultInputDevice(const DefaultInputDevice&) noexcept = delete;
			DefaultInputDevice& operator=(const DefaultInputDevice&) noexcept = delete;

		private:
			typedef std::queue<InputEvent>  InputEvents;
			typedef std::vector<InputListenerPtr> InputListeners;

			InputEvents events_;
			InputListeners inputListeners_;

			bool enableEventPosting_;

			std::mutex mutex_;
			std::condition_variable dispose_;
		};
	}
}

#endif