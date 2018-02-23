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

			virtual void set_capture_object(WindHandle window) noexcept override;
			virtual WindHandle get_capture_object() const noexcept override;

			virtual void enable_event_posting(bool enable) noexcept override;
			virtual bool enable_event_posting() const noexcept override;

			virtual void add_input_listener(const IInputListenerPtr& listener) noexcept override;
			virtual void add_input_listener(IInputListenerPtr&& listener) noexcept override;
			virtual void remove_input_listener(const IInputListenerPtr& listener) noexcept override;
			virtual void remove_input_listener(IInputListenerPtr&& listener) noexcept override;
			virtual void clear_input_listener() noexcept override;

			virtual bool send_event(const InputEvent& event) noexcept override;
			virtual bool post_event(const InputEvent& event) noexcept override;

			virtual bool peek_events(InputEvent& event) noexcept override;
			virtual bool poll_events(InputEvent& event) noexcept override;
			virtual bool wait_events(InputEvent& event) noexcept override;
			virtual bool wait_events(InputEvent& event, int timeout) noexcept override;
			virtual void flush_event() noexcept override;

			virtual IInputDevicePtr clone() const noexcept override;

		private:
			DefaultInputDevice(const DefaultInputDevice&) noexcept = delete;
			DefaultInputDevice& operator=(const DefaultInputDevice&) noexcept = delete;

		private:
			typedef std::queue<InputEvent>  InputEvents;
			typedef std::vector<IInputListenerPtr> InputListeners;

			InputEvents events_;
			InputListeners inputListeners_;

			bool enableEventPosting_;

			std::mutex mutex_;
			std::condition_variable dispose_;
		};
	}
}

#endif