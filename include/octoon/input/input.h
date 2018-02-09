#ifndef OCTOON_INPUT_H_
#define OCTOON_INPUT_H_

#include <octoon/input/input_base.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT DefaultInput final : public Input
		{
			OctoonDeclareSubClass(DefaultInput, Input)
		public:
			DefaultInput() noexcept;
			~DefaultInput() noexcept;

			virtual bool open() noexcept;
			virtual bool open(InputDevicePtr& device) noexcept;
			virtual bool open(InputDevicePtr&& device) noexcept;
			virtual void close() noexcept;

			virtual void setCaptureObject(WindHandle window) noexcept;
			virtual WindHandle getCaptureObject() const noexcept;

			virtual float getAxis(InputAxis::Code axis) const noexcept;

			virtual void setMousePos(InputButton::mouse_t x, InputButton::mouse_t y) noexcept;
			virtual void getMousePos(InputButton::mouse_t& x, InputButton::mouse_t& y) const noexcept;

			virtual bool isKeyDown(InputKey::Code key) const noexcept;
			virtual bool isKeyUp(InputKey::Code key) const noexcept;
			virtual bool isKeyPressed(InputKey::Code key) const noexcept;

			virtual bool isButtonDown(InputButton::Code key) const noexcept;
			virtual bool isButtonUp(InputButton::Code key) const noexcept;
			virtual bool isButtonPressed(InputButton::Code key) const noexcept;

			virtual void showCursor(bool show) noexcept;
			virtual bool isShowCursor() const noexcept;

			virtual void lockCursor(bool lock) noexcept;
			virtual bool isLockedCursor() const noexcept;

			virtual void obtainMouseCapture() noexcept;
			virtual void obtainKeyboardCapture() noexcept;

			virtual void obtainMouseCapture(InputMousePtr& mouse) noexcept;
			virtual void obtainMouseCapture(InputMousePtr&& mouse) noexcept;
			virtual void obtainKeyboardCapture(InputKeyboardPtr& key) noexcept;
			virtual void obtainKeyboardCapture(InputKeyboardPtr&& key) noexcept;
			virtual void obtainCapture() noexcept;

			virtual void releaseMouseCapture() noexcept;
			virtual void releaseKeyboardCapture() noexcept;
			virtual void releaseCapture() noexcept;

			virtual void reset() noexcept;

			virtual void addInputListener(InputListenerPtr& listener) noexcept;
			virtual void addInputListener(InputListenerPtr&& listener) noexcept;
			virtual void removeInputListener(InputListenerPtr& listener) noexcept;
			virtual void removeInputListener(InputListenerPtr&& listener) noexcept;
			virtual void clearInputListener() noexcept;

			virtual bool sendInputEvent(const InputEvent& event) noexcept;
			virtual bool postInputEvent(const InputEvent& event) noexcept;

			virtual void updateBegin() noexcept;
			virtual void update() noexcept;
			virtual void updateEnd() noexcept;

			virtual InputPtr clone() const noexcept;

		private:
			DefaultInput(const DefaultInput&) noexcept = delete;
			DefaultInput& operator=(const DefaultInput&) noexcept = delete;

		private:
			InputDevicePtr _inputDevice;
			InputMousePtr _mouseCaptureDevice;
			InputKeyboardPtr _keyboardCaptureDevice;
		};
	}
}

#endif