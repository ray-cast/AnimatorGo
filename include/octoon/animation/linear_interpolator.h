#ifndef OCTOON_LINEAR_INTERPOLATOR_H_
#define OCTOON_LINEAR_INTERPOLATOR_H_

#include <octoon/animation/interpolator.h>

namespace octoon
{
	namespace animation
	{
		template<typename T>
		class LinearInterpolator final : public Interpolator<T(T)>
		{
		public:
			LinearInterpolator() noexcept = default;
			virtual ~LinearInterpolator() noexcept = default;

			virtual T interpolator(T t) const noexcept override
			{
				return t;
			}
		};
	}
}

#endif