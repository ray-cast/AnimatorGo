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
			inputDevice_ = std::make_shared<MSWInputDevice>();

			this->obtainMouseCapture(std::make_shared<MSWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<MSWInputKeyboard>());
#elif defined(OCTOON_FEATURE_INPUT_API_GLFW)
			inputDevice_ = std::make_shared<GLFWInputDevice>();

			this->obtainMouseCapture(std::make_shared<GLFWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<GLFWInputKeyboard>());
#else
			inputDevice_ = std::make_shared<DefaultInputDevice>();

			this->obtainMouseCapture(std::make_shared<DefaultInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<DefaultInputKeyboard>());
#endif
			return true;
		}

		bool
		DefaultInput::open(const IInputDevicePtr& device) noexcept
		{
#if defined(OCTOON_FEATURE_INPUT_API_WINDOWS)
			inputDevice_ = device ? device : std::make_shared<MSWInputDevice>();

			this->obtainMouseCapture(std::make_shared<MSWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<MSWInputKeyboard>());
#elif defined(OCTOON_FEATURE_INPUT_API_GLFW)
			inputDevice_ = device ? device : std::make_shared<GLFWInputDevice>();

			this->obtainMouseCapture(std::make_shared<GLFWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<GLFWInputKeyboard>());
#else
			inputDevice_ = device ? device : std::make_shared<DefaultInputDevice>();

			this->obtainMouseCapture(std::make_shared<DefaultInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<DefaultInputKeyboard>());
#endif
			return true;
		}

		bool
		DefaultInput::open(IInputDevicePtr&& device) noexcept
		{
#if defined(OCTOON_FEATURE_INPUT_API_WINDOWS)
			inputDevice_ = device ? std::move(device) : std::make_shared<MSWInputDevice>();

			this->obtainMouseCapture(std::make_shared<MSWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<MSWInputKeyboard>());
#elif defined(OCTOON_FEATURE_INPUT_API_GLFW)
			inputDevice_ = device ? std::move(device) : std::make_shared<GLFWInputDevice>();

			this->obtainMouseCapture(std::make_shared<GLFWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<GLFWInputKeyboard>());
#else
			inputDevice_ = device ? std::move(device) : std::make_shared<DefaultInputDevice>();

			this->obtainMouseCapture(std::make_shared<DefaultInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<DefaultInputKeyboard>());
#endif
			return true;
		}

		void
		DefaultInput::close() noexcept
		{
			if (inputDevice_)
			{
				inputDevice_.reset();
				inputDevice_ = nullptr;
			}

			if (mouseCaptureDevice_)
			{
				mouseCaptureDevice_->releaseCapture();
				mouseCaptureDevice_.reset();
				mouseCaptureDevice_ = nullptr;
			}

			if (keyboardCaptureDevice_)
			{
				keyboardCaptureDevice_->releaseCapture();
				keyboardCaptureDevice_.reset();
				keyboardCaptureDevice_ = nullptr;
			}
		}

		void
		DefaultInput::setCaptureObject(WindHandle window) noexcept
		{
			if (inputDevice_)
				inputDevice_->setCaptureObject(window);
		}

		WindHandle
		DefaultInput::getCaptureObject() const noexcept
		{
			if (inputDevice_)
				return inputDevice_->getCaptureObject();
			return nullptr;
		}

		float
		DefaultInput::getAxis(InputAxis::Code axis) const noexcept
		{
			if (mouseCaptureDevice_)
				return mouseCaptureDevice_->getAxis(axis);
			return 0.0f;
		}

		void
		DefaultInput::setMousePos(InputButton::Type x, InputButton::Type y) noexcept
		{
			if (mouseCaptureDevice_)
				mouseCaptureDevice_->setPosition(x, y);
		}

		void
		DefaultInput::getMousePos(InputButton::Type& x, InputButton::Type& y) const noexcept
		{
			if (mouseCaptureDevice_)
				mouseCaptureDevice_->getPosition(x, y);
		}

		bool
		DefaultInput::isKeyDown(InputKey::Code key) const noexcept
		{
			if (keyboardCaptureDevice_)
				return keyboardCaptureDevice_->isKeyDown(key);

			return false;
		}

		bool
		DefaultInput::isKeyUp(InputKey::Code key) const noexcept
		{
			if (keyboardCaptureDevice_)
				return keyboardCaptureDevice_->isKeyUp(key);

			return false;
		}

		bool
		DefaultInput::isKeyPressed(InputKey::Code key) const noexcept
		{
			if (keyboardCaptureDevice_)
				return keyboardCaptureDevice_->isKeyPressed(key);

			return false;
		}

		bool
		DefaultInput::isButtonDown(InputButton::Code key) const noexcept
		{
			if (mouseCaptureDevice_)
				return mouseCaptureDevice_->isButtonDown(key);

			return false;
		}

		bool
		DefaultInput::isButtonUp(InputButton::Code key) const noexcept
		{
			if (mouseCaptureDevice_)
				return mouseCaptureDevice_->isButtonUp(key);

			return false;
		}

		bool
		DefaultInput::isButtonPressed(InputButton::Code key) const noexcept
		{
			if (mouseCaptureDevice_)
				return mouseCaptureDevice_->isButtonPressed(key);

			return false;
		}

		void
		DefaultInput::showCursor(bool show) noexcept
		{
			if (mouseCaptureDevice_)
			{
				if (show)
					mouseCaptureDevice_->showMouse();
				else
					mouseCaptureDevice_->hideMouse();
			}
		}

		bool
		DefaultInput::isShowCursor() const noexcept
		{
			if (mouseCaptureDevice_)
				return mouseCaptureDevice_->isShowMouse();
			return false;
		}

		void
		DefaultInput::lockCursor(bool lock) noexcept
		{
			if (mouseCaptureDevice_)
			{
				if (lock)
					mouseCaptureDevice_->lockMouse();
				else
					mouseCaptureDevice_->unlockMouse();
			}
		}

		bool
		DefaultInput::isLockedCursor() const noexcept
		{
			if (mouseCaptureDevice_)
				return mouseCaptureDevice_->isLockedMouse();
			return false;
		}

		void
		DefaultInput::obtainMouseCapture() noexcept
		{
			if (mouseCaptureDevice_ && !mouseCaptureDevice_->capture())
				mouseCaptureDevice_->obtainCapture();
		}

		void
		DefaultInput::obtainKeyboardCapture() noexcept
		{
			if (keyboardCaptureDevice_ && !keyboardCaptureDevice_->capture())
				keyboardCaptureDevice_->obtainCapture();
		}

		void
		DefaultInput::obtainMouseCapture(const IInputMousePtr& mouse) noexcept
		{
			if (mouseCaptureDevice_ != mouse)
			{
				if (mouseCaptureDevice_)
				{
					mouseCaptureDevice_->releaseCapture();

					if (inputDevice_)
						inputDevice_->removeInputListener(mouseCaptureDevice_);
				}

				mouseCaptureDevice_ = mouse;

				if (mouseCaptureDevice_)
				{
					mouseCaptureDevice_->obtainCapture();

					if (inputDevice_)
						inputDevice_->addInputListener(mouseCaptureDevice_);
				}
			}
		}

		void
		DefaultInput::obtainMouseCapture(IInputMousePtr&& mouse) noexcept
		{
			if (mouseCaptureDevice_ != mouse)
			{
				if (mouseCaptureDevice_)
				{
					mouseCaptureDevice_->releaseCapture();

					if (inputDevice_)
						inputDevice_->removeInputListener(mouseCaptureDevice_);
				}

				mouseCaptureDevice_ = std::move(mouse);

				if (mouseCaptureDevice_)
				{
					mouseCaptureDevice_->obtainCapture();

					if (inputDevice_)
						inputDevice_->addInputListener(mouseCaptureDevice_);
				}
			}
		}

		void
		DefaultInput::obtainKeyboardCapture(const IInputKeyboardPtr& keyboard) noexcept
		{
			if (keyboardCaptureDevice_ != keyboard)
			{
				if (keyboardCaptureDevice_)
				{
					keyboardCaptureDevice_->releaseCapture();

					if (inputDevice_)
						inputDevice_->removeInputListener(keyboard);
				}

				keyboardCaptureDevice_ = keyboard;

				if (keyboardCaptureDevice_)
				{
					keyboardCaptureDevice_->obtainCapture();

					if (inputDevice_)
						inputDevice_->addInputListener(keyboardCaptureDevice_);
				}
			}
		}

		void
		DefaultInput::obtainKeyboardCapture(IInputKeyboardPtr&& keyboard) noexcept
		{
			if (keyboardCaptureDevice_ != keyboard)
			{
				if (keyboardCaptureDevice_)
				{
					keyboardCaptureDevice_->releaseCapture();

					if (inputDevice_)
						inputDevice_->removeInputListener(keyboard);
				}

				keyboardCaptureDevice_ = std::move(keyboard);

				if (keyboardCaptureDevice_)
				{
					keyboardCaptureDevice_->obtainCapture();

					if (inputDevice_)
						inputDevice_->addInputListener(keyboardCaptureDevice_);
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
			if (mouseCaptureDevice_ && mouseCaptureDevice_->capture())
				mouseCaptureDevice_->releaseCapture();
		}

		void
		DefaultInput::releaseKeyboardCapture() noexcept
		{
			if (keyboardCaptureDevice_ && keyboardCaptureDevice_->capture())
				keyboardCaptureDevice_->releaseCapture();
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
			if (mouseCaptureDevice_)
				mouseCaptureDevice_->onReset();

			if (keyboardCaptureDevice_)
				keyboardCaptureDevice_->onReset();
		}

		void
		DefaultInput::addInputListener(const IInputListenerPtr& listener) noexcept
		{
			if (inputDevice_)
				inputDevice_->addInputListener(listener);
		}

		void
		DefaultInput::addInputListener(IInputListenerPtr&& listener) noexcept
		{
			if (inputDevice_)
				inputDevice_->addInputListener(std::move(listener));
		}

		void
		DefaultInput::removeInputListener(const IInputListenerPtr& listener) noexcept
		{
			if (inputDevice_)
				inputDevice_->removeInputListener(listener);
		}

		void
		DefaultInput::removeInputListener(IInputListenerPtr&& listener) noexcept
		{
			if (inputDevice_)
				inputDevice_->removeInputListener(std::move(listener));
		}

		void
		DefaultInput::clearInputListener() noexcept
		{
			if (inputDevice_)
				inputDevice_->clearInputListener();
		}

		bool
		DefaultInput::sendInputEvent(const InputEvent& event) noexcept
		{
			if (inputDevice_)
				return inputDevice_->sendEvent(event);
			return false;
		}

		bool
		DefaultInput::postInputEvent(const InputEvent& event) noexcept
		{
			if (inputDevice_)
				return inputDevice_->postEvent(event);
			return true;
		}

		void
		DefaultInput::updateBegin() noexcept
		{
			if (mouseCaptureDevice_)
				mouseCaptureDevice_->onFrameBegin();

			if (keyboardCaptureDevice_)
				keyboardCaptureDevice_->onFrameBegin();
		}

		void
		DefaultInput::update() noexcept
		{
			if (!inputDevice_)
				return;

			InputEvent event;
			while (inputDevice_->pollEvents(event))
			{
				this->sendInputEvent(event);
			}
		}

		void
		DefaultInput::updateEnd() noexcept
		{
			if (mouseCaptureDevice_)
				mouseCaptureDevice_->onFrameEnd();

			if (keyboardCaptureDevice_)
				keyboardCaptureDevice_->onFrameEnd();
		}

		IInputPtr
		DefaultInput::clone() const noexcept
		{
			auto input = std::make_shared<DefaultInput>();
			if (inputDevice_)
				input->open(inputDevice_->clone());

			if (keyboardCaptureDevice_)
				input->obtainKeyboardCapture(keyboardCaptureDevice_->clone());

			if (mouseCaptureDevice_)
				input->obtainMouseCapture(mouseCaptureDevice_->clone());

			return input;
		}
	}
}