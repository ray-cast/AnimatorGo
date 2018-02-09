#ifndef OCTOON_INPUT_BUTTON_H_
#define OCTOON_INPUT_BUTTON_H_

#include <octoon/input/input_types.h>

namespace octoon
{
	namespace input
	{
		class InputButton
		{
		public:
			using mouse_t = float;

			enum Code
			{
				LEFT,
				RIGHT,
				MIDDLE,

				MOUSE0 = 0,
				MOUSE1,
				MOUSE2,
				MOUSE3,
				MOUSE4,
				MOUSE5,
				MOUSE6,
				MOUSE7,
				MOUSE8,
				MOUSEWHEEL,
				MOUSEX,
				MOUSEY,
				MOUSEZ,
				MOUSELAST,

				NumButtonCodes,
			};
		};

		class InputAxis
		{
		public:
			enum Code
			{
				MouseX,
				MouseY,

				Horizontal,
				Vertical,

				NumAxisCodes,
			};
		};
	}
}

#endif