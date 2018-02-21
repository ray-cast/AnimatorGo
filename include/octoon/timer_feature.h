#ifndef OCTOON_TIMER_FEATURE_H_
#define OCTOON_TIMER_FEATURE_H_

#include <octoon/game_feature.h>
#include <octoon/runtime/timer.h>

namespace octoon
{
	class OCTOON_EXPORT TimerFeature final : public GameFeature
	{
		OctoonDeclareSubClass(TimerFeature, GameFeature)
	public:
		TimerFeature() noexcept;
		~TimerFeature() noexcept;

		float fps() const noexcept;
		float average_fps() const noexcept;

		float app_time() const noexcept;
		float start_time() const noexcept;
		float frame_time() const noexcept;

		float delta() const noexcept;

		float elapsed() const noexcept;
		float elapsed_max() const noexcept;
		float elapsed_min() const noexcept;

		void sleep_for_fps(float fps) const noexcept;

	private:
		void on_activate() except override;
		void on_deactivate() noexcept override;

		void on_frame_begin() noexcept override;

	private:
		TimerFeature(const TimerFeature&) = delete;
		TimerFeature& operator=(const TimerFeature&) = delete;

	private:
		std::shared_ptr<runtime::Timer> timer_;
	};
}

#endif