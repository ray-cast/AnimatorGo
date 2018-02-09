#include <octoon/input/input.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(Input, runtime::RttiInterface, "Input")

		InputListener::InputListener() noexcept
		{
		}

		InputListener::~InputListener() noexcept
		{
		}

		void
		InputListener::onAttach() noexcept
		{
		}

		void
		InputListener::onDetach() noexcept
		{
		}
	}
}