#ifndef OCTOON_MODEL_PATH_ITERPOLATOR_H_
#define OCTOON_MODEL_PATH_ITERPOLATOR_H_

#include <octoon/model/interpolator.h>

namespace octoon
{
	namespace model
	{
		template<typename T>
		class PathInterpolator;

		template<typename _Tx, typename _Ty>
		class PathInterpolator<_Tx(_Ty)> final : public Interpolator<_Tx(_Ty)>
		{
		public:
			PathInterpolator() noexcept = default;
			PathInterpolator(_Ty ip[4]) noexcept : xa(ip[0]), xb(ip[1]), ya(ip[2]), yb(ip[3]) {};
			PathInterpolator(_Ty xa_, _Ty xb_, _Ty ya_, _Ty yb_) noexcept : xa(xa_), xb(xb_), ya(ya_), yb(yb_) {};
			virtual ~PathInterpolator() noexcept = default;

			virtual _Tx getInterpolation(_Ty t) const noexcept override
			{
				_Tx min = 0.0f;
				_Tx max = 1.0f;

				_Tx ct = t;

				for (;;)
				{
					_Tx x11 = xa * ct;
					_Tx x12 = xa + (xb - xa) * ct;
					_Tx x13 = xb + (1.0f - xb) * ct;

					_Tx x21 = x11 + (x12 - x11) * ct;
					_Tx x22 = x12 + (x13 - x12) * ct;

					_Tx x3 = x21 + (x22 - x21) * ct;

					if (std::fabs(x3 - t) < 0.0001f)
					{
						_Tx y11 = ya * ct;
						_Tx y12 = ya + (yb - ya) * ct;
						_Tx y13 = yb + (1.0f - yb) * ct;

						_Tx y21 = y11 + (y12 - y11) * ct;
						_Tx y22 = y12 + (y13 - y12) * ct;

						_Tx y3 = y21 + (y22 - y21) * ct;

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

		private:
			_Ty xa, xb, ya, yb;
		};
	}
}

#endif