#include <octoon/input/input_controller.h>

namespace octoon
{
	namespace input
	{
		OctoonImplementSubInterface(InputController, InputListener, "InputController")

		InputController::InputController() noexcept
			:_capture(true)
		{
		}

		InputController::~InputController() noexcept
		{
		}

		void
		InputController::obtainCapture() noexcept
		{
			if (!_capture)
			{
				this->onObtainCapture();
				_capture = true;
			}
		}

		void
		InputController::releaseCapture() noexcept
		{
			if (_capture)
			{
				this->onReleaseCapture();
				_capture = false;
			}
		}

		bool
		InputController::capture() const noexcept
		{
			return _capture;
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