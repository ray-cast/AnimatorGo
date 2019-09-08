#ifndef OCTOON_ANIMATION_H_
#define OCTOON_ANIMATION_H_

#include <octoon/animation/animation_clip.h>

namespace octoon
{
	namespace animation
	{
		struct AnimatorStateInfo
		{
			bool finish;
		};

		template<typename _Elem = float, typename _Time = float>
		class Animation final
		{
		public:			
			std::string name;
			AnimatorStateInfo state;
			AnimationClips<_Elem, _Time> clips;

			Animation() noexcept
				: name("Default")
			{
				state.finish = false;
			}

			Animation(AnimationClip<_Elem, _Time>&& _clip) noexcept
				: Animation()
			{
				clips.emplace_back(std::move(_clip));
			}

			Animation(const AnimationClip<_Elem, _Time>& _clip) noexcept
				: Animation()
			{
				clips.emplace_back(std::move(_clip));
			}

			Animation(AnimationClips<_Elem, _Time>&& _clips) noexcept
				: Animation()
			{
				clips = std::move(_clips);
			}

			Animation(const AnimationClips<_Elem, _Time>& _clips) noexcept
				: Animation()
			{
				clips = _clips;
			}

			Animation(std::string&& _name, AnimationClips<_Elem, _Time>&& _clips) noexcept
				: name(std::move(_name))
				, clips(std::move(_clips))
			{
				state.finish = false;
			}

			Animation(const std::string& _name, const AnimationClips<_Elem, _Time>& _clips) noexcept
				: name(_name)
				, clips(_clips)
			{
				state.finish = false;
			}

			explicit Animation(const std::string& _name) noexcept
				: name(_name)
			{
				state.finish = false
			}

			void setName(std::string&& _name) noexcept
			{
				this->name = std::move(_name);
			}

			void setName(const std::string& _name) noexcept
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

			void setTime(const _Time& time) noexcept
			{
				for (auto& it : this->clips)
					it.setTime(time);
			}

			void evaluate(const _Time& delta) noexcept
			{
				this->state.finish = false;

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