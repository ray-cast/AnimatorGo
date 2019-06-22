#ifndef OCTOON_ANIMATION_CLIP_H_
#define OCTOON_ANIMATION_CLIP_H_

#include <unordered_map>
#include <octoon/animation/animation_curve.h>

namespace octoon
{
	namespace animation
	{
		template<typename _Elem, typename _Time = float>
		class AnimationClip final
		{
		public:
			std::string name;
			std::unordered_map<std::string, AnimationCurve<_Elem, _Time>> curves;

			AnimationClip() noexcept
			{
			}

			explicit AnimationClip(const std::string& _name) noexcept
				: name(_name)
			{				
			}

			void setName(std::string&& name_) noexcept
			{
				this->name = std::move(name_);
			}

			void setName(const std::string& name_) noexcept
			{
				this->name = name_;
			}

			void setCurve(const std::string& name, AnimationCurve<_Elem, _Time>&& curve) noexcept
			{
				this->curves[name] = std::move(curve)
			}

			void setCurve(const std::string& name, const AnimationCurve<_Elem, _Time>& curve) noexcept
			{
				this->curves[name] = curve
			}

			AnimationCurve<_Elem, _Time>& getCurve(const char* name_) noexcept
			{
				return this->curves[name_];
			}

			const AnimationCurve<_Elem, _Time>& getCurve(const char* name_) const noexcept
			{
				return this->curves[name_];
			}

			AnimationCurve<_Elem, _Time>& getCurve(const std::string& name_) noexcept
			{
				return this->curves[name_];
			}

			const AnimationCurve<_Elem, _Time>& getCurve(const std::string& name_) const noexcept
			{
				return this->curves[name_];
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