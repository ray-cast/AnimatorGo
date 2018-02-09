#include <octoon/input/input.h>

#if defined(_BUILD_PLATFORM_WINDOWS)
#	include "msw_input_device.h"
#	include "msw_input_keyboard.h"
#	include "msw_input_mouse.h"
#else
#	include <octoon/input/input_device.h>
#	include <octoon/input/input_keyboard.h>
#	include <octoon/input/input_mouse.h>
#endif

namespace octoon
{
	namespace input
	{
		OctoonImplementSubClass(DefaultInput, Input, "DefaultInput")

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
			_inputDevice = std::make_shared<DefaultInputDevice>();
#if defined(_BUILD_PLATFORM_WINDOWS)
			this->obtainMouseCapture(std::make_shared<MSWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<MSWInputKeyboard>());
#else
			this->obtainMouseCapture(std::make_shared<DefaultInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<DefaultInputKeyboard>());
#endif
			return true;
		}

		bool
			DefaultInput::open(InputDevicePtr& device) noexcept
		{
			_inputDevice = device ? device : std::make_shared<DefaultInputDevice>();
#if defined(_BUILD_PLATFORM_WINDOWS)
			this->obtainMouseCapture(std::make_shared<MSWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<MSWInputKeyboard>());
#else
			this->obtainMouseCapture(std::make_shared<DefaultInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<DefaultInputKeyboard>());
#endif
			return true;
		}

		bool
			DefaultInput::open(InputDevicePtr&& device) noexcept
		{
			_inputDevice = device ? std::move(device) : std::make_shared<DefaultInputDevice>();
#if defined(_BUILD_PLATFORM_WINDOWS)
			this->obtainMouseCapture(std::make_shared<MSWInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<MSWInputKeyboard>());
#else
			this->obtainMouseCapture(std::make_shared<DefaultInputMouse>());
			this->obtainKeyboardCapture(std::make_shared<DefaultInputKeyboard>());
#endif
			return true;
		}

		void
			DefaultInput::close() noexcept
		{
			if (_inputDevice)
			{
				_inputDevice.reset();
				_inputDevice = nullptr;
			}

			if (_mouseCaptureDevice)
			{
				_mouseCaptureDevice->releaseCapture();
				_mouseCaptureDevice.reset();
				_mouseCaptureDevice = nullptr;
			}

			if (_keyboardCaptureDevice)
			{
				_keyboardCaptureDevice->releaseCapture();
				_keyboardCaptureDevice.reset();
				_keyboardCaptureDevice = nullptr;
			}
		}

		void
			DefaultInput::setCaptureObject(WindHandle window) noexcept
		{
			if (_inputDevice)
				_inputDevice->setCaptureObject(window);
		}

		WindHandle
			DefaultInput::getCaptureObject() const noexcept
		{
			if (_inputDevice)
				return _inputDevice->getCaptureObject();
			return nullptr;
		}

		float
			DefaultInput::getAxis(InputAxis::Code axis) const noexcept
		{
			if (_mouseCaptureDevice)
				return _mouseCaptureDevice->getAxis(axis);
			return 0.0f;
		}

		void
			DefaultInput::setMousePos(InputButton::mouse_t x, InputButton::mouse_t y) noexcept
		{
			if (_mouseCaptureDevice)
				_mouseCaptureDevice->setPosition(x, y);
		}

		void
			DefaultInput::getMousePos(InputButton::mouse_t& x, InputButton::mouse_t& y) const noexcept
		{
			if (_mouseCaptureDevice)
				_mouseCaptureDevice->getPosition(x, y);
		}

		bool
			DefaultInput::isKeyDown(InputKey::Code key) const noexcept
		{
			if (_keyboardCaptureDevice)
				return _keyboardCaptureDevice->isKeyDown(key);

			return false;
		}

		bool
			DefaultInput::isKeyUp(InputKey::Code key) const noexcept
		{
			if (_keyboardCaptureDevice)
				return _keyboardCaptureDevice->isKeyUp(key);

			return false;
		}

		bool
			DefaultInput::isKeyPressed(InputKey::Code key) const noexcept
		{
			if (_keyboardCaptureDevice)
				return _keyboardCaptureDevice->isKeyPressed(key);

			return false;
		}

		bool
			DefaultInput::isButtonDown(InputButton::Code key) const noexcept
		{
			if (_mouseCaptureDevice)
				return _mouseCaptureDevice->isButtonDown(key);

			return false;
		}

		bool
			DefaultInput::isButtonUp(InputButton::Code key) const noexcept
		{
			if (_mouseCaptureDevice)
				return _mouseCaptureDevice->isButtonUp(key);

			return false;
		}

		bool
			DefaultInput::isButtonPressed(InputButton::Code key) const noexcept
		{
			if (_mouseCaptureDevice)
				return _mouseCaptureDevice->isButtonPressed(key);

			return false;
		}

		void
			DefaultInput::showCursor(bool show) noexcept
		{
			if (_mouseCaptureDevice)
			{
				if (show)
					_mouseCaptureDevice->showMouse();
				else
					_mouseCaptureDevice->hideMouse();
			}
		}

		bool
			DefaultInput::isShowCursor() const noexcept
		{
			if (_mouseCaptureDevice)
				return _mouseCaptureDevice->isShowMouse();
			return false;
		}

		void
			DefaultInput::lockCursor(bool lock) noexcept
		{
			if (_mouseCaptureDevice)
			{
				if (lock)
					_mouseCaptureDevice->lockMouse();
				else
					_mouseCaptureDevice->unlockMouse();
			}
		}

		bool
			DefaultInput::isLockedCursor() const noexcept
		{
			if (_mouseCaptureDevice)
				return _mouseCaptureDevice->isLockedMouse();
			return false;
		}

		void
			DefaultInput::obtainMouseCapture() noexcept
		{
			if (_mouseCaptureDevice && !_mouseCaptureDevice->capture())
				_mouseCaptureDevice->obtainCapture();
		}

		void
			DefaultInput::obtainKeyboardCapture() noexcept
		{
			if (_keyboardCaptureDevice && !_keyboardCaptureDevice->capture())
				_keyboardCaptureDevice->obtainCapture();
		}

		void
			DefaultInput::obtainMouseCapture(InputMousePtr& mouse) noexcept
		{
			if (_mouseCaptureDevice != mouse)
			{
				if (_mouseCaptureDevice)
				{
					_mouseCaptureDevice->releaseCapture();

					if (_inputDevice)
						_inputDevice->removeInputListener(_mouseCaptureDevice);
				}

				_mouseCaptureDevice = mouse;

				if (_mouseCaptureDevice)
				{
					_mouseCaptureDevice->obtainCapture();

					if (_inputDevice)
						_inputDevice->addInputListener(_mouseCaptureDevice);
				}
			}
		}

		void
			DefaultInput::obtainMouseCapture(InputMousePtr&& mouse) noexcept
		{
			if (_mouseCaptureDevice != mouse)
			{
				if (_mouseCaptureDevice)
				{
					_mouseCaptureDevice->releaseCapture();

					if (_inputDevice)
						_inputDevice->removeInputListener(_mouseCaptureDevice);
				}

				_mouseCaptureDevice = std::move(mouse);

				if (_mouseCaptureDevice)
				{
					_mouseCaptureDevice->obtainCapture();

					if (_inputDevice)
						_inputDevice->addInputListener(_mouseCaptureDevice);
				}
			}
		}

		void
			DefaultInput::obtainKeyboardCapture(InputKeyboardPtr& keyboard) noexcept
		{
			if (_keyboardCaptureDevice != keyboard)
			{
				if (_keyboardCaptureDevice)
				{
					_keyboardCaptureDevice->releaseCapture();

					if (_inputDevice)
						_inputDevice->removeInputListener(keyboard);
				}

				_keyboardCaptureDevice = keyboard;

				if (_keyboardCaptureDevice)
				{
					_keyboardCaptureDevice->obtainCapture();

					if (_inputDevice)
						_inputDevice->addInputListener(_keyboardCaptureDevice);
				}
			}
		}

		void
			DefaultInput::obtainKeyboardCapture(InputKeyboardPtr&& keyboard) noexcept
		{
			if (_keyboardCaptureDevice != keyboard)
			{
				if (_keyboardCaptureDevice)
				{
					_keyboardCaptureDevice->releaseCapture();

					if (_inputDevice)
						_inputDevice->removeInputListener(keyboard);
				}

				_keyboardCaptureDevice = std::move(keyboard);

				if (_keyboardCaptureDevice)
				{
					_keyboardCaptureDevice->obtainCapture();

					if (_inputDevice)
						_inputDevice->addInputListener(_keyboardCaptureDevice);
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
			if (_mouseCaptureDevice && _mouseCaptureDevice->capture())
				_mouseCaptureDevice->releaseCapture();
		}

		void
			DefaultInput::releaseKeyboardCapture() noexcept
		{
			if (_keyboardCaptureDevice && _keyboardCaptureDevice->capture())
				_keyboardCaptureDevice->releaseCapture();
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
			if (_mouseCaptureDevice)
				_mouseCaptureDevice->onReset();

			if (_keyboardCaptureDevice)
				_keyboardCaptureDevice->onReset();
		}

		void
			DefaultInput::addInputListener(InputListenerPtr& listener) noexcept
		{
			if (_inputDevice)
				_inputDevice->addInputListener(listener);
		}

		void
			DefaultInput::addInputListener(InputListenerPtr&& listener) noexcept
		{
			if (_inputDevice)
				_inputDevice->addInputListener(std::move(listener));
		}

		void
			DefaultInput::removeInputListener(InputListenerPtr& listener) noexcept
		{
			if (_inputDevice)
				_inputDevice->removeInputListener(listener);
		}

		void
			DefaultInput::removeInputListener(InputListenerPtr&& listener) noexcept
		{
			if (_inputDevice)
				_inputDevice->removeInputListener(std::move(listener));
		}

		void
			DefaultInput::clearInputListener() noexcept
		{
			if (_inputDevice)
				_inputDevice->clearInputListener();
		}

		bool
			DefaultInput::sendInputEvent(const InputEvent& event) noexcept
		{
			if (_inputDevice)
				return _inputDevice->sendEvent(event);
			return false;
		}

		bool
			DefaultInput::postInputEvent(const InputEvent& event) noexcept
		{
			if (_inputDevice)
				return _inputDevice->postEvent(event);
			return true;
		}

		void
			DefaultInput::updateBegin() noexcept
		{
			if (_mouseCaptureDevice)
				_mouseCaptureDevice->onFrameBegin();

			if (_keyboardCaptureDevice)
				_keyboardCaptureDevice->onFrameBegin();
		}

		void
			DefaultInput::update() noexcept
		{
			if (!_inputDevice)
				return;

			InputEvent event;
			while (_inputDevice->pollEvents(event))
			{
				this->sendInputEvent(event);
			}
		}

		void
			DefaultInput::updateEnd() noexcept
		{
			if (_mouseCaptureDevice && _mouseCaptureDevice->capture())
				_mouseCaptureDevice->onFrameEnd();

			if (_keyboardCaptureDevice && _keyboardCaptureDevice->capture())
				_keyboardCaptureDevice->onFrameEnd();
		}

		InputPtr
			DefaultInput::clone() const noexcept
		{
			auto input = std::make_shared<DefaultInput>();
			if (_inputDevice)
				input->open(_inputDevice->clone());

			if (_keyboardCaptureDevice)
				input->obtainKeyboardCapture(_keyboardCaptureDevice->clone());

			if (_mouseCaptureDevice)
				input->obtainMouseCapture(_mouseCaptureDevice->clone());

			return input;
		}
	}
}