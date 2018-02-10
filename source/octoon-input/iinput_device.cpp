#include <octoon/input/iinput_device.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(IInputDevice, runtime::RttiInterface, "IInputDevice")
	}
}