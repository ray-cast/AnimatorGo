#ifndef OCTOON_INPUT_PS3_H_
#define OCTOON_INPUT_PS3_H_

#include <octoon/input/input_types.h>

namespace octoon
{
	namespace input
	{
		class InputPS3
		{
		public:
			enum Code
			{
				// PS3 controller.
				PS3_SELECT,
				PS3_L3,
				PS3_R3,
				PS3_START,
				PS3_UP,
				PS3_RIGHT,
				PS3_DOWN,
				PS3_LEFT,
				PS3_L2,
				PS3_R2,
				PS3_L1,
				PS3_R1,
				PS3_TRIANGLE,
				PS3_CIRCLE,
				PS3_CROSS,
				PS3_SQUARE,
				PS3_STICKLX,
				PS3_STICKLY,
				PS3_STICKRX,
				PS3_STICKRY,
				PS3_ROTX,
				PS3_ROTY,
				PS3_ROTZ,
				PS3_ROTX_KEYL,
				PS3_ROTX_KEYR,
				PS3_ROTZ_KEYD,
				PS3_ROTZ_KEYU,

				NumKeyCodes,
			};
		};
	}
}

#endif