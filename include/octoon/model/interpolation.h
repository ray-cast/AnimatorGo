#ifndef OCTOON_MODEL_ITERPOLATION_H_
#define OCTOON_MODEL_ITERPOLATION_H_

#include <octoon/model/modtypes.h>

namespace octoon
{
	namespace model
	{
		namespace detail
		{
			template<typename T>
			class Interpolation
			{
			public:
				union
				{
					T interps[16];

					struct
					{
						T interpX[4];
						T interpY[4];
						T interpZ[4];
						T interpRotation[4];
					};
				};
			};
		}

		template<typename T, typename _Float = float>
		static _Float bezierEval(const T ip[4], _Float t) noexcept
		{
			_Float xa = static_cast<_Float>(ip[0]) / std::numeric_limits<T>::max();
			_Float xb = static_cast<_Float>(ip[2]) / std::numeric_limits<T>::max();
			_Float ya = static_cast<_Float>(ip[1]) / std::numeric_limits<T>::max();
			_Float yb = static_cast<_Float>(ip[3]) / std::numeric_limits<T>::max();

			_Float min = 0.0f;
			_Float max = 1.0f;

			_Float ct = t;
			for (;;)
			{
				_Float x11 = xa * ct;
				_Float x12 = xa + (xb - xa) * ct;
				_Float x13 = xb + (1.0f - xb) * ct;

				_Float x21 = x11 + (x12 - x11) * ct;
				_Float x22 = x12 + (x13 - x12) * ct;

				_Float x3 = x21 + (x22 - x21) * ct;

				if (std::fabs(x3 - t) < 0.0001f)
				{
					_Float y11 = ya * ct;
					_Float y12 = ya + (yb - ya) * ct;
					_Float y13 = yb + (1.0f - yb) * ct;

					_Float y21 = y11 + (y12 - y11) * ct;
					_Float y22 = y12 + (y13 - y12) * ct;

					_Float y3 = y21 + (y22 - y21) * ct;

					return y3;
				}
				else if (x3 < t)
				{
					min = ct;
				}
				else
				{
					max = ct;
				}

				ct = min * 0.5f + max * 0.5f;
			}
		}
	}
}

#endif