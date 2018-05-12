#ifndef OCTOON_MATH_TRIANGLE_H_
#define OCTOON_MATH_TRIANGLE_H_

#include <octoon/math/vector3.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T>
			class Triangle
			{
			public:
				Vector3<T> a, b, c;

				Triangle() noexcept = default;
				Triangle(const Vector3<T>& aa, const Vector3<T>& bb, const Vector3<T>& cc) noexcept : a(aa), b(bb), c(cc) {}

				friend bool operator == (const Triangle &a, const Triangle &b) noexcept
				{
					return a.a == b.a && a.b == b.b && a.c == b.c;
				}

				friend bool operator != (const Triangle &a, const Triangle &b) noexcept
				{
					return !(a == b);
				}

				T* ptr() noexcept { return (T*)this; }
				const T* ptr() const noexcept { return (T*)this; }
			};
		}
	}
}

#endif
