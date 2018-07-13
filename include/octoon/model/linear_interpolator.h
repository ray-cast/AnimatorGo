#ifndef OCTOON_MODEL_LINEAR_INTERPOLATOR_H_
#define OCTOON_MODEL_LINEAR_INTERPOLATOR_H_

#include <octoon/model/interpolator.h>

namespace octoon
{
	namespace model
	{
		template<typename T>
		class LinearInterpolator;

		template<typename _Tx, typename _Ty>
		class LinearInterpolator<_Tx(_Ty)> final : public Interpolator<_Tx(_Ty)>
		{
		public:
			LinearInterpolator() noexcept = default;
			virtual ~LinearInterpolator() noexcept = default;

			virtual _Tx getInterpolation(std::enable_if_t<std::is_same<_Tx, typename std::remove_const<typename std::remove_reference<_Ty>::type>::type>::value, _Ty> t) const noexcept override
			{
				return t;
			}
		};
	}
}

#endif