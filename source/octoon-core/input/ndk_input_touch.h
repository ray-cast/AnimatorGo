#ifndef OCTOON_INPUT_NDK_MOUSE_H_
#define OCTOON_INPUT_NDK_MOUSE_H_

#include <octoon/input/input_mouse_base.h>

namespace octoon
{
	namespace input
	{
		class DLL_EXPORT NDKInputTouch : public InputMouse
		{
			OctoonDeclareSubInterface(NDKInputMouse, InputMouse)
		public:
			NDKInputTouch() noexcept;
			virtual ~NDKInputTouch() noexcept;

			virtual void showMouse() noexcept;
			virtual void hideMouse() noexcept;

			virtual void setPosition(int x, int y) noexcept;

			virtual int getPositionX() const noexcept;
			virtual int getPositionY() const noexcept;
		};
	}
}

#endif