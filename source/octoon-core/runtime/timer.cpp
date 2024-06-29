#include <octoon/runtime/timer.h>

#include <cmath>
#include <limits>
#include <thread>

namespace octoon
{
	Timer::Timer() noexcept
		: fps_(0)
		, average_fps_(0)
		, app_time_(0)
		, startTime_(0)
		, last_time_(0)
		, frame_time_(0)
		, accumulate_time_(0)
		, num_frames_(0)
		, current_fps_index_(0)
	{
		this->reset();
	}

	Timer::~Timer() noexcept
	{
	}

	float
	Timer::elapsed() const noexcept
	{
		return float(std::clock()) / CLOCKS_PER_SEC - startTime_;
	}

	float
	Timer::elapsed_max() const noexcept
	{
		return float(std::numeric_limits<std::clock_t>::max()) / float(CLOCKS_PER_SEC) - startTime_;
	}

	float
	Timer::elapsed_min() const noexcept
	{
		return 1.0f / float(CLOCKS_PER_SEC);
	}

	float
	Timer::fps() const noexcept
	{
		return fps_;
	}

	float
	Timer::average_fps() const noexcept
	{
		return average_fps_;
	}

	float
	Timer::delta() const noexcept
	{
		return frame_time_;
	}

	float
	Timer::app_time() const noexcept
	{
		return app_time_;
	}

	float
	Timer::start_time() const noexcept
	{
		return startTime_;
	}

	float
	Timer::frame_time() const noexcept
	{
		return frame_time_;
	}

	void
	Timer::reset() noexcept
	{
		startTime_ = (float)std::clock() / (float)CLOCKS_PER_SEC;
		last_time_ = 0;
	}

	void
	Timer::sleep_for_fps(float fps) const noexcept
	{
		double first = CLOCKS_PER_SEC / fps;
		double second = this->delta() * CLOCKS_PER_SEC;
		if (first > second)
		{
			int sleep = (int)std::round(first - second);
			if (sleep > 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
			}
		}
	}

	void
	Timer::update() noexcept
	{
		frame_time_ = this->elapsed() - last_time_;

		num_frames_++;
		accumulate_time_ += frame_time_;
		app_time_ += frame_time_;

		if (accumulate_time_ > 1.0f)
		{
			fps_ = num_frames_ / accumulate_time_;
			fps_array_[current_fps_index_] = fps_;

			num_frames_ = 0;
			accumulate_time_ -= 1.0f;

			float accumulate_fps_ = 0;

			for (size_t i = 0; i <= current_fps_index_; i++)
				accumulate_fps_ += fps_array_[i];

			if (current_fps_index_ < 9)
			{
				current_fps_index_++;
				average_fps_ = accumulate_fps_ / current_fps_index_;
			}
			else
			{
				current_fps_index_ = 0;
			}
		}

		last_time_ = (float)std::clock() / (float)CLOCKS_PER_SEC - startTime_;
	}
}