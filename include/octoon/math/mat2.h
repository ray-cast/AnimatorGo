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

				Matrix2x2() noexcept = default;
				Matrix2x2(T mt00, T mt01, T mt10, T mt11) noexcept :a1(mt00), a2(mt01), b1(mt10), b2(mt11) {}
				~Matrix2x2() = default;

				Matrix2x2& set(T mt00, T mt01, T mt10, T mt11) noexcept
				{
					this->a1 = mt00; this->a2 = mt01;
					this->b1 = mt10; this->b2 = mt11;
					return *this;
				}

				void scale(T x, T y) noexcept
				{
					this->a1 *= x; this->a2 *= x;
					this->b1 *= y; this->b2 *= y;
				}

				void make_scale(T x, T y) noexcept { this->set(x, 0, 0, y); }
				void make_scale(const Vector2<T>& sz) noexcept { this->set(sz.x, 0, 0, sz.y); }

				void make_rotate(T x, T y, T z, T angle) noexcept { make_rotate(x, y, z, angle); }
				void make_rotate(const Quaternion<T>& q) noexcept { make_rotate(q.x, q.y, q.z, q.w); }

				void make_rotate(const Vector3<T>& axis, T angle) noexcept
				{
					T c, s;

					math::sinCos(&s, &c, degrees(angle));

					Vector3<T> v = axis;
					v.normalize();

					T x = v.x;
					T y = v.y;
					T z = v.z;

					T t = 1 - c;
					T tx = t * x, ty = t * y;

					this->a1 = (tx * x + c);
					this->a2 = (tx * y + s * z);

					this->b1 = (tx * y - s * z);
					this->b2 = (ty * y + c);
				}

				void make_rotation_x(T theta) noexcept
				{
					T ang = degrees(theta);
					T c, s;

					sinCos(&s, &c, ang);

					set(
						1, 0,
						0, c);
				}

				void make_rotation_y(T theta) noexcept
				{
					T ang = degrees(theta);
					T c, s;

					sinCos(&s, &c, ang);

					set(
						c, 0,
						0, 1);
				}

				void make_rotation_z(T theta) noexcept
				{
					T ang = degrees(theta);
					T c, s;

					sinCos(&s, &c, ang);

					set(
						c, -s,
						s, c);
				}

				pointer ptr() noexcept { return (pointer)this; }
				const_pointer ptr() const noexcept { return (const_pointer)this; }
				pointer data() noexcept { return (pointer)this; }
				const_pointer data() const noexcept { return (const_pointer)this; }

			private:
				T a1, a2;
				T b1, b2;
			};
		}
	}
}

#endif