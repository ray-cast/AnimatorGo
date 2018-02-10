#ifndef OCTOON_INPUT_MSW_DEVICE_H_
#define OCTOON_INPUT_MSW_DEVICE_H_

#include <octoon/input/input_device.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT MSWInputDevice final : public DefaultInputDevice
		{
			OctoonDeclareSubClass(MSWInputDevice, IInputDevice)
		public:
			MSWInputDevice() noexcept;
			~MSWInputDevice() noexcept;

			virtual void setCaptureObject(WindHandle window) noexcept;
			virtual WindHandle getCaptureObject() const noexcept;

			virtual bool peekEvents(InputEvent& event) noexcept;
			virtual bool pollEvents(InputEvent& event) noexcept;
			virtual bool waitEvents(InputEvent& event) noexcept;
			virtual bool waitEvents(InputEvent& event, int timeout) noexcept;
			virtual void flushEvent() noexcept;

			virtual InputDevicePtr clone() const noexcept;

		private:
			virtual void update() noexcept;

		private:
			MSWInputDevice(const MSWInputDevice&) noexcept = delete;
			MSWInputDevice& operator=(const MSWInputDevice&) noexcept = delete;

		private:

			HWND window_;

			bool isButtonPress_;

			int button_;

			int mouseX_;
			int mouseY_;
		};
	}
}

#endif