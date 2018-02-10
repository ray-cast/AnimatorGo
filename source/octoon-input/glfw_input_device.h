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