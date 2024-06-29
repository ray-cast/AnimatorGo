#ifndef OCTOON_MATH_QUATERNION_H_
#define OCTOON_MATH_QUATERNION_H_

#include <octoon/math/trait.h>
#include <octoon/math/mathfwd.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T>
			class Quaternion final
			{
			public:
				typedef typename trait::type_addition<T>::value_type value_type;
				typedef typename trait::type_addition<T>::pointer pointer;
				typedef typename trait::type_addition<T>::const_pointer const_pointer;
				typedef typename trait::type_addition<T>::reference reference;
				typedef typename trait::type_addition<T>::const_reference const_reference;

				T x, y, z, w;

				static const Quaternion<T> Zero;

				Quaternion() = default;
				Quaternion(T xx, T yy, T zz, T ww) noexcept : x(xx), y(yy), z(zz), w(ww) {}
				Quaternion(const Vector3<T>& axis, T angle) noexcept { this->makeRotation(axis, angle); }
				Quaternion(const Vector3<T>& forward, const Vector3<T>& up, const Vector3<T>& right) noexcept { this->makeRotation(forward, up, right); }
				Quaternion(const Vector3<T>& A, const Vector3<T>& B) noexcept { this->makeRotation(A, B); }
				explicit Quaternion(const T xyzw[4]) noexcept : x(xyzw[0]), y(xyzw[1]), z(xyzw[2]), w(xyzw[3]) {}
				explicit Quaternion(const std::array<T, 4>& v) noexcept : x(v[0]), y(v[1]), z(v[2]), w(v[3]) {}
				explicit Quaternion(const Vector3<T>& eulerXYZ) noexcept { this->makeRotation(eulerXYZ); }
				explicit Quaternion(const Vector4<T>& xyzw) noexcept : x(xyzw.x), y(xyzw.y), z(xyzw.z), w(xyzw.w) {}
				explicit Quaternion(const Matrix3x3<T>& m) noexcept { this->makeRotation(forward(m), up(m), right(m)); }
				explicit Quaternion(const Matrix4x4<T>& m) noexcept { this->makeRotation(forward(m), up(m), right(m)); }
				~Quaternion() = default;

				constexpr T operator[](std::uint8_t i) const noexcept
				{
					assert(i < 4);
					return *(&x + i);
				}

				constexpr T& operator[](std::uint8_t i) noexcept
				{
					assert(i < 4);
					return *(&x + i);
				}

				Quaternion<T>& identity() noexcept
				{
					x = y = z = 0.0f; w = 1.0f;
					return *this;
				}

				Quaternion<T>& set(T xx, T yy, T zz, T ww) noexcept { x = xx; y = yy; z = zz; w = ww; return *this; }
				Quaternion<T>& set(const Vector3<T>& v) noexcept { return this->set(v.x, v.y, v.z, T(1)); }
				Quaternion<T>& set(const Vector4<T>& v) noexcept { return this->set(v.x, v.y, v.z, v.w); }

				Quaternion<T>& makeRotationX(T theta) noexcept
				{
					T thetaOver2 = theta * 0.5f;

					w = std::cos(thetaOver2);
					x = std::sin(thetaOver2);
					y = 0.0f;
					z = 0.0f;

					return *this;
				}

				Quaternion<T>& makeRotationY(T theta) noexcept
				{
					T thetaOver2 = theta * 0.5f;

					w = std::cos(thetaOver2);
					x = 0.0f;
					y = std::sin(thetaOver2);
					z = 0.0f;

					return *this;
				}

				Quaternion<T>& makeRotationZ(T theta) noexcept
				{
					T thetaOver2 = theta * 0.5f;

					w = std::cos(thetaOver2);
					x = 0.0f;
					y = 0.0f;
					z = std::sin(thetaOver2);

					return *this;
				}

				Quaternion<T>& makeRotation(const Vector3<T>& axis, T theta) noexcept
				{
					assert(std::abs(1.0f - (axis.x * axis.x + axis.y * axis.y + axis.z * axis.z)) < 1e-3f);

					T thetaOver2 = theta * 0.5f;

					T sin_a = std::sin(thetaOver2);
					T cos_a = std::cos(thetaOver2);

					w = cos_a;
					x = axis.x * sin_a;
					y = axis.y * sin_a;
					z = axis.z * sin_a;

					return *this;
				}

				Quaternion<T>& makeRotation(const Vector3<T>& euler) noexcept
				{
					T sp, sb, sh;
					T cp, cb, ch;

					sinCos(&sp, &cp, (T)(euler.x * 0.5f));
					sinCos(&sh, &ch, (T)(euler.y * 0.5f));
					sinCos(&sb, &cb, (T)(euler.z * 0.5f));

					x = cb * sp * ch + sb * cp * sh;
					y = cb * cp * sh - sb * sp * ch;
					z = sb * cp * ch - cb * sp * sh;
					w = cb * cp * ch + sb * sp * sh;

					return *this;
				}

				Quaternion<T>& makeRotation(const Vector3<T>& a, const Vector3<T>& b) noexcept
				{
					auto axis = cross(a, b);
					auto angle = acos(dot(a, b));

					if (!all(axis))
					{
						auto right = cross(Vector3<T>::UnitY, a);
						if (any(right))
							axis = cross(a, right);
						else
							axis = Vector3<T>(0.0f, 0.0f, -sign(a.y));
					}

					return makeRotation(axis, angle);
				}

				Quaternion<T>& makeRotationZYX(const Vector3<T>& euler) noexcept
				{
					T sp, sb, sh;
					T cp, cb, ch;

					sinCos(&sp, &cp, (T)(euler.x * 0.5f));
					sinCos(&sh, &ch, (T)(euler.y * 0.5f));
					sinCos(&sb, &cb, (T)(euler.z * 0.5f));

					x = sp * ch * cb - cp * sh * sb;
					y = cp * sh * cb + sp * ch * sb;
					z = cp * ch * sb - sp * sh * cb;
					w = cp * ch * cb + sp * sh * sb;

					return *this;
				}

				Quaternion<T>& makeRotation(const Vector3<T>& forward, const Vector3<T>& up, const Vector3<T>& right) noexcept
				{
					T t = 1 + right.x + up.y + forward.z;

					if (t > 0.001f)
					{
						T s = std::sqrt(t) * 2.0f;
						this->x = (up.z - forward.y) / s;
						this->y = (forward.x - right.z) / s;
						this->z = (right.y - up.x) / s;
						this->w = 0.25f * s;
					}
					else if (right.x > up.y && right.x > forward.z)
					{
						T s = std::sqrt(1.0f + right.x - up.y - forward.z) * 2.0f;
						this->x = 0.25f * s;
						this->y = (right.y + up.x) / s;
						this->z = (forward.x + right.z) / s;
						this->w = (up.z - forward.y) / s;
					}
					else if (up.y > forward.z)
					{
						T s = std::sqrt(1.0f + up.y - right.x - forward.z) * 2.0f;
						this->x = (right.y + up.x) / s;
						this->y = 0.25f * s;
						this->z = (up.z + forward.y) / s;
						this->w = (forward.x - right.z) / s;
					}
					else
					{
						T s = std::sqrt(1.0f + forward.z - right.x - up.y) * 2.0f;
						this->x = (forward.x + right.z) / s;
						this->y = (up.z + forward.y) / s;
						this->z = 0.25f * s;
						this->w = (right.y - up.x) / s;
					}

					return *this;
				}

				std::array<T, 4> to_array() const noexcept { return { x, y, z, w }; }

			public:
				friend Quaternion<T> operator-(const Quaternion<T>& q) noexcept
				{
					return Quaternion<T>(-q.x, -q.y, -q.z, -q.w);
				}

				friend bool operator==(const Quaternion<T>& q1, const Quaternion<T>& q2) noexcept
				{
					return q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w;
				}

				friend bool operator!=(const Quaternion<T>& q1, const Quaternion<T>& q2) noexcept
				{
					return !(q1 == q2);
				}

				friend Quaternion<T> operator+(const Quaternion<T>& q1, const Quaternion<T>& q2) noexcept
				{
					return Quaternion<T>(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w);
				}

				friend Quaternion<T> operator*(const Quaternion<T>& q, const T& scale) noexcept
				{
					return Quaternion<T>(q.x * scale, q.y * scale, q.z * scale, q.w * scale);
				}

				friend Quaternion<T> operator*(const Quaternion<T>& q1, const Quaternion<T>& q2) noexcept
				{
					return Quaternion<T>(
						q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
						q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
						q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
						q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
				}

				friend Quaternion<T>& operator*=(Quaternion<T>& q1, const Quaternion<T>& q2) noexcept
				{
					q1 = q1 * q2;
					return q1;
				}

				template<typename ostream, std::enable_if_t<trait::has_left_shift<ostream, T>::value, int> = 0>
				friend ostream& operator << (ostream& os, const Quaternion<T>& v)
				{
					os << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
					return os;
				}

				template<typename istream, std::enable_if_t<trait::has_right_shift<istream>::value, int> = 0>
				friend istream& operator >> (istream& is, Quaternion<T>& v)
				{
					is >> v.x;
					is.ignore(2);
					is >> v.y;
					is.ignore(2);
					is >> v.z;
					is.ignore(2);
					is >> v.w;
					return is;
				}
			};

			template<typename T> const Quaternion<T> Quaternion<T>::Zero = Quaternion<T>((T)0, (T)0, (T)0, (T)1);
		}

		template<typename T>
		inline bool equal(const detail::Quaternion<T>& q1, const detail::Quaternion<T>& q2) noexcept
		{
			return
				equal<T>(q1.x, q2.x) &&
				equal<T>(q1.y, q2.y) &&
				equal<T>(q1.z, q2.z) &&
				equal<T>(q1.w, q2.w);
		}

		template<typename T>
		inline T dot(const detail::Quaternion<T>& q1, const detail::Quaternion<T>& q2) noexcept
		{
			return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
		}

		template<typename T>
		inline T length2(const detail::Quaternion<T>& q) noexcept
		{
			return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
		}

		template<typename T>
		inline T length(const detail::Quaternion<T>& q) noexcept
		{
			return std::sqrt(length2(q));
		}

		template<typename T>
		inline T angle(const detail::Quaternion<T>& q) noexcept
		{
			return safe_acos(q.w) * T(2);
		}

		template<typename T>
		inline detail::Vector3<T> eulerAngles(const detail::Quaternion<T>& q) noexcept
		{
			T x = std::asin(clamp<T>(2.0f * (q.w * q.x - q.y * q.z), -1.0f, 1.0f));
			T y = std::atan2(2.0f * (q.w * q.y + q.x * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y));
			T z = std::atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.x * q.x + q.z * q.z));
			return detail::Vector3<T>(x, y, z);
		}

		template<typename T>
		inline detail::Vector3<T> axis(const detail::Quaternion<T>& q) noexcept
		{
			T sinThetaOver2Sq = 1.0f - q.w * q.w;
			if (sinThetaOver2Sq <= 0.0f)
				return detail::Vector3<T>::UnitX;

			return detail::Vector3<T>(q.x, q.y, q.z) * (1.0f / std::sqrt(sinThetaOver2Sq));
		}

		template<typename T>
		inline detail::Vector3<T> basicVector0(const detail::Quaternion<T>& q) noexcept
		{
			const float x2 = q.x * 2.0f;
			const float w2 = q.w * 2.0f;
			return detail::Vector3<T>((q.w * q.w2) - 1.0f + q.x * q.x2, (q.z * q.w2) + q.y * q.x2, (-q.y * q.w2) + q.z * q.x2);
		}

		template<typename T>
		inline detail::Vector3<T> basicVector1(const detail::Quaternion<T>& q) noexcept
		{
			const float y2 = q.y * 2.0f;
			const float w2 = q.w * 2.0f;
			return detail::Vector3<T>((-q.z * q.w2) + q.x * q.y2, (q.w * q.w2) - 1.0f + q.y * q.y2, (q.x * q.w2) + q.z * q.y2);
		}

		template<typename T>
		inline detail::Vector3<T> basicVector2(const detail::Quaternion<T>& q) noexcept
		{
			const float z2 = q.z * 2.0f;
			const float w2 = q.w * 2.0f;
			return detail::Vector3<T>((q.y * q.w2) + q.x * q.z2, (-q.x * q.w2) + q.y * z2, (q.w * q.w2) - 1.0f + q.z * q.z2);
		}

		template<typename T>
		inline detail::Vector3<T> rotate(const detail::Quaternion<T>& q, const detail::Vector3<T>& v) noexcept
		{
			const float vx = 2.0f * v.x;
			const float vy = 2.0f * v.y;
			const float vz = 2.0f * v.z;
			const float w2 = q.w * q.w - 0.5f;
			const float dot2 = (q.x * vx + q.y * vy + q.z * vz);
			return detail::Vector3(
				(vx * w2 + (q.y * vz - q.z * vy) * q.w + q.x * dot2),
				(vy * w2 + (q.z * vx - q.x * vz) * q.w + q.y * dot2),
				(vz * w2 + (q.x * vy - q.y * vx) * q.w + q.z * dot2));
		}

		template<typename T>
		inline detail::Vector3<T> rotateInv(const detail::Quaternion<T>& q, const detail::Vector3<T>& v) noexcept
		{
			const float vx = 2.0f * v.x;
			const float vy = 2.0f * v.y;
			const float vz = 2.0f * v.z;
			const float w2 = q.w * q.w - 0.5f;
			const float dot2 = (q.x * vx + q.y * vy + q.z * vz);
			return detail::Vector3(
				(vx * w2 - (q.y * vz - q.z * vy) * q.w + q.x * dot2),
				(vy * w2 - (q.z * vx - q.x * vz) * q.w + q.y * dot2),
				(vz * w2 - (q.x * vy - q.y * vx) * q.w + q.z * dot2));
		}

		template<typename T>
		inline detail::Quaternion<T> conjugate(const detail::Quaternion<T>& q) noexcept
		{
			return detail::Quaternion<T>(-q.x, -q.y, -q.z, q.w);
		}

		template<typename T>
		inline detail::Quaternion<T> inverse(const detail::Quaternion<T>& q) noexcept
		{
			return conjugate(q);
		}

		template<typename T>
		inline detail::Quaternion<T> pow(const detail::Quaternion<T>& q, T exponent) noexcept
		{
			if (std::fabs(q.w) > 0.9999f)
				return *q;

			T alpha = std::acos(q.w);
			T newAlpha = alpha * exponent;
			T mult = std::sin(newAlpha) / std::sin(alpha);

			detail::Quaternion<T> result;
			result.x = q.x * mult;
			result.y = q.y * mult;
			result.z = q.z * mult;
			result.w = std::cos(newAlpha);
			return *q;
		}

		template<typename T>
		inline detail::Quaternion<T> cross(const detail::Quaternion<T>& q1, const detail::Quaternion<T>& q2) noexcept
		{
			return detail::Quaternion<T>(
				q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
				q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
				q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
				q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
		}

		template<typename T>
		inline detail::Quaternion<T> normalize(const detail::Quaternion<T>& q) noexcept
		{
			detail::Quaternion<T> result = q;

			const T magSqrt = length2(q);
			if (magSqrt)
			{
				const T inv = (T)(1.0) / std::sqrt(magSqrt);
				result.x *= inv;
				result.y *= inv;
				result.z *= inv;
				result.w *= inv;
			}

			return result;
		}

		template<typename T>
		inline detail::Quaternion<T> slerp(const detail::Quaternion<T>& q1, const detail::Quaternion<T>& q2, T t) noexcept
		{
			if (t <= 0.0f) { return q1; }
			if (t >= 1.0f) { return q2; }

			T cosOmega = clamp(dot(q1, q2), -1.0f, 1.0f);

			if (cosOmega < 0.0f)
			{
				T theta = std::acos(-cosOmega);
				if (std::fabs(theta) < 1.0e-10)
				{
					return q1;
				}

				T st = std::sin(theta);
				T inv_st = 1.0f / st;
				T c0 = std::sin((1.0f - t) * theta) * inv_st;
				T c1 = std::sin(t * theta) * inv_st;

				detail::Quaternion<T> result;
				result.x = c0 * q1.x - c1 * q2.x;
				result.y = c0 * q1.y - c1 * q2.y;
				result.z = c0 * q1.z - c1 * q2.z;
				result.w = c0 * q1.w - c1 * q2.w;
				return result;
			}
			else
			{
				T theta = std::acos(cosOmega);
				if (fabs(theta) < 1.0e-10)
				{
					return q1;
				}

				T st = std::sin(theta);
				T inv_st = 1.0f / st;
				T c0 = std::sin((1.0f - t) * theta) * inv_st;
				T c1 = std::sin(t * theta) * inv_st;

				detail::Quaternion<T> result;
				result.x = c0 * q1.x + c1 * q2.x;
				result.y = c0 * q1.y + c1 * q2.y;
				result.z = c0 * q1.z + c1 * q2.z;
				result.w = c0 * q1.w + c1 * q2.w;
				return result;
			}
		}

		template<typename T>
		inline detail::Quaternion<T> min(const detail::Quaternion<T>& a, const detail::Quaternion<T>& b) noexcept
		{
			return detail::Quaternion<T>(std::min(a.w, b.w), std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
		}

		template<typename T>
		inline  detail::Quaternion<T> max(const detail::Quaternion<T>& a, const detail::Quaternion<T>& b) noexcept
		{
			return detail::Quaternion<T>(std::max(a.w, b.w), std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
		}
	}
}

#endif