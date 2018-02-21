#include <octoon/timer_feature.h>

namespace octoon
{
	OctoonImplementSubClass(TimerFeature, GameFeature, "TimerFeature")

	TimerFeature::TimerFeature() noexcept
	{
	}

	TimerFeature::~TimerFeature() noexcept
	{
		assert(timer_ == nullptr); // check on_deactivate();
	}

	float
	TimerFeature::fps() const noexcept
	{
		return timer_->fps();
	}

	float
	TimerFeature::average_fps() const noexcept
	{
		return timer_->average_fps();
	}

	float
	TimerFeature::start_time() const noexcept
	{
		return timer_->start_time();
	}

	float
	TimerFeature::app_time() const noexcept
	{
		return timer_->app_time();
	}

	float
	TimerFeature::frame_time() const noexcept
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
	TimerFeature::elapsed_max() const noexcept
	{
		return timer_->elapsed_max();
	}

	float
	TimerFeature::elapsed_min() const noexcept
	{
		return timer_->elapsed_min();
	}

	void
	TimerFeature::sleep_for_fps(float fps) const noexcept
	{
		 timer_->sleep_for_fps(fps);
	}

	void
	TimerFeature::on_activate() except
	{
		timer_ = std::make_shared<runtime::Timer>();
		timer_->reset();
	}

	void
	TimerFeature::on_deactivate() noexcept
	{
		timer_.reset();
	}

	void
	TimerFeature::on_frame_begin() noexcept
	{
		timer_->update();
	}
}