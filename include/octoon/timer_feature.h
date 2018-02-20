#ifndef OCTOON_TIMER_FEATURE_H_
#define OCTOON_TIMER_FEATURE_H_

#include <octoon/game_feature.h>

namespace octoon
{
	class OCTOON_EXPORT TimerFeature final : public GameFeature
	{
		OctoonDeclareSubClass(TimerFeature, GameFeature)
	public:
		TimerFeature() noexcept;
		~TimerFeature() noexcept;

		float startTime() const noexcept;

		float elapsed() const noexcept;
		float elapsed_max() const noexcept;
		float elapsed_min() const noexcept;

		float fps() const noexcept;
		float averageFps() const noexcept;
		float appTime() const noexcept;
		float frameTime() const noexcept;
		float delta() const noexcept;

		void sleep(float fps) const noexcept;

	private:
		void on_activate() except;
		void on_deactivate() noexcept;

		void on_frame_begin() noexcept;

	private:
		TimerFeature(const TimerFeature&) = delete;
		TimerFeature& operator=(const TimerFeature&) = delete;
	};
}

#endif