#include <octoon/input/iinput_listener.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(IInputListener, Object, "IInputListener")

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