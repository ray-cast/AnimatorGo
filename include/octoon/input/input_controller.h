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

			virtual void on_attach() noexcept;
			virtual void on_detach() noexcept;

			virtual void on_input_event(const InputEvent& event) except = 0;

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

			virtual void obtain_capture() noexcept;
			virtual void release_capture() noexcept;
			virtual bool capture() const noexcept;

			virtual void on_frame_begin() noexcept;
			virtual void on_frame_end() noexcept;

			virtual void on_reset() noexcept;

			virtual void on_obtain_capture() noexcept;
			virtual void on_release_capture() noexcept;

		private:
			InputController(const InputController&) noexcept = delete;
			InputController& operator=(const InputController&) noexcept = delete;

		private:
			bool capture_;
		};
	}
}

#endif