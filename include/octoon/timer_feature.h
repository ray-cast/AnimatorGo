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
		float averageFps() const noexcept;

		float appTime() const noexcept;
		float startTime() const noexcept;
		float frameTime() const noexcept;

		float delta() const noexcept;

		float elapsed() const noexcept;
		float elapsedMax() const noexcept;
		float elapsedMin() const noexcept;

		void sleepForFps(float fps) const noexcept;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onFrameBegin() noexcept override;

	private:
		TimerFeature(const TimerFeature&) = delete;
		TimerFeature& operator=(const TimerFeature&) = delete;

	private:
		std::unique_ptr<runtime::Timer> timer_;
	};
}

#endif