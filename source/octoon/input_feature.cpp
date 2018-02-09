#include "input_feature.h"
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

	class InputEventListener : public input::InputListener
	{
	public:
		InputEventListener(InputFeature& input)
			: _input(input)
			, _message(std::make_shared<InputMessage>())
		{
		}

		void onInputEvent(const input::InputEvent& event) noexcept
		{
			_message->setEvent(event);

			//_input.sendMessage(_message);
		}

	private:
		InputEventListener(const InputEventListener&) noexcept = delete;
		InputEventListener& operator=(const InputEventListener&) noexcept = delete;

	private:
		InputFeature& _input;
		std::shared_ptr<InputMessage> _message;
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
		_input = input;
	}

	input::InputPtr
	InputFeature::getInput() const noexcept
	{
		return _input;
	}

	bool
	InputFeature::sendInputEvent(const input::InputEvent& event) noexcept
	{
		assert(_input);
		return _input->sendInputEvent(event);
	}

	bool
	InputFeature::postInputEvent(const input::InputEvent& event) noexcept
	{
		assert(_input);
		return _input->postInputEvent(event);
	}

	void
	InputFeature::onActivate() except
	{
		_input = std::make_shared<input::DefaultInput>();
		if (!_input->open())
			throw std::exception("Input::open() fail.");

		_input->addInputListener(std::make_shared<InputEventListener>(*this));
		_input->setCaptureObject(window_);
		_input->obtainCapture();
	}

	void
	InputFeature::onDeactivate() noexcept
	{
		_input.reset();
	}

	void
	InputFeature::onFrameBegin() noexcept
	{
		assert(_input);
		_input->updateBegin();
		_input->update();
	}

	void
	InputFeature::onFrameEnd() noexcept
	{
		assert(_input);
		_input->updateEnd();
	}

	void
	InputFeature::onReset() noexcept
	{
		assert(_input);
		_input->reset();
	}
}