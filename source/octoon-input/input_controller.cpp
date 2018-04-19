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
		InputController::obtainCapture() noexcept
		{
			if (!capture_)
			{
				this->onObtainCapture();
				capture_ = true;
			}
		}

		void
		InputController::releaseCapture() noexcept
		{
			if (capture_)
			{
				this->onReleaseCapture();
				capture_ = false;
			}
		}

		bool
		InputController::capture() const noexcept
		{
			return capture_;
		}

		void
		InputController::onFrameBegin() noexcept
		{
		}

		void
		InputController::onFrameEnd() noexcept
		{
		}

		void
		InputController::onObtainCapture() noexcept
		{
		}

		void
		InputController::onReleaseCapture() noexcept
		{
		}

		void
		InputController::onReset() noexcept
		{
		}
	}
}