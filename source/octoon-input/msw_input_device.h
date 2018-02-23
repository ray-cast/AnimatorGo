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

			virtual void set_capture_object(WindHandle window) noexcept override;
			virtual WindHandle get_capture_object() const noexcept override;

			virtual bool peek_events(InputEvent& event) noexcept override;
			virtual bool poll_events(InputEvent& event) noexcept override;
			virtual bool wait_events(InputEvent& event) noexcept override;
			virtual bool wait_events(InputEvent& event, int timeout) noexcept override;
			virtual void flush_event() noexcept override;

			virtual InputDevicePtr clone() const noexcept override;

		private:
			void update() noexcept;

		private:
			MSWInputDevice(const MSWInputDevice&) noexcept = delete;
			MSWInputDevice& operator=(const MSWInputDevice&) noexcept = delete;

		private:

			HWND window_;

			bool isButtonPress_;

			int button_;

			int mouse_x_;
			int mouse_y_;
		};
	}
}

#endif