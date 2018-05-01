#include <octoon/input/iinput_listener.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(IInputListener, runtime::RttiInterface, "IInputListener")

		IInputListener::IInputListener() noexcept
		{
		}

		IInputListener::~IInputListener()
		{
		}

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