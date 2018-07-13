#ifndef OCTOON_MODEL_INTERPOLATOR_H_
#define OCTOON_MODEL_INTERPOLATOR_H_

#include <octoon/model/modtypes.h>

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