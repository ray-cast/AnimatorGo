#ifndef OCTOON_MODEL_KEYFRAME_H_
#define OCTOON_MODEL_KEYFRAME_H_

#include <octoon/math/math.h>

namespace octoon
{
	namespace model
	{
		template<typename _Elem, typename _Time = float>
		class Keyframe final
		{
		public:
			_Time time;
			_Elem value;

			Keyframe() noexcept
				: time()
				, value()
			{
			}

			Keyframe(const _Time& time_, const _Elem& value_) noexcept
				: time(time_)
				, value(value_)
			{
			}

			Keyframe(_Time&& time_, _Elem&& value_) noexcept
				: time(std::move(time_))
				, value(std::move(value_))
			{
			}
		};

		template<typename _Elem, typename _Time = float>
		using Keyframes = std::vector<Keyframe<_Elem, _Time>>;
	}
}

#endif