#ifndef OCTOON_TIMER_H_
#define OCTOON_TIMER_H_

#include <ctime>
#include <memory>
#include <octoon/runtime/platform.h>

namespace octoon
{
	class OCTOON_EXPORT Timer final
	{
	public:
		Timer() noexcept;
		~Timer() noexcept;

		float fps() const noexcept;
		float average_fps() const noexcept;

		float app_time() const noexcept;
		float start_time() const noexcept;
		float frame_time() const noexcept;

		float delta() const noexcept;

		float elapsed() const noexcept;
		float elapsed_max() const noexcept;
		float elapsed_min() const noexcept;

		void reset() noexcept;

		void sleep_for_fps(float fps) const noexcept;

		void update() noexcept;

	private:
		Timer(const Timer&) = delete;
		Timer& operator=(const Timer&) = delete;

	private:
		float fps_;
		float average_fps_;

		float app_time_;
		float startTime_;
		float last_time_;
		float frame_time_;
		float accumulate_time_;

		std::clock_t num_frames_;
		std::uint8_t current_fps_index_;

		float fps_array_[10];
	};
}

#endif