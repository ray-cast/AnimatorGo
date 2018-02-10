#ifndef OCTOON_INPUT_CONTROLLER_H_
#define OCTOON_INPUT_CONTROLLER_H_

#include <octoon/input/input_event.h>

namespace octoon
{
	namespace input
	{
		class OCTOON_EXPORT IInputListener : public runtime::RttiInterface
		{
			OctoonDeclareSubInterface(IInputListener, runtime::RttiInterface)
		public:
			IInputListener() noexcept = default;
			virtual ~IInputListener() = default;

			virtual void onAttach() noexcept;
			virtual void onDetach() noexcept;

			virtual void onInputEvent(const InputEvent& event) except = 0;

		private:
			IInputListener(const IInputListener&) noexcept = delete;
			IInputListener& operator=(const IInputListener&)noexcept = delete;
		};
	}
}

#endif