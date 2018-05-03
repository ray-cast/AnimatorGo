#ifndef OCTOON_INPUT_TYPES_H_
#define OCTOON_INPUT_TYPES_H_

#include <octoon/runtime/rtti.h>
#include <octoon/runtime/rtti_interface.h>

namespace octoon
{
	namespace input
	{
		typedef std::shared_ptr<class IInputListener> IInputListenerPtr;
		typedef std::shared_ptr<class IInputKeyboard> IInputKeyboardPtr;
		typedef std::shared_ptr<class IInputMouse> IInputMousePtr;
		typedef std::shared_ptr<class IInputDevice> IInputDevicePtr;
		typedef std::shared_ptr<class IInput> IInputPtr;

		typedef std::shared_ptr<class DefaultInput> DefaultInputPtr;
		typedef std::shared_ptr<class InputMap> InputMapPtr;

		typedef void* WindHandle;
	}
}

#endif