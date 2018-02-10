#include <octoon/input/iinput_listener.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(IInputListener, runtime::RttiInterface, "IInputListener")

		void
		IInputListener::onAttach() noexcept
		{
		}

		void
		IInputListener::onDetach() noexcept
		{
		}
	}
}