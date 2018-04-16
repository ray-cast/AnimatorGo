#if defined(OCTOON_FEATURE_TIMER_ENABLE)
#include <octoon/timer_feature.h>

namespace octoon
{
	OctoonImplementSubClass(TimerFeature, GameFeature, "TimerFeature")

	TimerFeature::TimerFeature() noexcept
	{
	}

	TimerFeature::~TimerFeature() noexcept
	{
		assert(timer_ == nullptr); // check onDeactivate();
	}

	float
	TimerFeature::fps() const noexcept
	{
		return timer_->fps();
	}

	float
	TimerFeature::averageFps() const noexcept
	{
		return timer_->average_fps();
	}

	float
	TimerFeature::startTime() const noexcept
	{
		return timer_->start_time();
	}

	float
	TimerFeature::appTime() const noexcept
	{
		return timer_->app_time();
	}

	float
	TimerFeature::frameTime() const noexcept
	{
		return timer_->frame_time();
	}

	float
	TimerFeature::delta() const noexcept
	{
		return timer_->delta();
	}

	float
	TimerFeature::elapsed() const noexcept
	{
		return timer_->elapsed();
	}

	float
	TimerFeature::elapsedMax() const noexcept
	{
		return timer_->elapsed_max();
	}

	float
	TimerFeature::elapsedMin() const noexcept
	{
		return timer_->elapsed_min();
	}

	void
	TimerFeature::sleepForFps(float fps) const noexcept
	{
		 timer_->sleep_for_fps(fps);
	}

	void
	TimerFeature::onActivate() except
	{
		timer_ = std::make_shared<runtime::Timer>();
		timer_->reset();
	}

	void
	TimerFeature::onDeactivate() noexcept
	{
		timer_.reset();
	}

	void
	TimerFeature::onFrameBegin() noexcept
	{
		timer_->update();
	}
}
#endif