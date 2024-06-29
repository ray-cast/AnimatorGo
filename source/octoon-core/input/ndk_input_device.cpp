#include "ndk_input_device.h"

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(NDKInputDevice, InputDevice, "NDKInputDevice")

		NDKInputDevice::NDKInputDevice() noexcept
		{
		}

		void
		NDKInputDevice::setCaptureObject(CaptureObject window) noexcept
		{
		}

		CaptureObject
		NDKInputDevice::getCaptureObject() const noexcept
		{
			return nullptr;
		}

		bool 
		NDKInputDevice::injectTouchMove(int _absx, int _absy, InputButton::Code _id) noexcept
		{
			ray::InputEvent inputEvent;
			inputEvent.event = ray::InputEvent::TouchMotionMove;
			inputEvent.motion.x = _absx;
			inputEvent.motion.y = _absy;
			inputEvent.motion.xrel = _absx;
			inputEvent.motion.yrel = _absy;
			inputEvent.motion.state = _button[_id];
			inputEvent.motion.timestamp = ::clock();
			inputEvent.button.button = _id;

			this->postEvent(inputEvent);
		}

		bool 
		NDKInputDevice::injectTouchPress(int _absx, int _absy, InputButton::Code _id) noexcept
		{
			ray::InputEvent inputEvent;
			inputEvent.event = ray::InputEvent::TouchMotionDown;
			inputEvent.motion.x = _absx;
			inputEvent.motion.y = _absy;
			inputEvent.motion.xrel = _absx;
			inputEvent.motion.yrel = _absy;
			inputEvent.motion.state = _button[_id] = true;
			inputEvent.motion.timestamp = ::clock();
			inputEvent.button.button = _id;

			this->postEvent(inputEvent);
		}

		bool 
		NDKInputDevice::injectTouchRelease(int _absx, int _absy, InputButton::Code _id) noexcept
		{
			ray::InputEvent inputEvent;
			inputEvent.event = ray::InputEvent::TouchMotionUp;
			inputEvent.motion.x = _absx;
			inputEvent.motion.y = _absy;
			inputEvent.motion.xrel = _absx;
			inputEvent.motion.yrel = _absy;
			inputEvent.motion.state = _button[_id] = false;
			inputEvent.motion.timestamp = ::clock();
			inputEvent.button.button = _id;

			this->postEvent(inputEvent);
			return true;
		}

		bool
		NDKInputDevice::injectTouchCancel(InputButton::Code _id) noexcept
		{
			ray::InputEvent inputEvent;
			inputEvent.event = ray::InputEvent::TouchMotionCancel;
			inputEvent.motion.x = 0;
			inputEvent.motion.y = 0;
			inputEvent.motion.xrel = 0;
			inputEvent.motion.yrel = 0;
			inputEvent.motion.state = _button[_id] = false;
			inputEvent.motion.timestamp = ::clock();
			inputEvent.button.button = _id;

			this->postEvent(inputEvent);
			return true;
		}

		bool 
		NDKInputDevice::injectCharacter(InputKey::Code _key, std::uint32_t _char) noexcept
		{
			ray::InputEvent inputEvent;
			inputEvent.event = ray::InputEvent::Character;
			inputEvent.key.state = true;
			inputEvent.key.timestamp = ::clock();
			//inputEvent.key.keysym.raw = AKeyEvent_getScanCode(event);
			inputEvent.key.keysym.sym = _key;
			inputEvent.key.keysym.unicode = _char;

			this->postEvent(inputEvent);
			return true;
		}

		bool 
		NDKInputDevice::peekEvents(InputEvent& event) noexcept
		{
			return false;
		}

		bool 
		NDKInputDevice::pollEvents(InputEvent& event) noexcept
		{
			return false;
		}

		bool 
		NDKInputDevice::waitEvents(InputEvent& event) noexcept
		{
			return false;
		}

		bool 
		NDKInputDevice::waitEvents(InputEvent& event, int timeout) noexcept
		{
			return false;
		}

		void 
		NDKInputDevice::flushEvent() noexcept
		{
		}
	}
}