#ifndef OCTOON_ANIMATION_CURVE_H_
#define OCTOON_ANIMATION_CURVE_H_

#include <octoon/animation/keyframe.h>

namespace octoon
{
	namespace animation
	{
		template<typename _Elem = float, typename _Time = float>
		class AnimationCurve final
		{
		public:
			using Keyframes = std::vector<Keyframe<_Elem, _Time>>;

			Keyframes frames;
			Keyframe<_Elem, _Time> key;
			std::shared_ptr<Interpolator<_Time>> interpolator;

			AnimationCurve() noexcept
			{
			}

			explicit AnimationCurve(Keyframes&& frames_, std::shared_ptr<Interpolator<_Time>>&& interpolator_ = nullptr) noexcept
				: interpolator(std::move(interpolator_))
			{
				this->assign(std::move(frames_));
			}

			explicit AnimationCurve(const Keyframes& frames_, const std::shared_ptr<Interpolator<_Time>>& interpolator_ = nullptr) noexcept
				: interpolator(interpolator_)
			{
				this->assign(frames_);
			}

			void assign(Keyframes&& frames_) noexcept
			{
				frames = std::move(frames_);
				std::sort(frames.begin(), frames.end(), [](const Keyframe<_Elem, _Time>& a, const Keyframe<_Elem, _Time>& b) { return a.time < b.time; });
				key = frames.front();
			}

			void assign(const Keyframes& frames_) noexcept
			{
				frames = frames_;
				std::sort(frames.begin(), frames.end(), [](const Keyframe<_Elem, _Time>& a, const Keyframe<_Elem, _Time>& b) { return a.time < b.time; });
				key = frames.front();
			}

			void insert(Keyframe<_Elem, _Time>&& frame_) noexcept
			{
				frames.emplace_back(std::move(frame_));
				std::sort(frames.begin(), frames.end(), [](const Keyframe<_Elem, _Time>& a, const Keyframe<_Elem, _Time>& b) { return a.time < b.time; });
				key = frames.front();
			}

			void insert(const Keyframe<_Elem, _Time>& frame_) noexcept
			{
				frames.emplace_back(frame_);
				std::sort(frames.begin(), frames.end(), [](const Keyframe<_Elem, _Time>& a, const Keyframe<_Elem, _Time>& b) { return a.time < b.time; });
				key = frames.front();
			}

			bool empty() const noexcept
			{
				return frames.empty();
			}

			void setTime(const _Time& time) noexcept
			{
				key.time = time;
				this->evaluate(time);
			}

			const _Elem& evaluate(const _Time& delta) noexcept
			{
				key.time += delta;

				auto it = std::upper_bound(frames.begin(), frames.end(), key.time,
					[](const _Time& time, const Keyframe<_Elem, _Time>& a)
					{
						return time <= a.time;
					}
				);

				if (it == frames.begin())
					key.value = frames.front().value;
				else if (it == frames.end())
					key.value = frames.back().value;
				else
				{
					auto& a = *(it - 1);
					auto& b = *(it);
					auto t = (key.time - a.time) / (b.time - a.time);

					if (a.interpolator)
						t = a.interpolator->interpolator(t);
					else if (interpolator)
						t = interpolator->interpolator(t);

					key.value = a.value * (1.0f - t) + b.value * t;
				}

				return key.value;
			}
		};
	}
}

#endif