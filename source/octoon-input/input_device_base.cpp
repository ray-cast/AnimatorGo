#include <octoon/input/input_device_base.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(InputDevice, runtime::RttiInterface, "InputDevice")
		OctoonImplementSubInterface(InputListener, runtime::RttiInterface, "InputListener")

		InputDevice::InputDevice() noexcept
		{
		}

		InputDevice::~InputDevice() noexcept
		{
		}
	}
}