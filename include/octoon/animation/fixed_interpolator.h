#ifndef OCTOON_ANIMATION_FIXED_INTERPOLATOR_H_
#define OCTOON_ANIMATION_FIXED_INTERPOLATOR_H_

#include <octoon/animation/interpolator.h>

namespace octoon
{
	namespace model
	{
		template<typename T>
		class FixedInterpolator final : public Interpolator<T>
		{
		public:
			FixedInterpolator() noexcept = default;
			FixedInterpolator(T&& value) noexcept : value_(std::move(value)) {};
			FixedInterpolator(const T& value) noexcept : value_(value) {};
			virtual ~FixedInterpolator() noexcept = default;

			virtual T getInterpolation(T t) const noexcept
			{
				return value_;
			}

		private:
			T value_;
		};
	}
}

#endif