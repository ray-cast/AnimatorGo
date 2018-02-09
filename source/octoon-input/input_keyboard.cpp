#include <octoon/input/input_keyboard.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(DefaultInputKeyboard, InputKeyboard, "DefaultInputKeyboard")

		DefaultInputKeyboard::DefaultInputKeyboard() noexcept
		{
			std::memset(keyState_, 0, sizeof(keyState_));
		}

		DefaultInputKeyboard::~DefaultInputKeyboard() noexcept
		{
		}

		bool
		DefaultInputKeyboard::isKeyDown(InputKey::Code key) const noexcept
		{
			assert(key < InputKey::Code::NumKeyCodes);
			return keyState_[key].down;
		}

		bool
		DefaultInputKeyboard::isKeyUp(InputKey::Code key) const noexcept
		{
			assert(key < InputKey::Code::NumKeyCodes);
			return keyState_[key].up;
		}

		bool
		DefaultInputKeyboard::isKeyPressed(InputKey::Code key) const noexcept
		{
			assert(key < InputKey::Code::NumKeyCodes);
			return keyState_[key].pressed;
		}

		InputKeyboardPtr
		DefaultInputKeyboard::clone() const noexcept
		{
			return std::make_shared<DefaultInputKeyboard>();
		}

		void
		DefaultInputKeyboard::onFrameEnd() noexcept
		{
			for (std::size_t i = 0; i < InputKey::NumKeyCodes; i++)
			{
				auto& key = keyState_[i];

				if (key.up)
					key.pressed = false;

				key.up = false;
				key.down = false;
			}
		}

		void
		DefaultInputKeyboard::onObtainCapture() noexcept
		{
			for (auto& it : keyState_)
			{
				it.down = false;
				it.pressed = false;
				it.up = true;
			}
		}

		void
		DefaultInputKeyboard::onReset() noexcept
		{
			for (auto& it : keyState_)
			{
				it.down = false;
				it.pressed = false;
				it.up = true;
			}
		}

		void
		DefaultInputKeyboard::onInputEvent(const InputEvent& event) noexcept
		{
			switch (event.event)
			{
			case InputEvent::KeyDown:
			{
				auto& key = this->keyState_[event.key.keysym.sym];
				if (!key.pressed)
				{
					key.down = true;
					key.pressed = true;
				}
			}
			break;
			case InputEvent::KeyUp:
			{
				auto& key = this->keyState_[event.key.keysym.sym];
				key.up = true;
				key.pressed = false;
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
			default:
				break;
			}
		}
	}
}