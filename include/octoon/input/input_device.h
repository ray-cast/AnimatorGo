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

			virtual void setCaptureObject(WindHandle window) noexcept override;
			virtual WindHandle getCaptureObject() const noexcept override;

			virtual void enableEventPosting(bool enable) noexcept override;
			virtual bool enableEventPosting() const noexcept override;

			virtual void addInputListener(const IInputListenerPtr& listener) noexcept override;
			virtual void addInputListener(IInputListenerPtr&& listener) noexcept override;
			virtual void removeInputListener(const IInputListenerPtr& listener) noexcept override;
			virtual void removeInputListener(IInputListenerPtr&& listener) noexcept override;
			virtual void clearInputListener() noexcept override;

			virtual bool sendEvent(const InputEvent& event) noexcept override;
			virtual bool postEvent(const InputEvent& event) noexcept override;

			virtual bool peekEvents(InputEvent& event) noexcept override;
			virtual bool pollEvents(InputEvent& event) noexcept override;
			virtual bool waitEvents(InputEvent& event) noexcept override;
			virtual bool waitEvents(InputEvent& event, int timeout) noexcept override;
			virtual void flushEvent() noexcept override;

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