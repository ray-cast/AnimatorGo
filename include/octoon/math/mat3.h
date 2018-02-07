#ifndef OCTOON_MATRIX3X3_H_
#define OCTOON_MATRIX3X3_H_

#include <octoon/math/vector3.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T>
			class Matrix3x3
			{
			public:
				typedef typename trait::type_addition<T>::value_type value_type;
				typedef typename trait::type_addition<T>::pointer pointer;
				typedef typename trait::type_addition<T>::const_pointer const_pointer;
				typedef typename trait::type_addition<T>::reference reference;
				typedef typename trait::type_addition<T>::const_reference const_reference;

				Matrix3x3() noexcept
				{
				}

				Matrix3x3(T _a1, T _a2, T _a3, T _b1, T _b2, T _b3, T _c1, T _c2, T _c3) noexcept
					: a1(_a1), a2(_a2), a3(_a3)
					, b1(_b1), b2(_b2), b3(_b3)
					, c1(_c1), c2(_c2), c3(_c3)
				{
				}

				Matrix3x3(const Matrix3x3<T>& m1, const Matrix3x3<T>& m2) noexcept
				{
					this->multiplyMatrices(m1, m2);
				}

				Matrix3x3(const Vector3<T>& axis, T angle) noexcept
				{
					this->makeRotate(axis, angle);
				}

				explicit Matrix3x3(const Quaternion<T>& q) noexcept
				{
					this->makeRotate(q);
				}

				explicit Matrix3x3(const Matrix4x4<T>& m) noexcept
				{
					a1 = m.a1; a2 = m.a2; a3 = m.a3;
					b1 = m.b1; b2 = m.b2; b3 = m.b3;
					c1 = m.c1; c2 = m.c2; c3 = m.c3;
				}

				template<typename S, typename = std::enable_if<!std::is_pointer<S>::value>>
				explicit operator Matrix3x3<S>() const noexcept
				{
					return Matrix3x3<S>(
						static_cast<S>(a1), static_cast<S>(a2), static_cast<S>(a3),
						static_cast<S>(b1), static_cast<S>(b2), static_cast<S>(b3),
						static_cast<S>(c1), static_cast<S>(c2), static_cast<S>(c3));
				}

				Matrix3x3<T>& operator -= (const Matrix3x3<T>& m) noexcept
				{
					a1 -= m.a1; b1 -= m.a2; c1 -= m.c1;
					a2 -= m.a2; b2 -= m.a2; c2 -= m.c2;
					a3 -= m.a3; b3 -= m.a2; c3 -= m.c3;
					return *this;
				}

				Matrix3x3<T>& operator += (const Matrix3x3<T>& m) noexcept
				{
					a1 += m.a1; b1 += m.a2; c1 += m.c1;
					a2 += m.a2; b2 += m.a2; c2 += m.c2;
					a3 += m.a3; b3 += m.a2; c3 += m.c3;
					return *this;
				}

				Matrix3x3<T>& operator *= (const Matrix3x3<T>& m) noexcept
				{
					*this = Matrix3x3<T>(
						a1 * m.a1 + a2 * m.b1 + a3 * m.c1,
						a1 * m.a2 + a2 * m.b2 + a3 * m.c2,
						a1 * m.a3 + a2 * m.b3 + a3 * m.c3,
						b1 * m.a1 + b2 * m.b1 + b3 * m.c1,
						b1 * m.a2 + b2 * m.b2 + b3 * m.c2,
						b1 * m.a3 + b2 * m.b3 + b3 * m.c3,
						c1 * m.a1 + c2 * m.b1 + c3 * m.c1,
						c1 * m.a2 + c2 * m.b2 + c3 * m.c2,
						c1 * m.a3 + c2 * m.b3 + c3 * m.c3);
					return *this;
				}

				T& operator() (std::size_t m, std::size_t n) noexcept
				{
					assert(m * n < 9);
					return *(&a1)[m * 3 + n];
				}

				const T& operator() (std::size_t m, std::size_t n) const noexcept
				{
					assert(m * n < 9);
					return *(&a1)[m * 3 + n];
				}

				T& operator[] (std::size_t n) noexcept
				{
					assert(n < 9);
					return *((&a1) + n);
				}

				const T& operator[] (std::size_t n) const noexcept
				{
					assert(n < 9);
					return *((&a1) + n);
				}

				explicit operator T*() noexcept
				{
					return this->ptr();
				}

				explicit operator const T*() const noexcept
				{
					return this->ptr();
				}

				T* ptr() noexcept { return (T*)&a1; }
				T* data() noexcept { return (T*)&a1; }

				const T* ptr() const noexcept { return (const T*)&a1; }
				const T* data() const noexcept { return (const T*)&a1; }

				Matrix3x3<T>& set(T mt00, T mt01, T mt02, T mt10, T mt11, T mt12, T mt20, T mt21, T mt22) noexcept
				{
					a1 = T(mt00); a2 = T(mt01); a3 = T(mt02);
					b1 = T(mt10); b2 = T(mt11); b3 = T(mt12);
					c1 = T(mt20); c2 = T(mt21); c3 = T(mt22);

					return *this;
				}

				Matrix3x3<T>& set(const Matrix4x4<T>& m) noexcept
				{
					a1 = m.a1; a2 = m.a2; a3 = m.a3;
					b1 = m.b1; b2 = m.b2; b3 = m.b3;
					c1 = m.c1; c2 = m.c2; c3 = m.c3;
					return *this;
				}

				Matrix3x3<T>& loadIdentity() noexcept
				{
					set(1, 0, 0,
						0, 1, 0,
						0, 0, 1);
					return *this;
				}

				Matrix3x3<T>& multiply_scalar(T scale) noexcept
				{
					a1 *= scale; a2 *= scale; a3 *= scale;
					b1 *= scale; b2 *= scale; b3 *= scale;
					c1 *= scale; c2 *= scale; c3 *= scale;

					return *this;
				}

				Matrix3x3<T>& multiplyMatrices(const Matrix3x3<T>& m1, const Matrix3x3<T>& m2) noexcept
				{
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

				Matrix3x3<T>& makeRotationX(T theta) noexcept
				{
					T ang = radians(theta);
					T c, s;

					sinCos(&s, &c, ang);

					return set(
						1, 0, 0,
						0, c, -s,
						0, s, c);
				}

				Matrix3x3<T>& makeRotationY(T theta) noexcept
				{
					T ang = radians(theta);
					T c, s;

					sinCos(&s, &c, ang);

					return set(
						c, 0, s,
						0, 1, 0,
						-s, 0, c);
				}

				Matrix3x3<T>& makeRotationZ(T theta) noexcept
				{
					T c, s, ang = radians(theta);

					sinCos(&s, &c, ang);

					return set(
						c, -s, 0,
						s, c, 0,
						0, 0, 1);
				}

				Matrix3x3<T>& makeRotate(T eulerX, T eulerY, T eulerZ) noexcept
				{
					T sj, cj, si, ci, sh, ch;

					math::sinCos(&si, &ci, radians(eulerX));
					math::sinCos(&sj, &cj, radians(eulerY));
					math::sinCos(&sh, &ch, radians(eulerZ));

					T cc = ci * ch;
					T cs = ci * sh;
					T sc = si * ch;
					T ss = si * sh;

					a1 = cj * ch;
					a2 = sj * sc - cs;
					a3 = sj * cc + ss;

					b1 = cj * sh;
					b2 = sj * ss + cc;
					b3 = sj * cs - sc;

					c1 = -sj;
					c2 = cj * si;
					c3 = cj * ci;

					return *this;
				}

				Matrix3x3<T>& makeRotate(const Vector3<T>& forward, const Vector3<T>& up, const Vector3<T>& right) noexcept
				{
					a1 = right.x;
					a2 = right.y;
					a3 = right.z;

					b1 = up.x;
					b2 = up.y;
					b3 = up.z;

					c1 = forward.x;
					c2 = forward.y;
					c3 = forward.z;
					return *this;
				}

				Matrix3x3<T>& makeRotate(const Vector3<T>& axis, T angle) noexcept
				{
					T c, s;

					sinCos(&s, &c, radians(angle));

					Vector3<T> v = axis;
					v.normalize();

					T x = v.x;
					T y = v.y;
					T z = v.z;

					T t = 1 - c;

					T tx = t * x;
					T ty = t * y;
					T tz = t * z;

					a1 = (tx * x + c);
					b1 = (tx * y + s * z);
					c1 = (tx * z - s * y);

					a2 = (tx * y - s * z);
					b2 = (ty * y + c);
					c2 = (ty * z + s * x);

					a3 = (tx * z + s * y);
					b3 = (ty * z - s * x);
					c3 = (tz * z + c);

					return *this;
				}

				Matrix3x3<T>& makeRotate(const Quaternion<T>& q) noexcept
				{
					T xs = q.x * T(2.0), ys = q.y * T(2.0), zs = q.z * T(2.0);
					T wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
					T xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
					T yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;

					a1 = T(1.0) - (yy + zz);
					b1 = xy - wz;
					c1 = xz + wy;

					a2 = xy + wz;
					b2 = T(1.0) - (xx + zz);
					c2 = yz - wx;

					a3 = xz - wy;
					b3 = yz + wx;
					c3 = T(1.0) - (xx + yy);

					return *this;
				}

				Matrix3x3<T>& rotate(const Vector3<T>& axis, T angle) noexcept
				{
					Matrix3x3<T> m1(axis, angle);
					Matrix3x3<T> m2(*this);
					return multiplyMatrices(m1, m2);
				}

				Matrix3x3<T>& rotate(const Quaternion<T>& q) noexcept
				{
					return rotate(q.x, q.y, q.z, q.w);
				}

				Quaternion<T> getRotate() const noexcept
				{
					return Quaterniont<T>(this->getForward(), this->getUpVector(), this->getRight());
				}

				Matrix3x3<T>& makeScale(const Vector3<T>& sz) noexcept
				{
					set(sz.x, 0, 0,
						0, sz.y, 0,
						0, 0, sz.z);
					return *this;
				}

				Matrix3x3<T>& makeScale(T x, T y, T z) noexcept
				{
					set(x, 0, 0,
						0, y, 0,
						0, 0, z);
					return *this;
				}

				Matrix3x3<T>& scale(const Vector3<T>& sz) noexcept
				{
					return scale(sz.x, sz.y, sz.z);
				}

				Matrix3x3<T>& scale(T x, T y, T z) noexcept
				{
					a1 *= x;
					a2 *= x;
					a3 *= x;
					b1 *= y;
					b2 *= y;
					b3 *= y;
					c1 *= z;
					c2 *= z;
					return *this;
				}

				const Vector3<T>& getRight() const noexcept
				{
					return *(Vector3<T>*)&a1;
				}

				const Vector3<T>& getUpVector() const noexcept
				{
					return *(Vector3<T>*)&b1;
				}

				const Vector3<T>& getForward() const noexcept
				{
					return *(Vector3<T>*)&c1;
				}

			public:
				T a1, a2, a3;
				T b1, b2, b3;
				T c1, c2, c3;
			};
		}

		template<typename T>
		inline bool operator==(const detail::Matrix3x3<T>& m1, const detail::Matrix3x3<T>& m2) noexcept
		{
			return
				m1.a1 == m2.a1 && m1.a2 == m2.a2 && m1.a3 == m2.a3 &&
				m1.b1 == m2.b1 && m1.b2 == m2.b2 && m1.b3 == m2.b3 &&
				m1.c1 == m2.c1 && m1.c2 == m2.c2 && m1.c3 == m2.c3;
		}

		template<typename T>
		inline bool operator!=(const detail::Matrix3x3<T>& m1, const detail::Matrix3x3<T>& m2) noexcept
		{
			return
				m1.a1 != m2.a1 || m1.a2 != m2.a2 || m1.a3 != m2.a3 ||
				m1.b1 != m2.b1 || m1.b2 != m2.b2 || m1.b3 != m2.b3 ||
				m1.c1 != m2.c1 || m1.c2 != m2.c2 || m1.c3 != m2.c3;
		}

		template<typename T>
		inline detail::Vector3<T> operator*(const detail::Vector3<T>& v, const detail::Matrix3x3<T>& m) noexcept
		{
			return detail::Vector3<T>(
				m.a1 * v.x + m.a2 * v.y + m.a3 * v.z,
				m.b1 * v.x + m.b2 * v.y + m.b3 * v.z,
				m.c1 * v.x + m.c2 * v.y + m.c3 * v.z);
		}

		template<typename T>
		inline detail::Vector3<T> operator*(const detail::Matrix3x3<T>& m, const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(
				m.a1 * v.x + m.b1 * v.y + m.c1 * v.z,
				m.a2 * v.x + m.b2 * v.y + m.c2 * v.z,
				m.a3 * v.x + m.b3 * v.y + m.c3 * v.z);
		}

		template<typename T>
		inline detail::Matrix3x3<T> operator*(T& scale, const detail::Matrix3x3<T>& m1) noexcept
		{
			detail::Matrix3x3<T> m;
			m.a1 = m1.a1 * scale; m.b1 = m1.b1 * scale; m.c1 = m1.c1 * scale;
			m.a2 = m1.a2 * scale; m.b2 = m1.b2 * scale; m.c2 = m1.c2 * scale;
			m.a3 = m1.a3 * scale; m.b3 = m1.b3 * scale; m.c3 = m1.c3 * scale;
			return m;
		}

		template<typename T>
		inline detail::Matrix3x3<T> operator*(const detail::Matrix3x3<T>& m1, T& scale) noexcept
		{
			detail::Matrix3x3<T> m;
			m.a1 = m1.a1 * scale; m.b1 = m1.b1 * scale; m.c1 = m1.c1 * scale;
			m.a2 = m1.a2 * scale; m.b2 = m1.b2 * scale; m.c2 = m1.c2 * scale;
			m.a3 = m1.a3 * scale; m.b3 = m1.b3 * scale; m.c3 = m1.c3 * scale;
			return m;
		}

		template<typename T>
		inline detail::Matrix3x3<T> operator*(const detail::Matrix3x3<T>& m1, const detail::Matrix3x3<T>& m2) noexcept
		{
			return detail::Matrix3x3<T>(m1, m2);
		}

		template<typename T>
		inline detail::Vector3<T>& operator*=(detail::Vector3<T>& v, const detail::Matrix3x3<T>& m) noexcept
		{
			v = v * m;
			return v;
		}

		template<typename T>
		inline T determinant(const detail::Matrix3x3<T>& m) noexcept
		{
			return m.a1 * m.b2 * m.c3 - m.a1 * m.b3 * m.c2 + m.a2 * m.b3 * m.c1 - m.a2 * m.b1 * m.c3 + m.a3 * m.b1 * m.c2 - m.a3 * m.b2 * m.c1;
		}

		template<typename T>
		bool isIdentity(const detail::Matrix3x3<T>& m) noexcept
		{
			constexpr T epsilon = (T)EPSILON_E4;
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
		bool isOnlyRotate(const detail::Matrix3x3<T>& m) noexcept
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
		detail::Matrix3x3<T> orthonormalize(const detail::Matrix3x3<T>& _m) noexcept
		{
			detail::Matrix3x3<T> m;
			detail::Vector3<T> x(_m.a1, _m.b1, _m.c1);
			detail::Vector3<T> y(_m.a2, _m.b2, _m.c2);
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
		detail::Matrix3x3<T> transpose(const detail::Matrix3x3<T>& _m) noexcept
		{
			detail::Matrix3x3<T> m = _m;
			std::swap((T&)m.a2, (T&)m.b1);
			std::swap((T&)m.a3, (T&)m.c1);
			std::swap((T&)m.b3, (T&)m.c2);
			return m;
		}

		template<typename T>
		detail::Matrix3x3<T> inverse(const detail::Matrix3x3<T>& _m) noexcept
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

			Matrix4x4<T> m;
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
