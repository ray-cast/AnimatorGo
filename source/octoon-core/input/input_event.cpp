#include <octoon/input/input_event.h>

namespace octoon
{
	namespace input
	{
		void
		InputEvent::makeWindowResize(WindHandle window_, std::uint32_t w, std::uint32_t h, std::uint64_t timestamp) noexcept
		{
			this->event = octoon::input::InputEvent::SizeChange;
			this->change.w = w;
			this->change.h = h;
			this->change.windowID = (std::uint64_t)window_;
			this->change.timestamp = timestamp;
		}

		void
		InputEvent::makeWindowFramebufferResize(WindHandle window_, std::uint32_t w, std::uint32_t h, std::uint64_t timestamp) noexcept
		{
			this->event = octoon::input::InputEvent::SizeChangeDPI;
			this->change.w = w;
			this->change.h = h;
			this->change.windowID = (std::uint64_t)window_;
			this->change.timestamp = timestamp;
		}

		void
		InputEvent::makeWindowClose(WindHandle window_, std::uint64_t timestamp) noexcept
		{
			this->event = octoon::input::InputEvent::AppQuit;
			this->window.windowID = (std::uint64_t)window_;
			this->window.timestamp = timestamp;
		}

		void
		InputEvent::makeWindowFocus(WindHandle window_, bool focus, std::uint64_t timestamp) noexcept
		{
			this->event = focus ? octoon::input::InputEvent::GetFocus : octoon::input::InputEvent::LostFocus;
			this->window.windowID = (std::uint64_t)window_;
			this->window.timestamp = timestamp;
		}

		void
		InputEvent::makeWindowKeyDown(WindHandle window_, InputKey::Code input_key, std::uint16_t scancode, std::uint16_t mods, std::uint64_t timestamp) noexcept
		{
			this->event = octoon::input::InputEvent::KeyDown;
			this->key.windowID = (std::uint64_t)window_;
			this->key.timestamp = timestamp;
			this->key.padding2 = 0;
			this->key.padding3 = 0;
			this->key.repeat = false;
			this->key.state = true;
			this->key.keysym.raw = scancode;
			this->key.keysym.sym = input_key;
			this->key.keysym.mod = mods;
			this->key.keysym.unicode = 0;
		}

		void
		InputEvent::makeWindowKeyUp(WindHandle window_, InputKey::Code input_key, std::uint16_t scancode, std::uint16_t mods, std::uint64_t timestamp) noexcept
		{
			this->event = octoon::input::InputEvent::KeyUp;
			this->key.windowID = (std::uint64_t)window_;
			this->key.timestamp = timestamp;
			this->key.padding2 = 0;
			this->key.padding3 = 0;
			this->key.repeat = false;
			this->key.state = false;
			this->key.keysym.raw = scancode;
			this->key.keysym.sym = input_key;
			this->key.keysym.mod = mods;
			this->key.keysym.unicode = 0;
		}

		void
		InputEvent::makeWindowKeyPress(WindHandle window_, InputKey::Code input_key, std::uint16_t scancode, std::uint16_t mods, std::uint64_t timestamp) noexcept
		{
			this->event = octoon::input::InputEvent::KeyDown;
			this->key.windowID = (std::uint64_t)window_;
			this->key.timestamp = timestamp;
			this->key.padding2 = 0;
			this->key.padding3 = 0;
			this->key.repeat = true;
			this->key.state = false;
			this->key.keysym.raw = scancode;
			this->key.keysym.sym = input_key;
			this->key.keysym.mod = mods;
			this->key.keysym.unicode = 0;
		}

		void
		InputEvent::makeWindowKeyChar(WindHandle window_, std::uint16_t unicode, std::uint16_t mods, std::uint64_t timestamp) noexcept
		{
			this->event = octoon::input::InputEvent::Character;
			this->key.windowID = (std::uint64_t)window_;
			this->key.timestamp = timestamp;
			this->key.padding2 = 0;
			this->key.padding3 = 0;
			this->key.repeat = 0;
			this->key.state = true;
			this->key.keysym.raw = 0;
			this->key.keysym.sym = 0;
			this->key.keysym.mod = mods;
			this->key.keysym.unicode = unicode;
		}

		void
		InputEvent::makeWindowMouseButtonDown(WindHandle window_, InputButton::Code input_button, float x, float y, std::uint64_t timestamp) noexcept
		{
			this->event = octoon::input::InputEvent::MouseButtonDown;
			this->button.button = input_button;
			this->button.clicks = true;
			this->button.x = x;
			this->button.y = y;
			this->button.timestamp = timestamp;
			this->button.windowID = (std::uint64_t)window_;
			this->button.padding1 = 0;
			this->button.which = 0;
		}

		void
		InputEvent::makeWindowMouseButtonUp(WindHandle window_, InputButton::Code input_button, float x, float y, std::uint64_t timestamp) noexcept
		{
			this->event = octoon::input::InputEvent::MouseButtonUp;
			this->button.button = input_button;
			this->button.clicks = false;
			this->button.x = x;
			this->button.y = y;
			this->button.timestamp = timestamp;
			this->button.windowID = (std::uint64_t)window_;
			this->button.padding1 = 0;
			this->button.which = 0;
		}

		void
		InputEvent::makeWindowMouseButtonDoubleClick(WindHandle window_, InputButton::Code input_button, float x, float y, std::uint64_t timestamp) noexcept
		{
			this->event = octoon::input::InputEvent::MouseButtonDoubleClick;
			this->button.button = input_button;
			this->button.clicks = true;
			this->button.x = x;
			this->button.y = y;
			this->button.timestamp = timestamp;
			this->button.windowID = (std::uint64_t)window_;
			this->button.padding1 = 0;
			this->button.which = 0;
		}

		void
		InputEvent::makeWindowMouseMotion(WindHandle window_, float x, float y, std::uint64_t timestamp) noexcept
		{
			this->event = octoon::input::InputEvent::MouseMotion;
			this->motion.x = x;
			this->motion.y = y;
			this->motion.xrel = (std::uint32_t)x;
			this->motion.yrel = (std::uint32_t)y;
			this->motion.timestamp = timestamp;
			this->motion.state = false;
			this->motion.windowID = (std::uint64_t)window_;
		}

		void
		InputEvent::makeWindowScrool(WindHandle window_, float x, float y, std::uint64_t timestamp) noexcept
		{
			this->event = y > 0 ? octoon::input::InputEvent::MouseWheelUp : octoon::input::InputEvent::MouseWheelDown;
			this->wheel.timestamp = timestamp;
			this->wheel.windowID = (std::uint64_t)window_;
		}

		void
		InputEvent::makeWindowDrop(WindHandle window_, std::uint32_t count, const char** file_utf8, std::uint64_t timestamp) noexcept
		{
			this->event = octoon::input::InputEvent::Drop;
			this->drop.timestamp = timestamp;
			this->drop.count = count;
			this->drop.files = file_utf8;
			this->drop.windowID = (std::uint64_t)window_;
		}
	}
}