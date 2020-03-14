#ifndef OCTOON_ANIMATION_H_
#define OCTOON_ANIMATION_H_

#include <octoon/animation/animation_clip.h>

namespace octoon
{
	namespace animation
	{
		template<typename _Time = float>
		struct AnimatorStateInfo
		{
			bool finish;
			_Time time;
			_Time timeLength;
		};

		template<typename _Elem = float, typename _Time = float>
		class Animation final
		{
		public:			
			std::string name;
			AnimatorStateInfo<_Time> state;
			AnimationClips<_Elem, _Time> clips;

			Animation() noexcept
				: name("Default")
			{
				state.finish = false;
				state.time = 0;
				state.timeLength = 0;
			}

			Animation(AnimationClip<_Elem, _Time>&& _clip) noexcept
				: Animation()
			{
				clips.emplace_back(std::move(_clip));

				for (auto& clip : clips)
					state.timeLength = std::max(clip.timeLength, state.timeLength);
			}

			Animation(const AnimationClip<_Elem, _Time>& _clip) noexcept
				: Animation()
			{
				clips.emplace_back(std::move(_clip));

				for (auto& clip : clips)
					state.timeLength = std::max(clip.timeLength, state.timeLength);
			}

			Animation(AnimationClips<_Elem, _Time>&& _clips) noexcept
				: Animation()
			{
				clips = std::move(_clips);

				for (auto& clip : clips)
					state.timeLength = std::max(clip.timeLength, state.timeLength);
			}

			Animation(const AnimationClips<_Elem, _Time>& _clips) noexcept
				: Animation()
			{
				clips = _clips;

				for (auto& clip : clips)
					state.timeLength = std::max(clip.timeLength, state.timeLength);
			}

			Animation(std::string&& _name, AnimationClips<_Elem, _Time>&& _clips) noexcept
				: name(std::move(_name))
				, clips(std::move(_clips))
			{
				state.finish = false;
				state.time = 0;

				for (auto& clip : clips)
					state.timeLength = std::max(clip.timeLength, state.timeLength);
			}

			Animation(const std::string& _name, const AnimationClips<_Elem, _Time>& _clips) noexcept
				: name(_name)
				, clips(_clips)
			{
				state.finish = false;
				state.time = 0;

				for (auto& clip : clips)
					state.timeLength = std::max(clip.timeLength, state.timeLength);
			}

			explicit Animation(const std::string& _name) noexcept
				: name(_name)
			{
				state.finish = false
			}

			void setName(std::string_view _name) noexcept
			{
				this->name = name;
			}

			void addClip(AnimationClip<_Elem, _Time>&& clip) noexcept
			{
				this->clips.push_back(std::move(clip));
			}

			void addClip(const AnimationClip<_Elem, _Time>& clip) noexcept
			{
				this->clips.push_back(clip);
			}

			void setClips(const std::vector<AnimationClip<_Elem, _Time>>& clip) noexcept
			{
				this->clips = clip;
			}

			void setClips(std::vector<AnimationClip<_Elem, _Time>>&& clip) noexcept
			{
				this->clips = std::move(clip);
			}

			float timeLength() const noexcept
			{
				return state.timeLength;
			}

			void setTime(const _Time& time) noexcept
			{
				if (this->state.time != time)
				{
					for (auto& it : this->clips)
						it.setTime(time);

					this->state.time = time;
					this->state.finish = true;

					for (auto& it : this->clips)
						this->state.finish &= it.finish;
				}
			}

			_Time getTime() const noexcept
			{
				return this->state.time;
			}

			void evaluate(const _Time& delta) noexcept
			{
				this->state.time += delta;
				this->state.finish = true;

				for (auto& it : this->clips)
				{
					it.evaluate(delta);
					this->state.finish &= it.finish;
				}
			}

			bool empty() const noexcept
			{
				return this->clips.empty();
			}

			std::size_t size() const noexcept
			{
				return this->clips.size();
			}
		};
	}
}

#endif