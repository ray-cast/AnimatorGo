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
	TimerFeature::startTime() const noexcept
	{
		return Timer::instance()->startTime();
	}

	float TimerFeature::elapsed() const noexcept
	{
		return Timer::instance()->elapsed();
	}

	float TimerFeature::elapsed_max() const noexcept
	{
		return Timer::instance()->elapsed_max();
	}

	float TimerFeature::elapsed_min() const noexcept
	{
		return Timer::instance()->elapsed_min();
	}

	float TimerFeature::fps() const noexcept
	{
		return Timer::instance()->fps();
	}

	float TimerFeature::averageFps() const noexcept
	{
		return Timer::instance()->averageFps();
	}

	float TimerFeature::appTime() const noexcept
	{
		return Timer::instance()->appTime();
	}

	float TimerFeature::frameTime() const noexcept
	{
		return Timer::instance()->frameTime();
	}

	float TimerFeature::delta() const noexcept
	{
		return Timer::instance()->delta();
	}

	void TimerFeature::sleep(float fps) const noexcept
	{
		 Timer::instance()->sleep(fps);
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