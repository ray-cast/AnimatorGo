#include <octoon/input/input_controller.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(InputController, IInputListener, "InputController")

		InputController::InputController() noexcept
			:capture_(true)
		{
		}

		InputController::~InputController() noexcept
		{
		}

		void
		InputController::obtain_capture() noexcept
		{
			if (!capture_)
			{
				this->on_obtain_capture();
				capture_ = true;
			}
		}

		void
		InputController::release_capture() noexcept
		{
			if (capture_)
			{
				this->on_release_capture();
				capture_ = false;
			}
		}

		bool
		InputController::capture() const noexcept
		{
			return capture_;
		}

		void
		InputController::on_frame_begin() noexcept
		{
		}

		void
		InputController::on_frame_end() noexcept
		{
		}

		void
		InputController::on_obtain_capture() noexcept
		{
		}

		void
		InputController::on_release_capture() noexcept
		{
		}

		void
		InputController::on_reset() noexcept
		{
		}
	}
}