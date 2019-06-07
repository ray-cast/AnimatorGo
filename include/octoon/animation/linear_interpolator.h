#ifndef OCTOON_MODEL_LINEAR_INTERPOLATOR_H_
#define OCTOON_MODEL_LINEAR_INTERPOLATOR_H_

#include <octoon/animation/interpolator.h>

namespace octoon
{
	namespace model
	{
		template<typename T>
		class LinearInterpolator final : public Interpolator<T(T)>
		{
		public:
			LinearInterpolator() noexcept = default;
			virtual ~LinearInterpolator() noexcept = default;

			virtual T getInterpolation(T t) const noexcept override
			{
				return t;
			}
		};
	}
}

#endif