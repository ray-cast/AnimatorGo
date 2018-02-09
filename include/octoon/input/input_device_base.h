#ifndef OCTOON_INPUT_DEVICE_BASE_H_
#define OCTOON_INPUT_DEVICE_BASE_H_

#include <octoon/input/input_controller.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT InputDevice : public runtime::RttiInterface
		{
			OctoonDeclareSubInterface(DefaultInputDevice, runtime::RttiInterface)
		public:
			InputDevice() noexcept;
			virtual ~InputDevice() noexcept;

			virtual void setCaptureObject(WindHandle window) noexcept = 0;
			virtual WindHandle getCaptureObject() const noexcept = 0;

			virtual void addInputListener(InputListenerPtr& listener) noexcept = 0;
			virtual void addInputListener(InputListenerPtr&& listener) noexcept = 0;
			virtual void removeInputListener(InputListenerPtr& listener) noexcept = 0;
			virtual void removeInputListener(InputListenerPtr&& listener) noexcept = 0;
			virtual void clearInputListener() noexcept = 0;

			virtual void enableEventPosting(bool enable) noexcept = 0;
			virtual bool enableEventPosting() const noexcept = 0;

			virtual bool sendEvent(const InputEvent& event) noexcept = 0;
			virtual bool postEvent(const InputEvent& event) noexcept = 0;

			virtual bool peekEvents(InputEvent& event) noexcept = 0;
			virtual bool pollEvents(InputEvent& event) noexcept = 0;
			virtual bool waitEvents(InputEvent& event) noexcept = 0;
			virtual bool waitEvents(InputEvent& event, int timeout) noexcept = 0;
			virtual void flushEvent() noexcept = 0;

			virtual InputDevicePtr clone() const noexcept = 0;

		private:
			InputDevice(const InputDevice&) noexcept = delete;
			InputDevice& operator=(const InputDevice&) noexcept = delete;
		};
	}
}

#endif