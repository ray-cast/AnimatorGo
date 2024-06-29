#ifndef OCTOON_INPUT_DEVICE_BASE_H_
#define OCTOON_INPUT_DEVICE_BASE_H_

#include <octoon/input/input_controller.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT IInputDevice : public Object
		{
			OctoonDeclareSubInterface(DefaultInputDevice, Object)
		public:
			IInputDevice() noexcept = default;
			virtual ~IInputDevice() = default;

			virtual void setCaptureObject(WindHandle window) noexcept = 0;
			virtual WindHandle getCaptureObject() const noexcept = 0;

			virtual void addInputListener(const IInputListenerPtr& listener) noexcept = 0;
			virtual void addInputListener(IInputListenerPtr&& listener) noexcept = 0;
			virtual void removeInputListener(const IInputListenerPtr& listener) noexcept = 0;
			virtual void removeInputListener(IInputListenerPtr&& listener) noexcept = 0;
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

			virtual IInputDevicePtr clone() const noexcept = 0;

		private:
			IInputDevice(const IInputDevice&) noexcept = delete;
			IInputDevice& operator=(const IInputDevice&) noexcept = delete;
		};
	}
}

#endif