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

	void
	InputFeature::onActivate() except
	{
		input_ = std::make_shared<input::DefaultInput>();
		if (!input_->open())
			throw runtime::runtime_error::create("Input::open() fail.");

		input_->add_input_listener(std::make_shared<InputEventListener>(*this));
		input_->set_capture_object(window_);
		input_->obtain_capture();
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
		input_->send_input_event(event);
	}

	void
	InputFeature::onFrameBegin() noexcept
	{
		assert(input_);
		input_->update_begin();
		input_->update();
	}

	void
	InputFeature::onFrameEnd() noexcept
	{
		assert(input_);
		input_->update_end();
	}

	void
	InputFeature::onReset() noexcept
	{
		assert(input_);
		input_->reset();
	}
}
#endif