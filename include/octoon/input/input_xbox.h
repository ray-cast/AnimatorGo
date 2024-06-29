#ifndef OCTOON_INPUT_XBOX_H_
#define OCTOON_INPUT_XBOX_H_

#include <octoon/input/input_types.h>

namespace octoon
{
	namespace input
	{
		class InputXbox
		{
		public:

			// X360 controller.
			enum Code
			{
				XI_DPADUP,
				XI_DPADDOWN,
				XI_DPADLEFT,
				XI_DPADRIGHT,
				XI_START,
				XI_BACK,
				XI_THUMBL,
				XI_THUMBR,
				XI_SHOULDERL,
				XI_SHOULDERR,
				XI_A,
				XI_B,
				XI_X,
				XI_Y,
				XI_TRIGGERL,
				XI_TRIGGERR,
				XI_THUMBLX,
				XI_THUMBLY,
				XI_THUMBLUP,
				XI_THUMBLDOWN,
				XI_THUMBLLEFT,
				XI_THUMBLRIGHT,
				XI_THUMBRX,
				XI_THUMBRY,
				XI_THUMBRUP,
				XI_THUMBRDOWN,
				XI_THUMBRLEFT,
				XI_THUMBRRIGHT,
				XI_TRIGGERLBTN,
				XI_TRIGGERRBTN,
				XI_CONNECT,
				XI_DISCONNECT,

				NumKeyCodes,
			};
		};
	}
}

#endif