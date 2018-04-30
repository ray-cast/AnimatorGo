#ifndef OCTOON_INPUT_BASE_H_
#define OCTOON_INPUT_BASE_H_

#include <octoon/input/iinput_device.h>
#include <octoon/input/iinput_keyboard.h>
#include <octoon/input/iinput_mouse.h>

#include <string>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT IInput : public runtime::RttiInterface
		{
			OctoonDeclareSubInterface(IInput, runtime::RttiInterface)
		public:
			IInput() = default;
			virtual ~IInput() = default;

			virtual bool open() noexcept = 0;
			virtual bool open(const IInputDevicePtr& device) noexcept = 0;
			virtual bool open(IInputDevicePtr&& device) noexcept = 0;
			virtual void close() noexcept = 0;

			virtual void setCaptureObject(WindHandle window) noexcept = 0;
			virtual WindHandle getCaptureObject() const noexcept = 0;

			virtual float getAxis(InputAxis::Code axis) const noexcept = 0;

			virtual void setMousePos(InputButton::Type x, InputButton::Type y) noexcept = 0;
			virtual void getMousePos(InputButton::Type& x, InputButton::Type& y) const noexcept = 0;

			virtual bool isInput(const std::string& id) const = 0;

			virtual bool isKeyDown(InputKey::Code key) const noexcept = 0;
			virtual bool isKeyUp(InputKey::Code key) const noexcept = 0;
			virtual bool isKeyPressed(InputKey::Code key) const noexcept = 0;

			virtual bool isButtonDown(InputButton::Code key) const noexcept = 0;
			virtual bool isButtonUp(InputButton::Code key) const noexcept = 0;
			virtual bool isButtonPressed(InputButton::Code key) const noexcept = 0;

			virtual void showCursor(bool show) noexcept = 0;
			virtual bool isShowCursor() const noexcept = 0;

			virtual void lockCursor(bool lock) noexcept = 0;
			virtual bool isLockedCursor() const noexcept = 0;

			virtual void obtainMouseCapture() noexcept = 0;
			virtual void obtainKeyboardCapture() noexcept = 0;

			virtual void obtainMouseCapture(const IInputMousePtr& mouse) noexcept = 0;
			virtual void obtainMouseCapture(IInputMousePtr&& mouse) noexcept = 0;
			virtual void obtainKeyboardCapture(const IInputKeyboardPtr& key) noexcept = 0;
			virtual void obtainKeyboardCapture(IInputKeyboardPtr&& key) noexcept = 0;
			virtual void obtainCapture() noexcept = 0;

			virtual void releaseMouseCapture() noexcept = 0;
			virtual void releaseKeyboardCapture() noexcept = 0;
			virtual void releaseCapture() noexcept = 0;

			virtual void reset() noexcept = 0;

			virtual void addInputListener(const IInputListenerPtr& listener) noexcept = 0;
			virtual void addInputListener(IInputListenerPtr&& listener) noexcept = 0;
			virtual void removeInputListener(const IInputListenerPtr& listener) noexcept = 0;
			virtual void removeInputListener(IInputListenerPtr&& listener) noexcept = 0;
			virtual void clearInputListener() noexcept = 0;

			virtual bool sendInputEvent(const InputEvent& event) noexcept = 0;
			virtual bool postInputEvent(const InputEvent& event) noexcept = 0;

			virtual void updateBegin() noexcept = 0;
			virtual void update() noexcept = 0;
			virtual void updateEnd() noexcept = 0;

			virtual IInputPtr clone() const noexcept = 0;

		private:
			IInput(const IInput&) noexcept = delete;
			IInput& operator=(const IInput&) noexcept = delete;
		};
	}
}

#endif