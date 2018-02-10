#include <octoon/input_feature.h>
#include <octoon/input/input.h>

namespace octoon
{
	OctoonImplementSubClass(InputMessage, runtime::RttiInterface, "InputMessage")
	OctoonImplementSubClass(InputFeature, GameFeature, "InputFeature")

	InputMessage::InputMessage() noexcept
	{
	}

	InputMessage::~InputMessage() noexcept
	{
	}

	void
	InputMessage::setEvent(const input::InputEvent& event) noexcept
	{
		_event = event;
	}

	const input::InputEvent&
	InputMessage::getEvent() const noexcept
	{
		return _event;
	}

	class InputEventListener : public input::IInputListener
	{
	public:
		InputEventListener(InputFeature& input)
			: input_(input)
		{
		}

		void onInputEvent(const input::InputEvent& event) noexcept
		{
		}

	private:
		InputEventListener(const InputEventListener&) noexcept = delete;
		InputEventListener& operator=(const InputEventListener&) noexcept = delete;

	private:
		InputFeature& input_;
	};

	InputFeature::InputFeature() noexcept
		: window_(0)
	{
	}

	InputFeature::InputFeature(input::WindHandle hwnd) noexcept
		: window_(hwnd)
	{
	}

	InputFeature::~InputFeature() noexcept
	{
	}

	void
	InputFeature::setInput(input::InputPtr input) noexcept
	{
		assert(input);
		input_ = input;
	}

	input::InputPtr
	InputFeature::getInput() const noexcept
	{
		return input_;
	}

	void
	InputFeature::onActivate() except
	{
		input_ = std::make_shared<input::DefaultInput>();
		if (!input_->open())
			throw std::exception("Input::open() fail.");

		input_->addInputListener(std::make_shared<InputEventListener>(*this));
		input_->setCaptureObject(window_);
		input_->obtainCapture();
	}

	void
	InputFeature::onDeactivate() noexcept
	{
		assert(input_);
		input_.reset();
	}

	void
	InputFeature::onInputEvent(const input::InputEvent& event) noexcept
	{
		assert(input_);
		input_->sendInputEvent(event);
	}

	void
	InputFeature::onFrameBegin() noexcept
	{
		assert(input_);
		input_->updateBegin();
		input_->update();
	}

	void
	InputFeature::onFrameEnd() noexcept
	{
		assert(input_);
		input_->updateEnd();
	}

	void
	InputFeature::onReset() noexcept
	{
		assert(input_);
		input_->reset();
	}
}