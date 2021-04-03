#ifndef OCTOON_ANIMATION_CURVE_H_
#define OCTOON_ANIMATION_CURVE_H_

#include <octoon/animation/keyframe.h>

namespace octoon
{
	enum class AnimationMode
	{
		Once,
		Loop,
		PingPong,
		Default,
		ClampForever
	};

	template<typename _Elem = float, typename _Time = float>
	class AnimationCurve final
	{
	public:
		using Keyframes = std::vector<Keyframe<_Elem, _Time>>;

		bool finish;
		bool negative;
		_Time time;
		_Time timeLength;
		_Elem value;
		Keyframes frames;
		std::shared_ptr<Interpolator<_Time>> interpolator;
		AnimationMode preWrapMode;
		AnimationMode postWrapMode;

		AnimationCurve() noexcept
			: finish(false)
			, negative(false)
			, timeLength(0)
			, preWrapMode(AnimationMode::Default)
			, postWrapMode(AnimationMode::Default)
		{
		}

		explicit AnimationCurve(Keyframes&& frames_, std::shared_ptr<Interpolator<_Time>>&& interpolator_ = nullptr) noexcept
			: interpolator(std::move(interpolator_))
			, finish(false)
			, negative(false)
			, timeLength(0)
			, preWrapMode(AnimationMode::Default)
			, postWrapMode(AnimationMode::Default)
		{
			this->assign(std::move(frames_));
		}

		explicit AnimationCurve(const Keyframes& frames_, const std::shared_ptr<Interpolator<_Time>>& interpolator_ = nullptr) noexcept
			: interpolator(interpolator_)
			, finish(false)
			, timeLength(0)
		{
			this->assign(frames_);
		}

		void assign(Keyframes&& frames_) noexcept
		{
			frames = std::move(frames_);
			this->sort();
			this->time = frames.front().time;
			this->timeLength = frames.back().time;
			this->value = frames.front().value;
		}

		void assign(const Keyframes& frames_) noexcept
		{
			frames = frames_;
			this->sort();
			this->time = frames.front().time;
			this->timeLength = frames.back().time;
			this->value = frames.front().value;
		}

		void insert(Keyframe<_Elem, _Time>&& frame_) noexcept
		{
			frames.emplace_back(std::move(frame_));
			this->sort();
			this->time = frames.front().time;
			this->timeLength = frames.back().time;
			this->value = frames.front().value;
		}

		void insert(const Keyframe<_Elem, _Time>& frame_) noexcept
		{
			frames.emplace_back(frame_);				
			this->sort();
			this->time = frames.front().time;
			this->timeLength = frames.back().time;
			this->value = frames.front().value;
		}

		void sort() noexcept
		{
			std::sort(frames.begin(), frames.end(), [](const Keyframe<_Elem, _Time>& a, const Keyframe<_Elem, _Time>& b) { return a.time < b.time; });
		}

		bool empty() const noexcept
		{
			return frames.empty();
		}

		void setTime(const _Time& _time) noexcept
		{
			this->finish = false;
			this->time = std::clamp(_time, frames.front().time, frames.back().time);
			this->evaluate(0);
		}

		const _Elem& evaluate(const _Time& delta) noexcept
		{
			if (negative)
				this->time -= delta;
			else
				this->time += delta;

			this->time = std::clamp(this->time, frames.front().time, frames.back().time);
			if (frames.size() == 1)
			{
				if (!negative)
					this->updateAnimationMode(this->postWrapMode);
				this->value = frames.back().value;
			}
			else if (this->time <= frames.front().time)
			{
				if (negative)
					this->updateAnimationMode(this->preWrapMode);
				this->value = frames.front().value;
			}
			else if (this->time >= frames.back().time)
			{
				if (!negative)
					this->updateAnimationMode(this->postWrapMode);
				this->value = frames.back().value;
			}
			else
			{
				auto it = std::upper_bound(frames.begin(), frames.end(), this->time,
					[](const _Time& time, const Keyframe<_Elem, _Time>& a)
				{
					return time <= a.time;
				}
				);

				auto& a = *(it - 1);
				auto& b = *(it);
				auto t = (this->time - a.time) / (b.time - a.time);

				if (b.interpolator)
					t = b.interpolator->interpolator(t);
				else if (interpolator)
					t = interpolator->interpolator(t);

				this->finish = false;
				this->value = a.value * (1.0f - t) + b.value * t;
			}

			return this->value;
		}
	private:
		void updateAnimationMode(AnimationMode mode) noexcept
		{
			switch (mode)
			{
			case AnimationMode::Once:
				finish = true;
				break;
			case AnimationMode::Loop:
				this->time = 0.0f;
				break;
			case AnimationMode::PingPong:
				negative = true;
				break;
			case AnimationMode::Default:
				negative = false;
				finish = true;
				break;
			case AnimationMode::ClampForever:
				negative = false;
				break;
			default:
				break;
			}
		}
	};
}

#endif