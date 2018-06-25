#ifndef OCTOON_MODEL_ANIMATION_CURVE_H_
#define OCTOON_MODEL_ANIMATION_CURVE_H_

#include <octoon/model/keyframe.h>

namespace octoon
{
	namespace model
	{
		template<typename _Elem, typename _Time = float>
		class AnimationCurve
		{
		public:
			using Keyframes = std::vector<Keyframe<_Elem, _Time>>;

			Keyframes frames;
			Keyframe<_Elem, _Time> key;

			AnimationCurve() noexcept
			{
			}

			explicit AnimationCurve(Keyframes&& frames_) noexcept
			{
				this->assign(std::move(frames_));
			}

			explicit AnimationCurve(const Keyframes& frames_) noexcept
			{
				this->assign(frames_);
			}

			void assign(Keyframes&& frames_) noexcept
			{
				frames = std::move(frames_);
				std::sort(frames.begin(), frames.end(), [](const Keyframe<_Elem, _Time>& a, const Keyframe<_Elem, _Time>& b) { return a.time < b.time; });
			}

			void assign(const Keyframes& frames_) noexcept
			{
				frames = frames_;
				std::sort(frames.begin(), frames.end(), [](const Keyframe<_Elem, _Time>& a, const Keyframe<_Elem, _Time>& b) { return a.time < b.time; });
			}

			bool empty() const noexcept
			{
				return frames.empty();
			}

			const _Elem& evaluate(const _Time& delta) noexcept
			{
				key.time += delta;

				auto it = std::upper_bound(frames.begin(), frames.end(), key.time, 
					[](const _Time& time, const Keyframe<_Elem, _Time>& a)
					{
						return time < a.time;
					}
				);

				if (it == frames.begin())
					key.value = frames.front().value;
				else if (it == frames.end())
					key.value = frames.back().value;
				else
				{
					auto& a = *it;
					auto& b = *(it - 1);
					key.value = math::lerp(a.value, b.value, 1.0f - (b.time - key.time) / (b.time - a.time));
				}

				return key.value;
			}
		};
	}
}

#endif