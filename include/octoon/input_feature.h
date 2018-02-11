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

	private:
		virtual void on_activate() except;
		virtual void on_deactivate() noexcept;

		virtual void on_input_event(const input::InputEvent& event) noexcept;

		virtual void on_reset() noexcept;

		virtual void on_frame_begin() noexcept;
		virtual void on_frame_end() noexcept;

	private:
		InputFeature(const InputFeature&) = delete;
		InputFeature& operator=(const InputFeature&) = delete;

	private:
		input::InputPtr input_;
		input::WindHandle window_;
	};
}

#endif