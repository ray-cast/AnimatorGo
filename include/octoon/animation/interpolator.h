#ifndef OCTOON_ANIMATION_INTERPOLATOR_H_
#define OCTOON_ANIMATION_INTERPOLATOR_H_

namespace octoon
{
	namespace model
	{
		template<typename T>
		class Interpolator
		{
		public:
			Interpolator() = default;
			virtual ~Interpolator() = default;

			virtual T getInterpolation(T t) const noexcept = 0;
		};
	}
}

#endif