#include <octoon/input/input_mouse.h>
#include <cstring> // std::memset

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(DefaultInputMouse, IInputMouse, "DefaultInputMouse")

		DefaultInputMouse::DefaultInputMouse() noexcept
			: mouse_x_(0)
			, mouse_y_(0)
			, mouse_axis_x_(0)
			, mouse_axis_y_(0)
			, last_x_(0)
			, last_y_(0)
			, center_x_(0)
			, center_y_(0)
			, is_mouse_lock_(false)
			, is_mouse_locked_(false)
			, is_mouse_hide(false)
			, is_mouse_position_updated_(false)
		{
			std::memset(button_state_, 0, sizeof(button_state_));
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
				is_mouse_lock_ = true;
				is_mouse_locked_ = true;
			}
		}

		void
		DefaultInputMouse::unlockMouse() noexcept
		{
			if (isLockedMouse())
			{
				this->showMouse();
				is_mouse_lock_ = false;
				is_mouse_locked_ = false;
			}
		}

		bool
		DefaultInputMouse::isLockedMouse() const noexcept
		{
			return is_mouse_locked_;
		}

		void
		DefaultInputMouse::showMouse() noexcept
		{
			if (is_mouse_hide)
			{
				this->onShowMouse();
				is_mouse_hide = false;
			}
		}

		void
		DefaultInputMouse::hideMouse() noexcept
		{
			if (!is_mouse_hide)
			{
				this->onHideMouse();
				is_mouse_hide = true;
			}
		}

		bool
		DefaultInputMouse::isShowMouse() noexcept
		{
			return !is_mouse_hide;
		}

		float
		DefaultInputMouse::getAxis(InputAxis::Code axis) const noexcept
		{
			switch (axis)
			{
			case InputAxis::MouseX:
				return mouse_axis_x_;
			case InputAxis::MouseY:
				return mouse_axis_y_;
			case InputAxis::Horizontal:
			{
				float mouseX = 0.0f, mouseY = 0.0f;
				this->getPosition(mouseX, mouseY);

				return (float)(mouseX - center_x_) / center_x_;
			}
			break;
			case InputAxis::Vertical:
			{
				float mouseX = 0.0f, mouseY = 0.0f;
				this->getPosition(mouseX, mouseY);

				return (float)(mouseY - center_y_) / center_y_;
			}
			break;
			default:
				assert(false);
				return 0.0;
			}
		}

		void
		DefaultInputMouse::setPosition(InputButton::Type x, InputButton::Type y) noexcept
		{
			mouse_x_ = x;
			mouse_y_ = y;
			this->onChangePosition(x, y);
		}

		void
		DefaultInputMouse::getPosition(InputButton::Type& x, InputButton::Type& y) const noexcept
		{
			x = mouse_x_;
			y = mouse_y_;
		}

		bool
		DefaultInputMouse::isButtonDown(InputButton::Code key) const noexcept
		{
			return button_state_[key].down;
		}

		bool
		DefaultInputMouse::isButtonUp(InputButton::Code key) const noexcept
		{
			return button_state_[key].up;
		}

		bool
		DefaultInputMouse::isButtonPressed(InputButton::Code key) const noexcept
		{
			return button_state_[key].pressed;
		}

		IInputMousePtr
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
				this->setPosition(center_x_, center_y_);

			for (auto& button : button_state_)
			{
				if (button.up)
					button.pressed = false;

				button.up = false;
				button.down = false;
				button.doubleClick = false;
			}

			if (is_mouse_position_updated_)
			{
				mouse_axis_x_ = 0;
				mouse_axis_y_ = 0;
				last_x_ = mouse_x_;
				last_y_ = mouse_y_;

				is_mouse_position_updated_ = false;
			}
		}

		void
		DefaultInputMouse::onObtainCapture() noexcept
		{
			if (is_mouse_lock_ && !is_mouse_locked_)
			{
				this->hideMouse();
				is_mouse_locked_ = true;
			}

			for (auto& button : button_state_)
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
			if (is_mouse_lock_ && is_mouse_locked_)
			{
				this->showMouse();
				is_mouse_locked_ = false;
			}
		}

		void
		DefaultInputMouse::onReset() noexcept
		{
			for (auto& button : button_state_)
			{
				if (button.up)
					button.pressed = false;

				button.up = false;
				button.down = false;
			}
		}

		void
		DefaultInputMouse::onChangePosition(InputButton::Type x, InputButton::Type y) noexcept
		{
		}

		void
		DefaultInputMouse::onInputEvent(const InputEvent& event) noexcept
		{
			switch (event.event)
			{
			case InputEvent::MouseMotion:
			{
				mouse_x_ = event.motion.x;
				mouse_y_ = event.motion.y;
				mouse_axis_x_ = mouse_x_ - last_x_;
				mouse_axis_y_ = mouse_y_ - last_y_;
				last_x_ = mouse_x_;
				last_y_ = mouse_y_;

				is_mouse_position_updated_ = true;
			}
			break;
			case InputEvent::MouseButtonDown:
			{
				mouse_axis_x_ = 0;
				mouse_axis_y_ = 0;
				last_x_ = mouse_x_ = event.button.x;
				last_y_ = mouse_y_ = event.button.y;

				auto& key = this->button_state_[event.button.button];
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
				mouse_axis_x_ = 0;
				mouse_axis_y_ = 0;
				last_x_ = mouse_x_ = event.button.x;
				last_y_ = mouse_y_ = event.button.y;

				auto& key = this->button_state_[event.button.button];
				key.up = true;
				key.pressed = false;
				key.down = false;
			}
			break;
			case InputEvent::MouseWheelDown:
			{
				mouse_axis_x_ = 0;
				mouse_axis_y_ = 0;
				last_x_ = mouse_x_;
				last_y_ = mouse_y_;

				auto& key = this->button_state_[InputButton::MouseWheel];
				key.up = false;
				key.down = true;
				key.pressed = false;
			}
			break;
			case InputEvent::MouseWheelUp:
			{
				mouse_axis_x_ = 0;
				mouse_axis_y_ = 0;
				last_x_ = mouse_x_;
				last_y_ = mouse_y_;

				auto& key = this->button_state_[InputButton::MouseWheel];
				key.up = true;
				key.pressed = false;
				key.down = false;
			}
			break;
			case InputEvent::MouseButtonDoubleClick:
			{
				auto& key = this->button_state_[event.button.button];
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
				center_x_ = static_cast<InputButton::Type>(event.change.w >> 1);
				center_y_ = static_cast<InputButton::Type>(event.change.h >> 1);
			}
			break;
			default:
				break;
			}
		}
	}
}