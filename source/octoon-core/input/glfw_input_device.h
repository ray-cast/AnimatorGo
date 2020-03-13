#ifndef OCTOON_INPUT_GLFW_DEVICE_H_
#define OCTOON_INPUT_GLFW_DEVICE_H_

#include <octoon/input/input_device.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT GLFWInputDevice final : public DefaultInputDevice
		{
			OctoonDeclareSubClass(MSWInputDevice, IInputDevice)
		public:
			GLFWInputDevice() noexcept;
			~GLFWInputDevice() noexcept;

			virtual void set_capture_object(WindHandle window) noexcept override;
			virtual WindHandle get_capture_object() const noexcept override;

			virtual bool peek_events(InputEvent& event) noexcept override;
			virtual bool poll_events(InputEvent& event) noexcept override;
			virtual bool wait_events(InputEvent& event) noexcept override;
			virtual bool wait_events(InputEvent& event, int timeout) noexcept override;
			virtual void flush_event() noexcept;

			virtual IInputDevicePtr clone() const noexcept override;

		private:
			void update() noexcept;

		private:
			GLFWInputDevice(const GLFWInputDevice&) noexcept = delete;
			GLFWInputDevice& operator=(const GLFWInputDevice&) noexcept = delete;

		private:
			WindHandle window_;

			bool isButtonPress_;

			int button_;

			int mouseX_;
			int mouseY_;
		};
	}
}

#endif