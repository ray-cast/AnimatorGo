#if defined(OCTOON_FEATURE_TIMER_ENABLE)
#include <octoon/timer_feature.h>

namespace octoon
{
	OctoonImplementSubClass(TimerFeature, GameFeature, "TimerFeature")

	TimerFeature::TimerFeature() noexcept
		: time_(0.0f)
		, timeStep_(0.02f)
		, iterationCounts_(1)
	{
	}

	TimerFeature::~TimerFeature() noexcept
	{
		assert(timer_ == nullptr); // check onDeactivate();
	}

	void
	TimerFeature::setTimeStep(float timeStep) noexcept
	{
		timeStep_ = timeStep;
	}

	float
	TimerFeature::getTimeStep() const noexcept
	{
		return timeStep_;
	}

	void
	TimerFeature::setIterationCounts(std::uint32_t iterationCounts) noexcept
	{
		this->iterationCounts_ = iterationCounts;
	}
	
	std::uint32_t 
	TimerFeature::getIterationCounts() const noexcept
	{
		return this->iterationCounts_;
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
		return timeStep_ / iterationCounts_;
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
		timer_ = std::make_unique<runtime::Timer>();
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

		time_ += timer_->delta();

		while (time_ > timeStep_)
		{
			for (std::uint32_t i = 0; i < iterationCounts_; i++)
				this->sendMessage("feature:timer:fixed", timeStep_ / iterationCounts_);

			time_  -= timeStep_;
		}
	}
}
#endif