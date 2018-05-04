#ifndef OCTOON_MATRIX3X3_H_
#define OCTOON_MATRIX3X3_H_

#include <octoon/math/vector3.h>
#include <octoon/math/quat.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T>
			class Matrix3x3 final
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
						T a1, a2, a3;
						T b1, b2, b3;
						T c1, c2, c3;
					};

					struct
					{
						Vector3<T> right;
						Vector3<T> up;
						Vector3<T> forward;
					};
				};

				Matrix3x3() = default;
				Matrix3x3(T mt00, T mt01, T mt02, T mt10, T mt11, T mt12, T mt20, T mt21, T mt22) noexcept : a1(mt00), a2(mt01), a3(mt02), b1(mt10), b2(mt11), b3(mt12), c1(mt20), c2(mt21), c3(mt22){}
				Matrix3x3(const Matrix3x3<T>& m1, const Matrix3x3<T>& m2) noexcept { this->make_matrix(m1, m2); }
				Matrix3x3(const Vector3<T>& axis, T angle) noexcept { this->makeRotation(axis, angle); }
				Matrix3x3(const Quaternion<T>& q) noexcept { this->makeRotation(q); }
				~Matrix3x3() = default;

				static const Matrix3x3<T> Zero;
				static const Matrix3x3<T> One;

				Matrix3x3<T>& operator-=(const Matrix3x3<T>& m) noexcept
				{
					a1 -= m.a1; b1 -= m.a2; c1 -= m.c1;
					a2 -= m.a2; b2 -= m.a2; c2 -= m.c2;
					a3 -= m.a3; b3 -= m.a2; c3 -= m.c3;
					return *this;
				}

				Matrix3x3<T>& operator+=(const Matrix3x3<T>& m) noexcept
				{
					a1 += m.a1; b1 += m.a2; c1 += m.c1;
					a2 += m.a2; b2 += m.a2; c2 += m.c2;
					a3 += m.a3; b3 += m.a2; c3 += m.c3;
					return *this;
				}

				Matrix3x3<T>& operator*=(const Matrix3x3<T>& m) noexcept
				{
					Matrix3x3<T> m1(*this);
					this->make_matrix(m1, m);
					return *this;
				}

				reference operator()(std::uint8_t m, std::uint8_t n) noexcept
				{
					assert(m * n < 9);
					return *(&a1)[m * 3 + n];
				}

				const_reference operator()(std::uint8_t m, std::uint8_t n) const noexcept
				{
					assert(m * n < 9);
					return *(&a1)[m * 3 + n];
				}

				reference operator[](std::uint8_t n) noexcept
				{
					assert(n < 9);
					return *((&a1) + n);
				}

				const_reference operator[](std::uint8_t n) const noexcept
				{
					assert(n < 9);
					return *((&a1) + n);
				}

				template<typename S, typename = std::enable_if_t<std::is_integral<S>::value || std::is_floating_point<S>::value>>
				explicit operator Matrix3x3<S>() const noexcept
				{
					return Matrix3x3<S>(
						static_cast<S>(a1), static_cast<S>(a2), static_cast<S>(a3),
						static_cast<S>(b1), static_cast<S>(b2), static_cast<S>(b3),
						static_cast<S>(c1), static_cast<S>(c2), static_cast<S>(c3));
				}

				explicit operator pointer() noexcept
				{
					return this->ptr();
				}

				explicit operator const_pointer() const noexcept
				{
					return this->ptr();
				}

				Matrix3x3<T>& make_matrix(T mt00, T mt01, T mt02, T mt10, T mt11, T mt12, T mt20, T mt21, T mt22) noexcept
				{
					a1 = mt00; a2 = mt01; a3 = mt02;
					b1 = mt10; b2 = mt11; b3 = mt12;
					c1 = mt20; c2 = mt21; c3 = mt22;
					return *this;
				}

				Matrix3x3<T>& make_matrix(const Matrix4x4<T>& m) noexcept
				{
					a1 = m.a1; a2 = m.a2; a3 = m.a3;
					b1 = m.b1; b2 = m.b2; b3 = m.b3;
					c1 = m.c1; c2 = m.c2; c3 = m.c3;
					return *this;
				}

				Matrix3x3<T>& make_matrix(const Matrix3x3<T>& m1, const Matrix3x3<T>& m2) noexcept
				{
					assert(&m1 != this && &m2 != this);

					a1 = m1.a1 * m2.a1 + m1.b1 * m2.a2 + m1.c1 * m2.a3;
					a2 = m1.a2 * m2.a1 + m1.b2 * m2.a2 + m1.c2 * m2.a3;
					a3 = m1.a3 * m2.a1 + m1.b3 * m2.a2 + m1.c3 * m2.a3;
					b1 = m1.a1 * m2.b1 + m1.b1 * m2.b2 + m1.c1 * m2.b3;
					b2 = m1.a2 * m2.b1 + m1.b2 * m2.b2 + m1.c2 * m2.b3;
					b3 = m1.a3 * m2.b1 + m1.b3 * m2.b2 + m1.c3 * m2.b3;
					c1 = m1.a1 * m2.c1 + m1.b1 * m2.c2 + m1.c1 * m2.c3;
					c2 = m1.a2 * m2.c1 + m1.b2 * m2.c2 + m1.c2 * m2.c3;
					c3 = m1.a3 * m2.c1 + m1.b3 * m2.c2 + m1.c3 * m2.c3;
					return *this;
				}

				Matrix3x3<T>& make_identity() noexcept
				{
					a1 = 1.0f; a2 = 0.0f; a3 = 0.0f;
					b1 = 0.0f; b2 = 1.0f; b3 = 0.0f;
					c1 = 0.0f; c2 = 0.0f; c3 = 1.0f;
					return *this;
				}

				Matrix3x3<T>& make_scale(T x, T y, T z) noexcept
				{
					set(x, 0, 0,
						0, y, 0,
						0, 0, z);
					return *this;
				}

				Matrix3x3<T>& make_scale(const Vector3<T>& sz) noexcept
				{
					set(sz.x, 0, 0,
						0, sz.y, 0,
						0, 0, sz.z);
					return *this;
				}

				Matrix3x3<T>& make_rotation_x(T theta) noexcept
				{
					T c, s;
					math::sinCos(&s, &c, theta);

					a1 = 1; a2 = 0; a3 = 0;
					b1 = 0; b2 = c; b3 = s;
					c1 = 0; c2 =-s; c3 = c;
					return *this;
				}

				Matrix3x3<T>& make_rotation_y(T theta) noexcept
				{
					T c, s;
					math::sinCos(&s, &c, theta);

					a1 = c; a2 = 0; a3 =-s;
					b1 = 0; b2 = 1; b3 = 0;
					c1 = s; c2 = 0; c3 = c;
					return *this;
				}

				Matrix3x3<T>& make_rotation_z(T theta) noexcept
				{
					T c, s;
					math::sinCos(&s, &c, theta);

					a1 = c; a2 = s; a3 = 0;
					b1 =-s; b2 = c; b3 = 0;
					c1 = 0; c2 = 0; c3 = 1;
					return *this;
				}

				Matrix3x3<T>& makeRotation(const Vector3<T>& axis, T theta) noexcept
				{
					T c, s;
					math::sinCos(&s, &c, theta);

					T x = axis.x;
					T y = axis.y;
					T z = axis.z;

					T t = 1 - c;
					T tx = t * x;
					T ty = t * y;
					T tz = t * z;

					a1 = (tx * x + c);
					a2 = (tx * y + s * z);
					a3 = (tx * z - s * y);

					b1 = (tx * y - s * z);
					b2 = (ty * y + c);
					b3 = (ty * z + s * x);

					c1 = (tx * z + s * y);
					c2 = (ty * z - s * x);
					c3 = (tz * z + c);

					return *this;
				}

				Matrix3x3<T>& makeRotation(const Quaternion<T>& q) noexcept
				{
					T xs = q.x * T(2.0f), ys = q.y * T(2.0f), zs = q.z * T(2.0f);
					T wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
					T xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
					T yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;

					a1 = T(1.0f) - (yy + zz);
					a2 = xy + wz;
					a3 = xz - wy;

					b1 = xy - wz;
					b2 = T(1.0f) - (xx + zz);
					b3 = yz + wx;

					c1 = xz + wy;
					c2 = yz - wx;
					c3 = T(1.0f) - (xx + yy);

					return *this;
				}

				Matrix3x3<T>& makeRotation(const Vector3<T>& forward_, const Vector3<T>& up_, const Vector3<T>& right_) noexcept
				{
					this->right = right_;
					this->up = up_;
					this->forward = forward_;
					return *this;
				}

				Matrix3x3<T>& multiply_scalar(T scale) noexcept
				{
					this->right *= scale;
					this->up *= scale;
					this->forward *= scale;
					return *this;
				}

				Matrix3x3<T>& multiply_scalar(T x, T y, T z) noexcept
				{
					this->right *= x;
					this->up *= y;
					this->forward *= z;
					return *this;
				}

				Matrix3x3<T>& multiply_scalar(const Vector3<T>& sz) noexcept
				{
					this->right *= sz.x;
					this->up *= sz.y;
					this->forward *= sz.z;
					return *this;
				}

				Matrix3x3<T>& multiply_rotate(const Vector3<T>& axis, T angle) noexcept
				{
					Matrix3x3<T> m1(*this);
					Matrix3x3<T> m2(axis, angle);
					return make_matrix(m1, m2);
				}

				Matrix3x3<T>& multiply_rotate(const Quaternion<T>& q) noexcept
				{
					Matrix3x3<T> m1(*this);
					Matrix3x3<T> m2(q);
					return make_matrix(m1, m2);
				}

				pointer ptr() noexcept { return (pointer)&a1; }
				const_pointer ptr() const noexcept { return (const_pointer)&a1; }

				pointer data() noexcept { return (pointer)&a1; }
				const_pointer data() const noexcept { return (const_pointer)&a1; }

			public:

				friend bool operator==(const Matrix3x3<T>& m1, const Matrix3x3<T>& m2) noexcept
				{
					return
						m1.a1 == m2.a1 && m1.a2 == m2.a2 && m1.a3 == m2.a3 &&
						m1.b1 == m2.b1 && m1.b2 == m2.b2 && m1.b3 == m2.b3 &&
						m1.c1 == m2.c1 && m1.c2 == m2.c2 && m1.c3 == m2.c3;
				}

				friend bool operator!=(const Matrix3x3<T>& m1, const Matrix3x3<T>& m2) noexcept
				{
					return
						m1.a1 != m2.a1 || m1.a2 != m2.a2 || m1.a3 != m2.a3 ||
						m1.b1 != m2.b1 || m1.b2 != m2.b2 || m1.b3 != m2.b3 ||
						m1.c1 != m2.c1 || m1.c2 != m2.c2 || m1.c3 != m2.c3;
				}

				friend Vector3<T> operator*(const Vector3<T>& v, const Matrix3x3<T>& m) noexcept
				{
					return Vector3<T>(
						m.a1 * v.x + m.a2 * v.y + m.a3 * v.z,
						m.b1 * v.x + m.b2 * v.y + m.b3 * v.z,
						m.c1 * v.x + m.c2 * v.y + m.c3 * v.z);
				}

				friend Vector3<T> operator*(const Matrix3x3<T>& m, const Vector3<T>& v) noexcept
				{
					return Vector3<T>(
						m.a1 * v.x + m.b1 * v.y + m.c1 * v.z,
						m.a2 * v.x + m.b2 * v.y + m.c2 * v.z,
						m.a3 * v.x + m.b3 * v.y + m.c3 * v.z);
				}

				friend Matrix3x3<T> operator*(const T& scale, const Matrix3x3<T>& m1) noexcept
				{
					Matrix3x3<T> m;
					m.a1 = m1.a1 * scale; m.b1 = m1.b1 * scale; m.c1 = m1.c1 * scale;
					m.a2 = m1.a2 * scale; m.b2 = m1.b2 * scale; m.c2 = m1.c2 * scale;
					m.a3 = m1.a3 * scale; m.b3 = m1.b3 * scale; m.c3 = m1.c3 * scale;
					return m;
				}

				friend Matrix3x3<T> operator*(const Matrix3x3<T>& m1, T& scale) noexcept
				{
					Matrix3x3<T> m;
					m.a1 = m1.a1 * scale; m.b1 = m1.b1 * scale; m.c1 = m1.c1 * scale;
					m.a2 = m1.a2 * scale; m.b2 = m1.b2 * scale; m.c2 = m1.c2 * scale;
					m.a3 = m1.a3 * scale; m.b3 = m1.b3 * scale; m.c3 = m1.c3 * scale;
					return m;
				}

				friend Matrix3x3<T> operator*(const Matrix3x3<T>& m1, const Matrix3x3<T>& m2) noexcept
				{
					return Matrix3x3<T>(m1, m2);
				}

				friend Vector3<T>& operator*=(Vector3<T>& v, const Matrix3x3<T>& m) noexcept
				{
					v = v * m;
					return v;
				}
			};

			template<typename T> const Matrix3x3<T> Matrix3x3<T>::Zero(0, 0, 0, 0, 0, 0, 0, 0, 0);
			template<typename T> const Matrix3x3<T> Matrix3x3<T>::One(1, 0, 0, 0, 1, 0, 0, 0, 1);
		}

		template<typename T>
		inline const detail::Vector3<T>& right(const detail::Matrix3x3<T>& m) noexcept
		{
			return m.right;
		}

		template<typename T>
		inline const detail::Vector3<T>& up(const detail::Matrix3x3<T>& m) noexcept
		{
			return m.up;
		}

		template<typename T>
		inline const detail::Vector3<T>& forward(const detail::Matrix3x3<T>& m) noexcept
		{
			return m.forward;
		}

		template<typename T>
		inline T determinant(const detail::Matrix3x3<T>& m) noexcept
		{
			return m.a1 * m.b2 * m.c3 - m.a1 * m.b3 * m.c2 + m.a2 * m.b3 * m.c1 - m.a2 * m.b1 * m.c3 + m.a3 * m.b1 * m.c2 - m.a3 * m.b2 * m.c1;
		}

		template<typename T>
		inline bool is_identity(const detail::Matrix3x3<T>& m) noexcept
		{
			constexpr T epsilon = EPSILON_E4;
			return (
				m.a2 <= epsilon && m.a2 >= -epsilon &&
				m.a3 <= epsilon && m.a3 >= -epsilon &&
				m.b1 <= epsilon && m.b1 >= -epsilon &&
				m.b3 <= epsilon && m.b3 >= -epsilon &&
				m.c1 <= epsilon && m.c1 >= -epsilon &&
				m.c2 <= epsilon && m.c2 >= -epsilon &&
				m.a1 <= 1.f + epsilon && m.a1 >= 1.f - epsilon &&
				m.b2 <= 1.f + epsilon && m.b2 >= 1.f - epsilon &&
				m.c3 <= 1.f + epsilon && m.c3 >= 1.f - epsilon);
		}

		template<typename T>
		inline bool is_only_rotate(const detail::Matrix3x3<T>& m) noexcept
		{
			constexpr T epsilon = 10e-3f;
			return (
				m.a1 <= 1.f + epsilon && m.a1 >= 1.f - epsilon &&
				m.a2 <= 1.f + epsilon && m.a2 >= 1.f - epsilon &&
				m.a3 <= 1.f + epsilon && m.a3 >= 1.f - epsilon &&
				m.b1 <= 1.f + epsilon && m.b1 >= 1.f - epsilon &&
				m.b2 <= 1.f + epsilon && m.b2 >= 1.f - epsilon &&
				m.b3 <= 1.f + epsilon && m.b3 >= 1.f - epsilon &&
				m.c1 <= 1.f + epsilon && m.c1 >= 1.f - epsilon &&
				m.c2 <= 1.f + epsilon && m.c2 >= 1.f - epsilon &&
				m.c3 <= 1.f + epsilon && m.c3 >= 1.f - epsilon);
		}

		template<typename T>
		inline detail::Matrix3x3<T> orthonormalize(const detail::Matrix3x3<T>& m_) noexcept
		{
			detail::Matrix3x3<T> m;
			detail::Vector3<T> x = m.right;
			detail::Vector3<T> y = m.up;
			detail::Vector3<T> z;

			x = math::normalize(x);
			z = math::cross(x, y);
			z = math::normalize(z);
			y = math::cross(z, x);
			y = math::normalize(y);

			m.a1 = x.x; m.a2 = x.y; m.a3 = x.z;
			m.b1 = y.x; m.b2 = y.y; m.b3 = y.z;
			m.c1 = z.x; m.c3 = z.y; m.c3 = z.z;
			return m;
		}

		template<typename T>
		inline detail::Matrix3x3<T> transpose(const detail::Matrix3x3<T>& _m) noexcept
		{
			detail::Matrix3x3<T> m = _m;
			std::swap((T&)m.a2, (T&)m.b1);
			std::swap((T&)m.a3, (T&)m.c1);
			std::swap((T&)m.b3, (T&)m.c2);
			return m;
		}

		template<typename T>
		inline detail::Matrix3x3<T> inverse(const detail::Matrix3x3<T>& _m) noexcept
		{
			const T det = determinant(_m);
			if (det == T(0.0))
			{
				const T nan = std::numeric_limits<T>::quiet_NaN();
				return detail::Matrix3x3<T>(
					nan, nan, nan,
					nan, nan, nan,
					nan, nan, nan);
			}

			T invdet = T(1.0) / det;

			detail::Matrix3x3<T> m;
			m.a1 = invdet * (_m.b2 * _m.c3 - _m.b3 * _m.c2);
			m.a2 = -invdet * (_m.a2 * _m.c3 - _m.a3 * _m.c2);
			m.a3 = invdet * (_m.a2 * _m.b3 - _m.a3 * _m.b2);
			m.b1 = -invdet * (_m.b1 * _m.c3 - _m.b3 * _m.c1);
			m.b2 = invdet * (_m.a1 * _m.c3 - _m.a3 * _m.c1);
			m.b3 = -invdet * (_m.a1 * _m.b3 - _m.a3 * _m.b1);
			m.c1 = invdet * (_m.b1 * _m.c2 - _m.b2 * _m.c1);
			m.c2 = -invdet * (_m.a1 * _m.c2 - _m.a2 * _m.c1);
			m.c3 = invdet * (_m.a1 * _m.b2 - _m.a2 * _m.b1);

			return m;
		}
	}
}

#endif
