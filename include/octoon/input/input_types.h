#ifndef OCTOON_INPUT_TYPES_H_
#define OCTOON_INPUT_TYPES_H_

#include <octoon/runtime/rtti.h>
#include <octoon/runtime/rtti_interface.h>

namespace octoon
{
	namespace input
	{
		typedef std::shared_ptr<class IInputListener> InputListenerPtr;
		typedef std::shared_ptr<class IInputKeyboard> InputKeyboardPtr;
		typedef std::shared_ptr<class IInputMouse> InputMousePtr;
		typedef std::shared_ptr<class IInputDevice> InputDevicePtr;
		typedef std::shared_ptr<class IInput> InputPtr;

		typedef void* WindHandle;
	}
}

#endif