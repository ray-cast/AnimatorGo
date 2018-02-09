#include <octoon/input/input_mouse.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(DefaultInputMouse, InputMouse, "DefaultInputMouse")

		DefaultInputMouse::DefaultInputMouse() noexcept
			: _mouseX(0)
			, _mouseY(0)
			, _mouseAxisX(0)
			, _mouseAxisY(0)
			, _lastX(0)
			, _lastY(0)
			, _centerX(0)
			, _centerY(0)
			, _isMouseLock(false)
			, _isMouseLocked(false)
			, _isMouseHide(false)
		{
			std::memset(_buttonState, 0, sizeof(_buttonState));
		}

		DefaultInputMouse::~DefaultInputMouse() noexcept
		{
		}

		void
		DefaultInputMouse::lockMouse() noexcept
		{
			if (!isLockedMouse())
			{
				this->hideMouse();
				_isMouseLock = true;
				_isMouseLocked = true;
			}
		}

		void
		DefaultInputMouse::unlockMouse() noexcept
		{
			if (isLockedMouse())
			{
				this->showMouse();
				_isMouseLock = false;
				_isMouseLocked = false;
			}
		}

		bool
		DefaultInputMouse::isLockedMouse() const noexcept
		{
			return _isMouseLocked;
		}

		void
		DefaultInputMouse::showMouse() noexcept
		{
			if (_isMouseHide)
			{
				this->onShowMouse();
				_isMouseHide = false;
			}
		}

		void
		DefaultInputMouse::hideMouse() noexcept
		{
			if (!_isMouseHide)
			{
				this->onHideMouse();
				_isMouseHide = true;
			}
		}

		bool
		DefaultInputMouse::isShowMouse() noexcept
		{
			return !_isMouseHide;
		}

		float
		DefaultInputMouse::getAxis(InputAxis::Code axis) const noexcept
		{
			switch (axis)
			{
			case InputAxis::MouseX:
				return _mouseAxisX;
			case InputAxis::MouseY:
				return _mouseAxisY;
			case InputAxis::Horizontal:
			{
				float mouseX = 0.0f, mouseY = 0.0f;
				this->getPosition(mouseX, mouseY);

				return (float)(mouseX - _centerX) / _centerX;
			}
			break;
			case InputAxis::Vertical:
			{
				float mouseX = 0.0f, mouseY = 0.0f;
				this->getPosition(mouseX, mouseY);

				return (float)(mouseY - _centerY) / _centerY;
			}
			break;
			default:
				assert(false);
				return 0.0;
			}
		}

		void
		DefaultInputMouse::setPosition(InputButton::mouse_t x, InputButton::mouse_t y) noexcept
		{
			_mouseX = x;
			_mouseY = y;
			this->onChangePosition(x, y);
		}

		void
		DefaultInputMouse::getPosition(InputButton::mouse_t& x, InputButton::mouse_t& y) const noexcept
		{
			x = _mouseX;
			y = _mouseY;
		}

		bool
		DefaultInputMouse::isButtonDown(InputButton::Code key) const noexcept
		{
			return _buttonState[key].down;
		}

		bool
		DefaultInputMouse::isButtonUp(InputButton::Code key) const noexcept
		{
			return _buttonState[key].up;
		}

		bool
		DefaultInputMouse::isButtonPressed(InputButton::Code key) const noexcept
		{
			return _buttonState[key].pressed;
		}

		InputMousePtr
		DefaultInputMouse::clone() const noexcept
		{
			return std::make_shared<DefaultInputMouse>();
		}

		void
		DefaultInputMouse::onShowMouse() noexcept
		{
		}

		void
		DefaultInputMouse::onHideMouse() noexcept
		{
		}

		void
		DefaultInputMouse::onFrameBegin() noexcept
		{
		}

		void
		DefaultInputMouse::onFrameEnd() noexcept
		{
			if (this->isLockedMouse())
				this->setPosition(_centerX, _centerY);

			for (auto& button : _buttonState)
			{
				if (button.up)
					button.pressed = false;

				button.up = false;
				button.down = false;
				button.doubleClick = false;
			}
		}

		void
		DefaultInputMouse::onObtainCapture() noexcept
		{
			if (_isMouseLock && !_isMouseLocked)
			{
				this->hideMouse();
				_isMouseLocked = true;
			}

			for (auto& button : _buttonState)
			{
				if (button.up)
					button.pressed = false;

				button.up = false;
				button.down = false;
			}
		}

		void
		DefaultInputMouse::onReleaseCapture() noexcept
		{
			if (_isMouseLock && _isMouseLocked)
			{
				this->showMouse();
				_isMouseLocked = false;
			}
		}

		void
		DefaultInputMouse::onReset() noexcept
		{
			for (auto& button : _buttonState)
			{
				if (button.up)
					button.pressed = false;

				button.up = false;
				button.down = false;
			}
		}

		void
		DefaultInputMouse::onChangePosition(InputButton::mouse_t x, InputButton::mouse_t y) noexcept
		{
		}

		void
		DefaultInputMouse::onInputEvent(const InputEvent& event) noexcept
		{
			switch (event.event)
			{
			case InputEvent::MouseMotion:
			{
				_mouseX = event.motion.x;
				_mouseY = event.motion.y;
				_mouseAxisX = _mouseX - _lastX;
				_mouseAxisY = _mouseY - _lastY;
				_lastX = _mouseX;
				_lastY = _mouseY;
			}
			break;
			case InputEvent::MouseButtonDown:
			{
				_mouseAxisX = 0;
				_mouseAxisY = 0;
				_lastX = _mouseX = event.button.x;
				_lastY = _mouseY = event.button.y;

				auto& key = this->_buttonState[event.button.button];
				if (!key.pressed)
				{
					key.up = false;
					key.down = true;
					key.pressed = true;
				}
			}
			break;
			case InputEvent::MouseButtonUp:
			{
				_mouseAxisX = 0;
				_mouseAxisY = 0;
				_lastX = _mouseX = event.button.x;
				_lastY = _mouseY = event.button.y;

				auto& key = this->_buttonState[event.button.button];
				key.up = true;
				key.pressed = false;
				key.down = false;
			}
			break;
			case InputEvent::MouseWheelDown:
			{
				_mouseAxisX = 0;
				_mouseAxisY = 0;
				_lastX = _mouseX;
				_lastY = _mouseY;

				auto& key = this->_buttonState[InputButton::MOUSEWHEEL];
				key.up = false;
				key.down = true;
				key.pressed = false;
			}
			break;
			case InputEvent::MouseWheelUp:
			{
				_mouseAxisX = 0;
				_mouseAxisY = 0;
				_lastX = _mouseX;
				_lastY = _mouseY;

				auto& key = this->_buttonState[InputButton::MOUSEWHEEL];
				key.up = true;
				key.pressed = false;
				key.down = false;
			}
			break;
			case InputEvent::MouseButtonDoubleClick:
			{
				auto& key = this->_buttonState[event.button.button];
				key.doubleClick = true;
			}
			break;
			case InputEvent::GetFocus:
				this->obtainCapture();
				break;
			case InputEvent::LostFocus:
				this->releaseCapture();
				break;
			case InputEvent::Reset:
				this->onReset();
				break;
			case InputEvent::SizeChange:
			{
				_centerX = static_cast<InputButton::mouse_t>(event.change.w >> 1);
				_centerY = static_cast<InputButton::mouse_t>(event.change.h >> 1);
			}
			break;
			default:
				break;
			}
		}
	}
}