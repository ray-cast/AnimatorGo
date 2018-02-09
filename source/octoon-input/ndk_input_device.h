#ifndef OCTOON_INPUT_NDK_DEVICE_H_
#define OCTOON_INPUT_NDK_DEVICE_H_

#include <octoon/input/input_device_base.h>

namespace octoon
{
	namespace input
	{
		class DLL_EXPORT NDKInputDevice : public InputDevice
		{
			OctoonDeclareSubInterface(NDKInputDevice, InputDevice)
		public:
			NDKInputDevice() noexcept;

			virtual void setCaptureObject(CaptureObject window) noexcept;
			virtual CaptureObject getCaptureObject() const noexcept;

			virtual bool injectTouchMove(int _absx, int _absy, InputButton::Code _id) noexcept;
			virtual bool injectTouchPress(int _absx, int _absy, InputButton::Code _id) noexcept;
			virtual bool injectTouchRelease(int _absx, int _absy, InputButton::Code _id) noexcept;
			virtual bool injectTouchCancel(InputButton::Code _id) noexcept;

			virtual bool injectCharacter(InputKey::Code _key, std::uint32_t _char) noexcept;

			virtual bool peekEvents(InputEvent& event) noexcept;
			virtual bool pollEvents(InputEvent& event) noexcept;
			virtual bool waitEvents(InputEvent& event) noexcept;
			virtual bool waitEvents(InputEvent& event, int timeout) noexcept;
			virtual void flushEvent() noexcept;

		private:
			NDKInputDevice(const NDKInputDevice&) noexcept = delete;
			NDKInputDevice& operator=(const NDKInputDevice&) noexcept = delete;

		private:

			bool _isButtonPress;

			int _button[InputButton::Code::NumButtonCodes];

			int _mouseX[InputButton::Code::NumButtonCodes];
			int _mouseY[InputButton::Code::NumButtonCodes];
		};
	}
}

#endif