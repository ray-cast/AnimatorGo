#ifndef OCTOON_MATRIX2X2_H_
#define OCTOON_MATRIX2X2_H_

#include <octoon/math/vector2.h>
#include <octoon/math/quat.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T>
			class Matrix2x2 final
			{
			public:
				typedef typename trait::type_addition<T>::value_type value_type;
				typedef typename trait::type_addition<T>::pointer pointer;
				typedef typename trait::type_addition<T>::const_pointer const_pointer;
				typedef typename trait::type_addition<T>::reference reference;
				typedef typename trait::type_addition<T>::const_reference const_reference;

				union
				{
					struct
					{
						T a1, a2;
						T b1, b2;
					};

					struct
					{
						Vector2<T> right;
						Vector2<T> up;
					};
				};

				Matrix2x2() noexcept = default;
				Matrix2x2(T mt00, T mt01, T mt10, T mt11) noexcept :a1(mt00), a2(mt01), b1(mt10), b2(mt11) {}
				~Matrix2x2() = default;

				Matrix2x2<T>& makeMatrix(T mt00, T mt01, T mt10, T mt11) noexcept
				{
					this->a1 = mt00; this->a2 = mt01;
					this->b1 = mt10; this->b2 = mt11;
					return *this;
				}

				Matrix2x2<T>& makeIdentity() noexcept
				{
					a1 = 1.0f; a2 = 0.0f;
					b1 = 0.0f; b2 = 1.0f;
					return *this;
				}

				Matrix2x2<T>& makeScale(T x, T y) noexcept { return this->makeMatrix(x, 0, 0, y); }
				Matrix2x2<T>& makeScale(const Vector2<T>& sz) noexcept { return this->makeMatrix(sz.x, 0, 0, sz.y); }

				Matrix2x2<T>& makeRotation(T x, T y, T z, T angle) noexcept { return makeRotation(x, y, z, angle); }
				Matrix2x2<T>& makeRotation(const Quaternion<T>& q) noexcept { return makeRotation(q.x, q.y, q.z, q.w); }

				Matrix2x2<T>& makeRotationX(T theta) noexcept
				{
					T ang = theta;
					T c, s;

					sinCos(&s, &c, ang);

					return makeMatrix(1, 0, 0, c);
				}

				Matrix2x2<T>& makeRotationY(T theta) noexcept
				{
					T ang = theta;
					T c, s;

					sinCos(&s, &c, ang);

					return makeMatrix(c, 0, 0, 1);
				}

				Matrix2x2<T>& makeRotationZ(T theta) noexcept
				{
					T ang = theta;
					T c, s;

					sinCos(&s, &c, ang);

					return makeMatrix(c, -s, s, c);
				}

				Matrix2x2<T>& makeRotation(const Vector3<T>& axis, T theta) noexcept
				{
					T c, s;
					sinCos(&s, &c, theta);

					T x = axis.x;
					T y = axis.y;
					T z = axis.z;

					T t = 1 - c;
					T tx = t * x, ty = t * y;

					this->a1 = (tx * x + c);
					this->a2 = (tx * y + s * z);

					this->b1 = (tx * y - s * z);
					this->b2 = (ty * y + c);

					return *this;
				}

				Matrix2x2<T>& multiplyScalar(T x, T y) noexcept
				{
					this->a1 *= x; this->a2 *= x;
					this->b1 *= y; this->b2 *= y;
					return *this;
				}

				pointer ptr() noexcept { return (pointer)this; }
				const_pointer ptr() const noexcept { return (const_pointer)this; }
				pointer data() noexcept { return (pointer)this; }
				const_pointer data() const noexcept { return (const_pointer)this; }

			public:
				friend bool operator==(const Matrix2x2<T>& m1, const Matrix2x2<T>& m2) noexcept
				{
					return
						m1.a1 == m2.a1 && m1.a2 == m2.a2 &&
						m1.b1 == m2.b1 && m1.b2 == m2.b2;
				}

				friend bool operator!=(const Matrix2x2<T>& m1, const Matrix2x2<T>& m2) noexcept
				{
					return
						m1.a1 != m2.a1 || m1.a2 != m2.a2 ||
						m1.b1 != m2.b1 || m1.b2 != m2.b2;
				}
			};
		}
	}
}

#endif