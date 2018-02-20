#ifndef OCTOON_TIMER_H_
#define OCTOON_TIMER_H_

#include <ctime>
#include <memory>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	namespace runtime
	{
		class OCTOON_EXPORT Timer final : public Singleton<Timer>
		{
		public:
			Timer() noexcept;
			~Timer() noexcept;

			float startTime() const noexcept;

			float elapsed() const noexcept;
			float elapsed_max() const noexcept;
			float elapsed_min() const noexcept;

			float fps() const noexcept;
			float averageFps() const noexcept;
			float appTime() const noexcept;
			float frameTime() const noexcept;
			float delta() const noexcept;

			void reset() noexcept;

			void sleep(float fps) const noexcept;

			void update() noexcept;

		private:
			Timer(const Timer&) = delete;
			Timer& operator=(const Timer&) = delete;

		private:
			float fps_;
			float average_fps_;

			float app_time_;
			float start_time_;
			float last_time_;
			float frame_time_;
			float accumulate_time_;
			float accumulate_fps_;

			std::clock_t num_frames_;
			std::uint8_t current_fps_index_;

			float fps_array_[10];
		};

		typedef std::shared_ptr<class Timer> TimerPtr;
	}
}

#endif