#include <octoon/input/iinput_listener.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(IInputListener, runtime::RttiInterface, "IInputListener")

		void
		IInputListener::on_attach() noexcept
		{
		}

		void
		IInputListener::on_detach() noexcept
		{
		}
	}
}