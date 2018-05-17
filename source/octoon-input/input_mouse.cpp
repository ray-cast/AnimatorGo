#include <octoon/input/input_mouse.h>
#include <cstring> // std::memset

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(DefaultInputMouse, IInputMouse, "DefaultInputMouse")

		DefaultInputMouse::DefaultInputMouse() noexcept
			: mouseX_(0)
			, mouseY_(0)
			, mouseAxisX_(0)
			, mouseAxisY_(0)
			, lastX_(0)
			, lastY_(0)
			, centerX_(0)
			, centerY_(0)
			, isMouseLock_(false)
			, isMouseLocked_(false)
			, isMouseHide(false)
			, isMousePositionUpdated_(false)
		{
			std::memset(buttonState_, 0, sizeof(buttonState_));
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
				isMouseLock_ = true;
				isMouseLocked_ = true;
			}
		}

		void
		DefaultInputMouse::unlockMouse() noexcept
		{
			if (isLockedMouse())
			{
				this->showMouse();
				isMouseLock_ = false;
				isMouseLocked_ = false;
			}
		}

		bool
		DefaultInputMouse::isLockedMouse() const noexcept
		{
			return isMouseLocked_;
		}

		void
		DefaultInputMouse::showMouse() noexcept
		{
			if (isMouseHide)
			{
				this->onShowMouse();
				isMouseHide = false;
			}
		}

		void
		DefaultInputMouse::hideMouse() noexcept
		{
			if (!isMouseHide)
			{
				this->onHideMouse();
				isMouseHide = true;
			}
		}

		bool
		DefaultInputMouse::isShowMouse() noexcept
		{
			return !isMouseHide;
		}

		float
		DefaultInputMouse::getAxis(InputAxis::Code axis) const noexcept
		{
			switch (axis)
			{
			case InputAxis::MouseX:
				return mouseAxisX_;
			case InputAxis::MouseY:
				return mouseAxisY_;
			case InputAxis::Horizontal:
			{
				float mouseX = 0.0f, mouseY = 0.0f;
				this->getPosition(mouseX, mouseY);

				return (float)(mouseX - centerX_) / centerX_;
			}
			break;
			case InputAxis::Vertical:
			{
				float mouseX = 0.0f, mouseY = 0.0f;
				this->getPosition(mouseX, mouseY);

				return (float)(mouseY - centerY_) / centerY_;
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
			mouseX_ = x;
			mouseY_ = y;
			this->onChangePosition(x, y);
		}

		void
		DefaultInputMouse::getPosition(InputButton::Type& x, InputButton::Type& y) const noexcept
		{
			x = mouseX_;
			y = mouseY_;
		}

		bool
		DefaultInputMouse::isButtonDown(InputButton::Code key) const noexcept
		{
			return buttonState_[key].down;
		}

		bool
		DefaultInputMouse::isButtonUp(InputButton::Code key) const noexcept
		{
			return buttonState_[key].up;
		}

		bool
		DefaultInputMouse::isButtonPressed(InputButton::Code key) const noexcept
		{
			return buttonState_[key].pressed;
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
				this->setPosition(centerX_, centerY_);

			for (auto& button : buttonState_)
			{
				if (button.up)
					button.pressed = false;

				button.up = false;
				button.down = false;
				button.doubleClick = false;
			}

			if (isMousePositionUpdated_)
			{
				mouseAxisX_ = 0;
				mouseAxisY_ = 0;
				lastX_ = mouseX_;
				lastY_ = mouseY_;

				isMousePositionUpdated_ = false;
			}
		}

		void
		DefaultInputMouse::onObtainCapture() noexcept
		{
			if (isMouseLock_ && !isMouseLocked_)
			{
				this->hideMouse();
				isMouseLocked_ = true;
			}

			for (auto& button : buttonState_)
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
			if (isMouseLock_ && isMouseLocked_)
			{
				this->showMouse();
				isMouseLocked_ = false;
			}
		}

		void
		DefaultInputMouse::onReset() noexcept
		{
			for (auto& button : buttonState_)
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
				mouseX_ = event.motion.x;
				mouseY_ = event.motion.y;
				mouseAxisX_ = mouseX_ - lastX_;
				mouseAxisY_ = mouseY_ - lastY_;
				lastX_ = mouseX_;
				lastY_ = mouseY_;

				isMousePositionUpdated_ = true;
			}
			break;
			case InputEvent::MouseButtonDown:
			{
				mouseAxisX_ = 0;
				mouseAxisY_ = 0;
				lastX_ = mouseX_ = event.button.x;
				lastY_ = mouseY_ = event.button.y;

				auto& key = this->buttonState_[event.button.button];
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
				mouseAxisX_ = 0;
				mouseAxisY_ = 0;
				lastX_ = mouseX_ = event.button.x;
				lastY_ = mouseY_ = event.button.y;

				auto& key = this->buttonState_[event.button.button];
				key.up = true;
				key.pressed = false;
				key.down = false;
			}
			break;
			case InputEvent::MouseWheelDown:
			{
				mouseAxisX_ = 0;
				mouseAxisY_ = 0;
				lastX_ = mouseX_;
				lastY_ = mouseY_;

				auto& key = this->buttonState_[InputButton::MouseWheel];
				key.up = false;
				key.down = true;
				key.pressed = false;
			}
			break;
			case InputEvent::MouseWheelUp:
			{
				mouseAxisX_ = 0;
				mouseAxisY_ = 0;
				lastX_ = mouseX_;
				lastY_ = mouseY_;

				auto& key = this->buttonState_[InputButton::MouseWheel];
				key.up = true;
				key.pressed = false;
				key.down = false;
			}
			break;
			case InputEvent::MouseButtonDoubleClick:
			{
				auto& key = this->buttonState_[event.button.button];
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
				centerX_ = static_cast<InputButton::Type>(event.change.w >> 1);
				centerY_ = static_cast<InputButton::Type>(event.change.h >> 1);
			}
			break;
			default:
				break;
			}
		}
	}
}