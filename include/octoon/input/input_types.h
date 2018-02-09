#ifndef OCTOON_INPUT_TYPES_H_
#define OCTOON_INPUT_TYPES_H_

#include <octoon/runtime/rtti.h>
#include <octoon/runtime/rtti_interface.h>

namespace octoon
{
	namespace input
	{
		typedef std::shared_ptr<class InputListener> InputListenerPtr;
		typedef std::shared_ptr<class InputKeyboard> InputKeyboardPtr;
		typedef std::shared_ptr<class InputMouse> InputMousePtr;
		typedef std::shared_ptr<class InputDevice> InputDevicePtr;
		typedef std::shared_ptr<class Input> InputPtr;

		typedef void* WindHandle;
	}
}

#endif