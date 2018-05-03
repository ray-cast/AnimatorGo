#include <octoon/input/input.h>
#include <octoon/input/input_device.h>
#include <octoon/input/input_map.h>

#if defined(OCTOON_FEATURE_INPUT_API_WINDOWS)
#	include "msw_input_device.h"
#	include "msw_input_keyboard.h"
#	include "msw_input_mouse.h"
#elif defined(OCTOON_FEATURE_INPUT_API_GLFW)
#	include "glfw_input_device.h"
#	include "glfw_input_keyboard.h"
#	include "glfw_input_mouse.h"
#else
#	include <octoon/input/input_device.h>
#	include <octoon/input/input_keyboard.h>
#	include <octoon/input/input_mouse.h>
#endif

#include <algorithm>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubClass(DefaultInput, IInput, "DefaultInput")

		DefaultInput::DefaultInput() noexcept
		{
		}

		DefaultInput::~DefaultInput() noexcept
		{
			this->close();
		}

		bool
		DefaultInput::open() noexcept
		{
#if defined(OCTOON_FEATURE_INPUT_API_WINDOWS)
			input_device_ = std::make_shared<MSWInputDevice>();

			this->obtainMouseCapture(std::make_shared<MSWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<MSWInputKeyboard>());
#elif defined(OCTOON_FEATURE_INPUT_API_GLFW)
			input_device_ = std::make_shared<GLFWInputDevice>();

			this->obtainMouseCapture(std::make_shared<GLFWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<GLFWInputKeyboard>());
#else
			input_device_ = std::make_shared<DefaultInputDevice>();

			this->obtainMouseCapture(std::make_shared<DefaultInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<DefaultInputKeyboard>());
#endif
			return true;
		}

		bool
		DefaultInput::open(const IInputDevicePtr& device) noexcept
		{
#if defined(OCTOON_FEATURE_INPUT_API_WINDOWS)
			input_device_ = device ? device : std::make_shared<MSWInputDevice>();

			this->obtainMouseCapture(std::make_shared<MSWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<MSWInputKeyboard>());
#elif defined(OCTOON_FEATURE_INPUT_API_GLFW)
			input_device_ = device ? device : std::make_shared<GLFWInputDevice>();

			this->obtainMouseCapture(std::make_shared<GLFWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<GLFWInputKeyboard>());
#else
			input_device_ = device ? device : std::make_shared<DefaultInputDevice>();

			this->obtainMouseCapture(std::make_shared<DefaultInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<DefaultInputKeyboard>());
#endif
			return true;
		}

		bool
		DefaultInput::open(IInputDevicePtr&& device) noexcept
		{
#if defined(OCTOON_FEATURE_INPUT_API_WINDOWS)
			input_device_ = device ? std::move(device) : std::make_shared<MSWInputDevice>();

			this->obtainMouseCapture(std::make_shared<MSWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<MSWInputKeyboard>());
#elif defined(OCTOON_FEATURE_INPUT_API_GLFW)
			input_device_ = device ? std::move(device) : std::make_shared<GLFWInputDevice>();

			this->obtainMouseCapture(std::make_shared<GLFWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<GLFWInputKeyboard>());
#else
			input_device_ = device ? std::move(device) : std::make_shared<DefaultInputDevice>();

			this->obtainMouseCapture(std::make_shared<DefaultInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<DefaultInputKeyboard>());
#endif
			return true;
		}

		void
		DefaultInput::close() noexcept
		{
			if (input_device_)
			{
				input_device_.reset();
				input_device_ = nullptr;
			}

			if (mouse_capture_device_)
			{
				mouse_capture_device_->releaseCapture();
				mouse_capture_device_.reset();
				mouse_capture_device_ = nullptr;
			}

			if (keyboard_capture_device_)
			{
				keyboard_capture_device_->releaseCapture();
				keyboard_capture_device_.reset();
				keyboard_capture_device_ = nullptr;
			}
		}

		void
		DefaultInput::setCaptureObject(WindHandle window) noexcept
		{
			if (input_device_)
				input_device_->setCaptureObject(window);
		}

		WindHandle
		DefaultInput::getCaptureObject() const noexcept
		{
			if (input_device_)
				return input_device_->getCaptureObject();
			return nullptr;
		}

		float
		DefaultInput::getAxis(InputAxis::Code axis) const noexcept
		{
			if (mouse_capture_device_)
				return mouse_capture_device_->getAxis(axis);
			return 0.0f;
		}

		void
		DefaultInput::setMousePos(InputButton::Type x, InputButton::Type y) noexcept
		{
			if (mouse_capture_device_)
				mouse_capture_device_->setPosition(x, y);
		}

		void
		DefaultInput::getMousePos(InputButton::Type& x, InputButton::Type& y) const noexcept
		{
			if (mouse_capture_device_)
				mouse_capture_device_->getPosition(x, y);
		}

		bool
		DefaultInput::isKeyDown(InputKey::Code key) const noexcept
		{
			if (keyboard_capture_device_)
				return keyboard_capture_device_->isKeyDown(key);

			return false;
		}

		bool
		DefaultInput::isKeyUp(InputKey::Code key) const noexcept
		{
			if (keyboard_capture_device_)
				return keyboard_capture_device_->isKeyUp(key);

			return false;
		}

		bool
		DefaultInput::isKeyPressed(InputKey::Code key) const noexcept
		{
			if (keyboard_capture_device_)
				return keyboard_capture_device_->isKeyPressed(key);

			return false;
		}

		bool
		DefaultInput::isButtonDown(InputButton::Code key) const noexcept
		{
			if (mouse_capture_device_)
				return mouse_capture_device_->isButtonDown(key);

			return false;
		}

		bool
		DefaultInput::isButtonUp(InputButton::Code key) const noexcept
		{
			if (mouse_capture_device_)
				return mouse_capture_device_->isButtonUp(key);

			return false;
		}

		bool
		DefaultInput::isButtonPressed(InputButton::Code key) const noexcept
		{
			if (mouse_capture_device_)
				return mouse_capture_device_->isButtonPressed(key);

			return false;
		}

		void
		DefaultInput::showCursor(bool show) noexcept
		{
			if (mouse_capture_device_)
			{
				if (show)
					mouse_capture_device_->showMouse();
				else
					mouse_capture_device_->hideMouse();
			}
		}

		bool
		DefaultInput::isShowCursor() const noexcept
		{
			if (mouse_capture_device_)
				return mouse_capture_device_->isShowMouse();
			return false;
		}

		void
		DefaultInput::lockCursor(bool lock) noexcept
		{
			if (mouse_capture_device_)
			{
				if (lock)
					mouse_capture_device_->lockMouse();
				else
					mouse_capture_device_->unlockMouse();
			}
		}

		bool
		DefaultInput::isLockedCursor() const noexcept
		{
			if (mouse_capture_device_)
				return mouse_capture_device_->isLockedMouse();
			return false;
		}

		void
		DefaultInput::obtainMouseCapture() noexcept
		{
			if (mouse_capture_device_ && !mouse_capture_device_->capture())
				mouse_capture_device_->obtainCapture();
		}

		void
		DefaultInput::obtainKeyboardCapture() noexcept
		{
			if (keyboard_capture_device_ && !keyboard_capture_device_->capture())
				keyboard_capture_device_->obtainCapture();
		}

		void
		DefaultInput::obtainMouseCapture(const IInputMousePtr& mouse) noexcept
		{
			if (mouse_capture_device_ != mouse)
			{
				if (mouse_capture_device_)
				{
					mouse_capture_device_->releaseCapture();

					if (input_device_)
						input_device_->removeInputListener(mouse_capture_device_);
				}

				mouse_capture_device_ = mouse;

				if (mouse_capture_device_)
				{
					mouse_capture_device_->obtainCapture();

					if (input_device_)
						input_device_->addInputListener(mouse_capture_device_);
				}
			}
		}

		void
		DefaultInput::obtainMouseCapture(IInputMousePtr&& mouse) noexcept
		{
			if (mouse_capture_device_ != mouse)
			{
				if (mouse_capture_device_)
				{
					mouse_capture_device_->releaseCapture();

					if (input_device_)
						input_device_->removeInputListener(mouse_capture_device_);
				}

				mouse_capture_device_ = std::move(mouse);

				if (mouse_capture_device_)
				{
					mouse_capture_device_->obtainCapture();

					if (input_device_)
						input_device_->addInputListener(mouse_capture_device_);
				}
			}
		}

		void
		DefaultInput::obtainKeyboardCapture(const IInputKeyboardPtr& keyboard) noexcept
		{
			if (keyboard_capture_device_ != keyboard)
			{
				if (keyboard_capture_device_)
				{
					keyboard_capture_device_->releaseCapture();

					if (input_device_)
						input_device_->removeInputListener(keyboard);
				}

				keyboard_capture_device_ = keyboard;

				if (keyboard_capture_device_)
				{
					keyboard_capture_device_->obtainCapture();

					if (input_device_)
						input_device_->addInputListener(keyboard_capture_device_);
				}
			}
		}

		void
		DefaultInput::obtainKeyboardCapture(IInputKeyboardPtr&& keyboard) noexcept
		{
			if (keyboard_capture_device_ != keyboard)
			{
				if (keyboard_capture_device_)
				{
					keyboard_capture_device_->releaseCapture();

					if (input_device_)
						input_device_->removeInputListener(keyboard);
				}

				keyboard_capture_device_ = std::move(keyboard);

				if (keyboard_capture_device_)
				{
					keyboard_capture_device_->obtainCapture();

					if (input_device_)
						input_device_->addInputListener(keyboard_capture_device_);
				}
			}
		}

		void
		DefaultInput::obtainCapture() noexcept
		{
			this->obtainMouseCapture();
			this->obtainKeyboardCapture();
		}

		void
		DefaultInput::releaseMouseCapture() noexcept
		{
			if (mouse_capture_device_ && mouse_capture_device_->capture())
				mouse_capture_device_->releaseCapture();
		}

		void
		DefaultInput::releaseKeyboardCapture() noexcept
		{
			if (keyboard_capture_device_ && keyboard_capture_device_->capture())
				keyboard_capture_device_->releaseCapture();
		}

		void
		DefaultInput::releaseCapture() noexcept
		{
			this->releaseMouseCapture();
			this->releaseKeyboardCapture();
		}

		void
		DefaultInput::reset() noexcept
		{
			if (mouse_capture_device_)
				mouse_capture_device_->onReset();

			if (keyboard_capture_device_)
				keyboard_capture_device_->onReset();
		}

		void
		DefaultInput::addInputListener(const IInputListenerPtr& listener) noexcept
		{
			if (input_device_)
				input_device_->addInputListener(listener);
		}

		void
		DefaultInput::addInputListener(IInputListenerPtr&& listener) noexcept
		{
			if (input_device_)
				input_device_->addInputListener(std::move(listener));
		}

		void
		DefaultInput::removeInputListener(const IInputListenerPtr& listener) noexcept
		{
			if (input_device_)
				input_device_->removeInputListener(listener);
		}

		void
		DefaultInput::removeInputListener(IInputListenerPtr&& listener) noexcept
		{
			if (input_device_)
				input_device_->removeInputListener(std::move(listener));
		}

		void
		DefaultInput::clearInputListener() noexcept
		{
			if (input_device_)
				input_device_->clearInputListener();
		}

		bool
		DefaultInput::sendInputEvent(const InputEvent& event) noexcept
		{
			if (input_device_)
				return input_device_->sendEvent(event);
			return false;
		}

		bool
		DefaultInput::postInputEvent(const InputEvent& event) noexcept
		{
			if (input_device_)
				return input_device_->postEvent(event);
			return true;
		}

		void
		DefaultInput::updateBegin() noexcept
		{
			if (mouse_capture_device_)
				mouse_capture_device_->onFrameBegin();

			if (keyboard_capture_device_)
				keyboard_capture_device_->onFrameBegin();
		}

		void
		DefaultInput::update() noexcept
		{
			if (!input_device_)
				return;

			InputEvent event;
			while (input_device_->pollEvents(event))
			{
				this->sendInputEvent(event);
			}
		}

		void
		DefaultInput::updateEnd() noexcept
		{
			if (mouse_capture_device_ && mouse_capture_device_->capture())
				mouse_capture_device_->onFrameEnd();

			if (keyboard_capture_device_ && keyboard_capture_device_->capture())
				keyboard_capture_device_->onFrameEnd();
		}

		IInputPtr
		DefaultInput::clone() const noexcept
		{
			auto input = std::make_shared<DefaultInput>();
			if (input_device_)
				input->open(input_device_->clone());

			if (keyboard_capture_device_)
				input->obtainKeyboardCapture(keyboard_capture_device_->clone());

			if (mouse_capture_device_)
				input->obtainMouseCapture(mouse_capture_device_->clone());

			return input;
		}
	}
}