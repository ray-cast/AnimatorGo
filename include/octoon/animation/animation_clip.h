#ifndef OCTOON_ANIMATION_CLIP_H_
#define OCTOON_ANIMATION_CLIP_H_

#include <unordered_map>
#include <octoon/animation/animation_curve.h>

namespace octoon
{
	namespace animation
	{
		template<typename _Elem = float, typename _Time = float>
		class AnimationClip final
		{
		public:
			std::string name;
			std::unordered_map<std::string, AnimationCurve<_Elem, _Time>> curves;
			bool finish;
			_Time timeLength;

			AnimationClip() noexcept
				: finish(false)
				, timeLength(0)
			{
			}

			explicit AnimationClip(const std::string& _name) noexcept
				: name(_name)
				, finish(false)
				, timeLength(0)
			{				
			}

			void setName(std::string&& _name) noexcept
			{
				this->name = std::move(_name);
			}

			void setName(const std::string& _name) noexcept
			{
				this->name = _name;
			}

			void setCurve(const char* _name, AnimationCurve<_Elem, _Time>&& curve) noexcept
			{
				this->curves[_name] = std::move(curve);
				timeLength = 0;
				for (auto& it : this->curves)
					timeLength = std::max(it.second.timeLength, timeLength);
			}

			void setCurve(const char* _name, const AnimationCurve<_Elem, _Time>& curve) noexcept
			{
				this->curves[_name] = curve;
				timeLength = 0;
				for (auto& it : this->curves)
					timeLength = std::max(it.second.timeLength, timeLength);
			}

			void setCurve(const std::string& _name, AnimationCurve<_Elem, _Time>&& curve) noexcept
			{
				this->curves[_name] = std::move(curve);
				timeLength = 0;
				for (auto& it : this->curves)
					timeLength = std::max(it.second.timeLength, timeLength);
			}

			void setCurve(const std::string& _name, const AnimationCurve<_Elem, _Time>& curve) noexcept
			{
				this->curves[_name] = curve;
				timeLength = 0;
				for (auto& it : this->curves)
					timeLength = std::max(it.second.timeLength, timeLength);
			}

			AnimationCurve<_Elem, _Time>& getCurve(const char* _name) noexcept
			{
				return this->curves[_name];
			}

			const AnimationCurve<_Elem, _Time>& getCurve(const char* _name) const noexcept
			{
				return this->curves[_name];
			}

			AnimationCurve<_Elem, _Time>& getCurve(const std::string& _name) noexcept
			{
				return this->curves[_name];
			}

			const AnimationCurve<_Elem, _Time>& getCurve(const std::string& _name) const noexcept
			{
				return this->curves[_name];
			}

			bool empty() const noexcept
			{
				return this->curves.empty();
			}

			std::size_t size() const noexcept
			{
				return this->curves.size();
			}

			void evaluate(const _Time& delta) noexcept
			{
				this->finish = true;

				for (auto& it : this->curves)
				{
					it.second.evaluate(delta);
					this->finish &= it.second.finish;
				}
			}

			void setTime(const _Time& time) noexcept
			{
				for (auto& it : this->curves)
					it.second.setTime(time);

				this->finish = true;

				for (auto& it : this->curves)
					this->finish &= it.second.finish;
			}
		};

		template<typename _Elem, typename _Time = float>
		using AnimationClips = std::vector<AnimationClip<_Elem, _Time>>;
	}
}

#endif