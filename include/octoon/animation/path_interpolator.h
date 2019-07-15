#ifndef OCTOON_PATH_ITERPOLATOR_H_
#define OCTOON_PATH_ITERPOLATOR_H_

#include <octoon/animation/interpolator.h>

namespace octoon
{
	namespace animation
	{
		template<typename T = float>
		class PathInterpolator final : public Interpolator<T>
		{
		public:
			PathInterpolator() noexcept = default;
			PathInterpolator(T ip[4]) noexcept : xa(ip[0]), xb(ip[1]), ya(ip[2]), yb(ip[3]) {};
			PathInterpolator(T xa_, T xb_, T ya_, T yb_) noexcept : xa(xa_), xb(xb_), ya(ya_), yb(yb_) {};
			virtual ~PathInterpolator() noexcept = default;

			T evalX(T t) const noexcept
			{
				T x11 = xa * t;
				T x12 = xa + (xb - xa) * t;
				T x13 = xb + (1.0f - xb) * t;

				T x21 = x11 + (x12 - x11) * t;
				T x22 = x12 + (x13 - x12) * t;

				return x21 + (x22 - x21) * t;
			}

			T evalY(T t) const noexcept
			{
				T y11 = ya * t;
				T y12 = ya + (yb - ya) * t;
				T y13 = yb + (1.0f - yb) * t;

				T y21 = y11 + (y12 - y11) * t;
				T y22 = y12 + (y13 - y12) * t;

				return y21 + (y22 - y21) * t;
			}

			virtual T interpolator(T time) const noexcept override
			{
				T min = 0.0f;
				T max = 1.0f;

				T t = 0.5f;
				T x = this->evalX(t);

				while (std::fabs(x - time) > math::EPSILON_E4)
				{
					if (x < time)
						min = t;
					else
						max = t;

					t = (min + max) * 0.5f;
					x = evalX(t);
				}

				return this->evalY(t);
			}

		private:
			T xa, xb, ya, yb;
		};
	}
}

#endif