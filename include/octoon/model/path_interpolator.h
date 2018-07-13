#ifndef OCTOON_MODEL_PATH_ITERPOLATOR_H_
#define OCTOON_MODEL_PATH_ITERPOLATOR_H_

#include <octoon/model/interpolator.h>

namespace octoon
{
	namespace model
	{
		template<typename T>
		class PathInterpolator final : public Interpolator<T>
		{
		public:
			PathInterpolator() noexcept = default;
			PathInterpolator(T ip[4]) noexcept : xa(ip[0]), xb(ip[1]), ya(ip[2]), yb(ip[3]) {};
			PathInterpolator(T xa_, T xb_, T ya_, T yb_) noexcept : xa(xa_), xb(xb_), ya(ya_), yb(yb_) {};
			virtual ~PathInterpolator() noexcept = default;

			virtual T getInterpolation(T t) const noexcept override
			{
				T min = 0.0f;
				T max = 1.0f;

				T ct = t;

				for (;;)
				{
					T x11 = xa * ct;
					T x12 = xa + (xb - xa) * ct;
					T x13 = xb + (1.0f - xb) * ct;

					T x21 = x11 + (x12 - x11) * ct;
					T x22 = x12 + (x13 - x12) * ct;

					T x3 = x21 + (x22 - x21) * ct;

					if (std::fabs(x3 - t) < 0.0001f)
					{
						T y11 = ya * ct;
						T y12 = ya + (yb - ya) * ct;
						T y13 = yb + (1.0f - yb) * ct;

						T y21 = y11 + (y12 - y11) * ct;
						T y22 = y12 + (y13 - y12) * ct;

						T y3 = y21 + (y22 - y21) * ct;

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
			T xa, xb, ya, yb;
		};
	}
}

#endif