#ifndef OCTOON_INPUT_FEATURES_H_
#define OCTOON_INPUT_FEATURES_H_

#include <octoon/game_features.h>
#include <octoon/input/input.h>

namespace octoon
{
	class OCTOON_EXPORT InputMessage final : public runtime::RttiInterface
	{
		OctoonDeclareSubClass(InputMessage, runtime::RttiInterface)
	public:
		InputMessage() noexcept;
		~InputMessage() noexcept;

		void setEvent(const input::InputEvent& event) noexcept;
		const input::InputEvent& getEvent() const noexcept;

	private:
		input::InputEvent _event;
	};

	class OCTOON_EXPORT InputFeature final : public GameFeature
	{
		OctoonDeclareSubClass(InputFeature, GameFeature)
	public:
		InputFeature() noexcept;
		InputFeature(input::WindHandle hwnd) noexcept;
		virtual ~InputFeature() noexcept;

		void setInput(input::InputPtr input) noexcept;
		input::InputPtr getInput() const noexcept;

		bool sendInputEvent(const input::InputEvent& event) noexcept;
		bool postInputEvent(const input::InputEvent& event) noexcept;

	private:
		virtual void onActivate() except;
		virtual void onDeactivate() noexcept;

		virtual void onReset() noexcept;

		virtual void onFrameBegin() noexcept;
		virtual void onFrameEnd() noexcept;

	private:
		InputFeature(const InputFeature&) = delete;
		InputFeature& operator=(const InputFeature&) = delete;

	private:
		input::InputPtr _input;
		input::WindHandle window_;
	};
}

#endif