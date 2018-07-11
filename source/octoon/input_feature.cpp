#if defined(OCTOON_FEATURE_INPUT_ENABLE)
#include <octoon/input_feature.h>
#include <octoon/input/input.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	OctoonImplementSubClass(InputFeature, GameFeature, "InputFeature")

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

	const input::IInputPtr&
	InputFeature::getInput() const noexcept
	{
		return input_;
	}

	void
	InputFeature::onActivate() except
	{
		this->addMessageListener("feature:input:event", std::bind(&InputFeature::onInputEvent, this, std::placeholders::_1));

		input_ = std::make_shared<input::DefaultInput>();
		if (!input_->open())
			throw runtime::runtime_error::create("Input::open() fail.");

		input_->addInputListener(std::make_shared<InputEventListener>(*this));
		input_->setCaptureObject(window_);
		input_->obtainCapture();
	}

	void
	InputFeature::onDeactivate() noexcept
	{
		assert(input_);
		this->removeMessageListener("feature:input:event", std::bind(&InputFeature::onInputEvent, this, std::placeholders::_1));
		input_.reset();
	}

	void
	InputFeature::onInputEvent(const runtime::any& data) noexcept
	{
		assert(input_);
		input_->sendInputEvent(runtime::any_cast<input::InputEvent>(data));
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
#endif