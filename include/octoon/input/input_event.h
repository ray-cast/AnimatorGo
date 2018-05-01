#ifndef OCTOON_INPUT_EVENT_H_
#define OCTOON_INPUT_EVENT_H_

#include <octoon/input/input_key.h>
#include <octoon/input/input_button.h>

namespace octoon
{
	namespace input
	{
		struct InputKeysym
		{
			std::uint16_t sym;
			std::uint16_t raw;
			std::uint16_t mod;
			std::uint16_t unicode;
		};

		struct KeyboardEvent
		{
			std::uint64_t timestamp;
			std::uint64_t windowID;
			std::uint8_t state;
			std::uint8_t repeat;
			std::uint8_t padding2;
			std::uint8_t padding3;
			InputKeysym keysym;
		};

		struct MouseMotionEvent
		{
			std::uint64_t timestamp;
			std::uint64_t windowID;
			std::uint8_t state;
			float x;
			float y;
			std::uint32_t xrel;
			std::uint32_t yrel;
		};

		struct MouseButtonEvent
		{
			std::uint64_t timestamp;
			std::uint64_t windowID;
			std::uint32_t which;
			std::uint8_t button;
			std::uint8_t state;
			std::uint8_t clicks;
			std::uint8_t padding1;
			float x;
			float y;
		};

		struct MouseWheelEvent
		{
			std::uint64_t timestamp;
			std::uint64_t windowID;
		};

		struct SizeChangeEvent
		{
			std::uint64_t timestamp;
			std::uint64_t windowID;
			std::uint32_t w;
			std::uint32_t h;
		};

		struct WindowEvent
		{
			std::uint64_t timestamp;
			std::uint64_t windowID;
		};

		struct DropEvent
		{
			std::uint64_t timestamp;
			std::uint64_t windowID;
			std::uint32_t count;
			const char** files;
		};

		struct JoyAxisEvent {};
		struct JoyBallEvent {};
		struct JoyHatEvent {};
		struct JoyButtonEvent {};
		struct JoyDeviceEvent {};

		class OCTOON_EXPORT InputEvent final
		{
		public:
			enum Type
			{
				KeyDown,
				KeyUp,
				Character,

				MouseMotion,
				MouseButtonDown,
				MouseButtonUp,
				MouseButtonDoubleClick,
				MouseWheelUp,
				MouseWheelDown,

				GamePadButtonDown,
				GamePadButtonUp,

				TouchMotionMove,
				TouchMotionDown,
				TouchMotionUp,
				TouchMotionCancel,

				SizeChange,
				SizeChangeDPI,

				GetFocus,
				LostFocus,

				Drop,

				Reset,

				AppQuit
			};

			Type event;

			union
			{
				KeyboardEvent key;
				MouseMotionEvent motion;
				MouseButtonEvent button;
				MouseWheelEvent wheel;
				JoyAxisEvent jaxis;
				JoyBallEvent jball;
				JoyHatEvent  jhat;
				JoyButtonEvent jbutton;
				JoyDeviceEvent jdevice;
				SizeChangeEvent change;
				WindowEvent window;
				DropEvent drop;
			};

			void makeWindowResize(WindHandle window_, std::uint32_t w, std::uint32_t h, std::uint64_t timestamp) noexcept;
			void makeWindowFramebufferResize(WindHandle window_, std::uint32_t w, std::uint32_t h, std::uint64_t timestamp) noexcept;
			void makeWindowClose(WindHandle window_, std::uint64_t timestamp) noexcept;
			void makeWindowFocus(WindHandle window_, bool focus, std::uint64_t timestamp) noexcept;
			void makeWindowKeyDown(WindHandle window_, std::uint16_t input_key, std::uint16_t scancode, std::uint16_t mods, std::uint64_t timestamp) noexcept;
			void makeWindowKeyUp(WindHandle window_, std::uint16_t input_key, std::uint16_t scancode, std::uint16_t mods, std::uint64_t timestamp) noexcept;
			void makeWindowKeyPress(WindHandle window_, std::uint16_t input_key, std::uint16_t scancode, std::uint16_t mods, std::uint64_t timestamp) noexcept;
			void makeWindowKeyChar(WindHandle window_, std::uint16_t unicode, std::uint16_t mods, std::uint64_t timestamp) noexcept;
			void makeWindowMouseButtonDown(WindHandle window_, std::uint8_t input_button, float x, float y, std::uint64_t timestamp) noexcept;
			void makeWindowMouseButtonUp(WindHandle window_, std::uint8_t input_button, float x, float y, std::uint64_t timestamp) noexcept;
			void makeWindowMouseButtonDoubleClick(WindHandle window_, std::uint8_t input_button, float x, float y, std::uint64_t timestamp) noexcept;
			void makeWindowMouseMotion(WindHandle window_, float x, float y, std::uint64_t timestamp) noexcept;
			void makeWindowScrool(WindHandle window_, float x, float y, std::uint64_t timestamp) noexcept;
			void makeWindowDrop(WindHandle window_, std::uint32_t count, const char** file_utf8, std::uint64_t timestamp) noexcept;
		};
	}
}

#endif