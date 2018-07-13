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

			virtual T getInterpolation() const noexcept = 0;
		};

		template<typename Result, typename ...Argc>
		class Interpolator<Result(Argc...)>
		{
		public:
			Interpolator() = default;
			virtual ~Interpolator() = default;

			virtual Result getInterpolation(Argc... t) const noexcept = 0;
		};
	}
}

#endif