#ifndef OCTOON_ANIMATION_H_
#define OCTOON_ANIMATION_H_

#include <octoon/animation/animation_clip.h>

namespace octoon
{
	namespace animation
	{
		template<typename _Elem = float, typename _Time = float>
		class Animation final
		{
		public:
			std::string name;
			std::vector<AnimationClip<_Elem, _Time>> curves;

			Animation() noexcept
			{
			}

			explicit Animation(const std::string& _name) noexcept
				: name(_name)
			{
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
				this->curves.push_back(std::move(clip));
			}

			void addClip(const AnimationClip<_Elem, _Time>& clip) noexcept
			{
				this->curves.push_back(clip);
			}

			void setClip(const std::string& name, const std::vector<AnimationClip<_Elem, _Time>>& clip) noexcept
			{
				this->curves = clip;
			}

			void setClip(const std::string& name, std::vector<AnimationClip<_Elem, _Time>>&& clip) noexcept
			{
				this->curves = std::move(clip);
			}

			void setTime(const _Time& time) noexcept
			{
				for (auto& it : this->curves)
					it.second.setTime(time);
			}

			bool empty() const noexcept
			{
				return this->curves.empty();
			}

			std::size_t size() const noexcept
			{
				return this->curves.size();
			}
		};
	}
}

#endif