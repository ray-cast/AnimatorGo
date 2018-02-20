#include <octoon/timer_feature.h>
#include <octoon/runtime/timer.h>

using namespace octoon::runtime;

namespace octoon
{
	OctoonImplementSubClass(TimerFeature, GameFeature, "TimerFeature")

	TimerFeature::TimerFeature() noexcept
	{
	}

	TimerFeature::~TimerFeature() noexcept
	{
	}

	float
	TimerFeature::fps() const noexcept
	{
		return Timer::instance()->fps();
	}

	float
	TimerFeature::average_fps() const noexcept
	{
		return Timer::instance()->average_fps();
	}

	float
	TimerFeature::start_time() const noexcept
	{
		return Timer::instance()->start_time();
	}

	float
	TimerFeature::app_time() const noexcept
	{
		return Timer::instance()->app_time();
	}

	float
	TimerFeature::frame_time() const noexcept
	{
		return Timer::instance()->frame_time();
	}

	float
	TimerFeature::delta() const noexcept
	{
		return Timer::instance()->delta();
	}

	float
	TimerFeature::elapsed() const noexcept
	{
		return Timer::instance()->elapsed();
	}

	float
	TimerFeature::elapsed_max() const noexcept
	{
		return Timer::instance()->elapsed_max();
	}

	float
	TimerFeature::elapsed_min() const noexcept
	{
		return Timer::instance()->elapsed_min();
	}

	void
	TimerFeature::sleep_for_fps(float fps) const noexcept
	{
		 Timer::instance()->sleep_for_fps(fps);
	}

	void
	TimerFeature::on_activate() except
	{
		Timer::instance()->reset();
	}

	void
	TimerFeature::on_deactivate() noexcept
	{
	}

	void
	TimerFeature::on_frame_begin() noexcept
	{
		Timer::instance()->update();
	}
}