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
			IInputListener() noexcept;
			virtual ~IInputListener() noexcept;

			virtual void onAttach() noexcept;
			virtual void onDetach() noexcept;

			virtual void onInputEvent(const InputEvent& event) except = 0;

		private:
			IInputListener(const IInputListener&) noexcept = delete;
			IInputListener& operator=(const IInputListener&)noexcept = delete;
		};

		class OCTOON_EXPORT InputController : public IInputListener
		{
			OctoonDeclareSubInterface(InputController, IInputListener)
		public:
			InputController() noexcept;
			virtual ~InputController() noexcept;

			virtual void obtainCapture() noexcept;
			virtual void releaseCapture() noexcept;
			virtual bool capture() const noexcept;

			virtual void onFrameBegin() noexcept;
			virtual void onFrameEnd() noexcept;

			virtual void onReset() noexcept;

			virtual void onObtainCapture() noexcept;
			virtual void onReleaseCapture() noexcept;

		private:
			InputController(const InputController&) noexcept = delete;
			InputController& operator=(const InputController&) noexcept = delete;

		private:

			bool capture_;
		};
	}
}

#endif