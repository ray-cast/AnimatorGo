#ifndef OCTOON_INPUT_FEATURE_H_
#define OCTOON_INPUT_FEATURE_H_

#include <octoon/game_feature.h>
#include <octoon/input/input.h>

namespace octoon
{
	class OCTOON_EXPORT InputFeature final : public GameFeature
	{
		OctoonDeclareSubClass(InputFeature, GameFeature)
	public:
		InputFeature() noexcept;
		InputFeature(input::WindHandle hwnd) noexcept;
		virtual ~InputFeature() noexcept;

		void setInput(input::InputPtr input) noexcept;
		input::InputPtr getInput() const noexcept;

	private:
		virtual void onActivate() except;
		virtual void onDeactivate() noexcept;

		virtual void onInputEvent(const input::InputEvent& event) noexcept;

		virtual void onReset() noexcept;

		virtual void onFrameBegin() noexcept;
		virtual void onFrameEnd() noexcept;

	private:
		InputFeature(const InputFeature&) = delete;
		InputFeature& operator=(const InputFeature&) = delete;

	private:
		input::InputPtr input_;
		input::WindHandle window_;
	};
}

#endif