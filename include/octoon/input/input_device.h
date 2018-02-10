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

			InputEvents events_;
			InputListeners inputListeners_;

			bool enableEventPosting_;

			std::mutex mutex_;
			std::condition_variable dispose_;
		};
	}
}

#endif