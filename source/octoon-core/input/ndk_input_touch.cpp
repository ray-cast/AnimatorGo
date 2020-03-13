#include "ndk_input_touch.h"

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(NDKInputTouch, InputMouse, "NDKInputTouch")

		NDKInputTouch::NDKInputTouch() noexcept
		{
		}

		NDKInputTouch::~NDKInputTouch() noexcept
		{
		}

		void
		NDKInputTouch::showMouse() noexcept
		{
		}

		void
		NDKInputTouch::hideMouse() noexcept
		{
		}

		void
		NDKInputTouch::setPosition(int x, int y) noexcept
		{
		}

		int
		NDKInputTouch::getPositionX() const noexcept
		{
			return 0;
		}

		int
		NDKInputTouch::getPositionY() const noexcept
		{
			return 0;
		}
	}
}