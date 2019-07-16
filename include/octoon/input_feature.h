#ifndef OCTOON_INPUT_FEATURE_H_
#define OCTOON_INPUT_FEATURE_H_

#include <octoon/game_feature.h>
#include <octoon/input/iinput.h>

namespace octoon
{
	class OCTOON_EXPORT InputFeature final : public GameFeature
	{
		OctoonDeclareSubClass(InputFeature, GameFeature)
	public:
		InputFeature() noexcept;
		InputFeature(input::WindHandle hwnd) noexcept;
		virtual ~InputFeature() noexcept;

		const input::IInputPtr& getInput() const noexcept;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onInputEvent(const runtime::any& data) noexcept;

		void onReset() noexcept override;

		void onFrameBegin() noexcept override;
		void onFrameEnd() noexcept override;

	private:
		InputFeature(const InputFeature&) = delete;
		InputFeature& operator=(const InputFeature&) = delete;

	private:
		input::IInputPtr input_;
		input::WindHandle window_;
	};
}

#endif