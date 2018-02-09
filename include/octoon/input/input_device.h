#ifndef OCTOON_INPUT_DEVICE_H_
#define OCTOON_INPUT_DEVICE_H_

#include <octoon/input/input_device_base.h>

#include <queue>
#include <thread>
#include <condition_variable>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT DefaultInputDevice : public InputDevice
		{
			OctoonDeclareSubInterface(DefaultInputDevice, InputDevice)
		public:
			DefaultInputDevice() noexcept;
			virtual ~DefaultInputDevice() noexcept;

			virtual void setCaptureObject(WindHandle window) noexcept;
			virtual WindHandle getCaptureObject() const noexcept;

			virtual void enableEventPosting(bool enable) noexcept;
			virtual bool enableEventPosting() const noexcept;

			virtual void addInputListener(InputListenerPtr& listener) noexcept;
			virtual void addInputListener(InputListenerPtr&& listener) noexcept;
			virtual void removeInputListener(InputListenerPtr& listener) noexcept;
			virtual void removeInputListener(InputListenerPtr&& listener) noexcept;
			virtual void clearInputListener() noexcept;

			virtual bool sendEvent(const InputEvent& event) noexcept;
			virtual bool postEvent(const InputEvent& event) noexcept;

			virtual bool peekEvents(InputEvent& event) noexcept;
			virtual bool pollEvents(InputEvent& event) noexcept;
			virtual bool waitEvents(InputEvent& event) noexcept;
			virtual bool waitEvents(InputEvent& event, int timeout) noexcept;
			virtual void flushEvent() noexcept;

			virtual InputDevicePtr clone() const noexcept;

		private:
			DefaultInputDevice(const DefaultInputDevice&) noexcept = delete;
			DefaultInputDevice& operator=(const DefaultInputDevice&) noexcept = delete;

		private:

			typedef std::queue<InputEvent>  InputEvents;
			typedef std::vector<InputListenerPtr> InputListeners;

			InputEvents _events;
			InputListeners _inputListeners;

			bool _enableEventPosting;

			std::mutex _mutex;
			std::condition_variable _dispose;
		};
	}
}

#endif