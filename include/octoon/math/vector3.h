#ifndef OCTOON_MATH_VECTOR3_H_
#define OCTOON_MATH_VECTOR3_H_

#include <octoon/math/trait.h>
#include <octoon/math/mathfwd.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T>
			class Vector3 final
			{
			public:
				typedef typename trait::type_addition<T>::value_type value_type;
				typedef typename trait::type_addition<T>::pointer pointer;
				typedef typename trait::type_addition<T>::const_pointer const_pointer;
				typedef typename trait::type_addition<T>::reference reference;
				typedef typename trait::type_addition<T>::const_reference const_reference;

				T x, y, z;

				static const Vector3<T> Zero;
				static const Vector3<T> One;
				static const Vector3<T> UnitX;
				static const Vector3<T> UnitY;
				static const Vector3<T> UnitZ;
				static const Vector3<T> Right;
				static const Vector3<T> Up;
				static const Vector3<T> Forward;

				Vector3() = default;
				Vector3(T xx, T yy, T zz) noexcept :x(xx), y(yy), z(zz) {}
				Vector3(T xx, const Vector2<T>& yz) noexcept : x(xx), y(yz.x), z(yz.y) {}
				Vector3(const Vector2<T>& xy, T zz) noexcept : x(xy.x), y(xy.y), z(zz) {}
				explicit Vector3(const T xyz[3]) noexcept : x(xyz[0]), y(xyz[1]), z(xyz[2]) {}
				explicit Vector3(T xyz) noexcept : x(xyz), y(xyz), z(xyz) {}
				~Vector3() = default;

#pragma warning(push)
#pragma warning(disable : 4244) // float multiply with double
				template<typename S, typename = std::enable_if_t<trait::is_same_v<S, T> || (!trait::is_same_v<S, T> && trait::is_floating_point_v<S>)>>
				Vector3<T>& operator+=(const S sz) noexcept { x += sz; y += sz; z += sz; return *this; }

				template<typename S, typename = std::enable_if_t<trait::is_same_v<S, T> || (!trait::is_same_v<S, T> && trait::is_floating_point_v<S>)>>
				Vector3<T>& operator-=(const S sz) noexcept { x -= sz; y -= sz; z -= sz; return *this; }

				template<typename S, typename = std::enable_if_t<trait::is_same_v<S, T> || (!trait::is_same_v<S, T> && trait::is_floating_point_v<S>)>>
				Vector3<T>& operator*=(const S sz) noexcept { x *= sz; y *= sz; z *= sz; return *this; }

				template<typename S, typename = std::enable_if_t<trait::is_same_v<S, T> || (!trait::is_same_v<S, T> && trait::is_floating_point_v<S>)>>
				Vector3<T>& operator/=(const S sz) noexcept { x /= sz; y /= sz; z /= sz; return *this; }

				template<typename S, typename = std::enable_if_t<trait::is_same_v<S, T> || (!trait::is_same_v<S, T> && trait::is_floating_point_v<S>)>>
				Vector3<T>& operator+=(const Vector2<S>& rt) noexcept { x += rt.x; y += rt.y; return *this; }

				template<typename S, typename = std::enable_if_t<trait::is_same_v<S, T> || (!trait::is_same_v<S, T> && trait::is_floating_point_v<S>)>>
				Vector3<T>& operator-=(const Vector2<S>& rt) noexcept { x -= rt.x; y -= rt.y; return *this; }

				template<typename S, typename = std::enable_if_t<trait::is_same_v<S, T> || (!trait::is_same_v<S, T> && trait::is_floating_point_v<S>)>>
				Vector3<T>& operator/=(const Vector2<S>& rt) noexcept { x *= rt.x; y *= rt.y; return *this; }

				template<typename S, typename = std::enable_if_t<trait::is_same_v<S, T> || (!trait::is_same_v<S, T> && trait::is_floating_point_v<S>)>>
				Vector3<T>& operator*=(const Vector2<S>& rt) noexcept { x /= rt.x; y /= rt.y; return *this; }

				template<typename S, typename = std::enable_if_t<trait::is_same_v<S, T> || (!trait::is_same_v<S, T> && trait::is_floating_point_v<S>)>>
				Vector3<T>& operator+=(const Vector3<S>& pt) noexcept { x += pt.x; y += pt.y; z += pt.z; return *this; }

				template<typename S, typename = std::enable_if_t<trait::is_same_v<S, T> || (!trait::is_same_v<S, T> && trait::is_floating_point_v<S>)>>
				Vector3<T>& operator-=(const Vector3<S>& pt) noexcept { x -= pt.x; y -= pt.y; z -= pt.z; return *this; }

				template<typename S, typename = std::enable_if_t<trait::is_same_v<S, T> || (!trait::is_same_v<S, T> && trait::is_floating_point_v<S>)>>
				Vector3<T>& operator*=(const Vector3<S>& pt) noexcept { x *= pt.x; y *= pt.y; z *= pt.z; return *this; }

				template<typename S, typename = std::enable_if_t<trait::is_same_v<S, T> || (!trait::is_same_v<S, T> && trait::is_floating_point_v<S>)>>
				Vector3<T>& operator/=(const Vector3<S>& pt) noexcept { x /= pt.x; y /= pt.y; z /= pt.z; return *this; }
#pragma warning(pop)

				template<typename S, typename = std::enable_if_t<trait::is_integral_v<S> || trait::is_floating_point_v<S>>>
				explicit operator Vector3<S>() const noexcept
				{
					return Vector3<S>(static_cast<S>(x), static_cast<S>(y), static_cast<S>(z));
				}

				constexpr Vector2<T> xx()  const noexcept { return Vector2<T>(x, x); }
				constexpr Vector2<T> xy()  const noexcept { return Vector2<T>(x, y); }
				constexpr Vector2<T> xz()  const noexcept { return Vector2<T>(x, z); }
				constexpr Vector2<T> yx()  const noexcept { return Vector2<T>(y, x); }
				constexpr Vector2<T> yy()  const noexcept { return Vector2<T>(y, y); }
				constexpr Vector2<T> yz()  const noexcept { return Vector2<T>(y, z); }
				constexpr Vector2<T> zx()  const noexcept { return Vector2<T>(z, x); }
				constexpr Vector2<T> zy()  const noexcept { return Vector2<T>(z, y); }
				constexpr Vector2<T> zz()  const noexcept { return Vector2<T>(z, z); }
				constexpr Vector3<T> xxx() const noexcept { return Vector3<T>(x, x, x); }
				constexpr Vector3<T> xxy() const noexcept { return Vector3<T>(x, x, y); }
				constexpr Vector3<T> xxz() const noexcept { return Vector3<T>(x, x, z); }
				constexpr Vector3<T> xyx() const noexcept { return Vector3<T>(x, y, x); }
				constexpr Vector3<T> xyy() const noexcept { return Vector3<T>(x, y, y); }
				constexpr Vector3<T> xyz() const noexcept { return Vector3<T>(x, y, z); }
				constexpr Vector3<T> xzx() const noexcept { return Vector3<T>(x, z, x); }
				constexpr Vector3<T> xzy() const noexcept { return Vector3<T>(x, z, y); }
				constexpr Vector3<T> xzz() const noexcept { return Vector3<T>(x, z, z); }
				constexpr Vector3<T> yxx() const noexcept { return Vector3<T>(y, x, x); }
				constexpr Vector3<T> yxy() const noexcept { return Vector3<T>(y, x, y); }
				constexpr Vector3<T> yxz() const noexcept { return Vector3<T>(y, x, z); }
				constexpr Vector3<T> yyx() const noexcept { return Vector3<T>(y, y, x); }
				constexpr Vector3<T> yyy() const noexcept { return Vector3<T>(y, y, y); }
				constexpr Vector3<T> yyz() const noexcept { return Vector3<T>(y, y, z); }
				constexpr Vector3<T> yzx() const noexcept { return Vector3<T>(y, z, x); }
				constexpr Vector3<T> yzy() const noexcept { return Vector3<T>(y, z, y); }
				constexpr Vector3<T> yzz() const noexcept { return Vector3<T>(y, z, z); }
				constexpr Vector3<T> zxx() const noexcept { return Vector3<T>(z, x, x); }
				constexpr Vector3<T> zxy() const noexcept { return Vector3<T>(z, x, y); }
				constexpr Vector3<T> zxz() const noexcept { return Vector3<T>(z, x, z); }
				constexpr Vector3<T> zyx() const noexcept { return Vector3<T>(z, y, x); }
				constexpr Vector3<T> zyy() const noexcept { return Vector3<T>(z, y, y); }
				constexpr Vector3<T> zyz() const noexcept { return Vector3<T>(z, y, z); }
				constexpr Vector3<T> zzx() const noexcept { return Vector3<T>(z, z, x); }
				constexpr Vector3<T> zzy() const noexcept { return Vector3<T>(z, z, y); }
				constexpr Vector3<T> zzz() const noexcept { return Vector3<T>(z, z, z); }

				constexpr T operator[](std::uint8_t i) const noexcept
				{
					assert(i < 3);
					return *(&x + i);
				}

				constexpr T& operator[](std::uint8_t i) noexcept
				{
					assert(i < 3);
					return *(&x + i);
				}

				Vector3<T>& set(T val) noexcept { x = y = z = val; return *this; }
				Vector3<T>& set(T xx, T yy, T zz) noexcept { x = xx; y = yy; z = zz; return *this; }
				Vector3<T>& set(T xyz[3]) noexcept { x = xyz[0]; y = xyz[1]; z = xyz[2]; return *this; }
				Vector3<T>& set(T xx, const Vector2<T>& yz) noexcept { x = xx; y = yz.x; z = yz.y; return *this; }
				Vector3<T>& set(const Vector2<T>& xy, T zz) noexcept { x = xy.x; y = xy.y; z = zz; return *this; }

				constexpr reference get(std::uint8_t index) noexcept
				{
					switch (index)
					{
					case 0 : return x;
					case 1 : return y;
					case 2 : return z;
					case 'x': return x;
					case 'y': return y;
					case 'z': return z;
					default:
						assert(false);
					}
					return x;
				}

				pointer ptr() noexcept { return (pointer)this; }
				const_pointer ptr() const noexcept { return (const_pointer)this; }
				pointer data() noexcept { return (pointer)this; }
				const_pointer data() const noexcept { return (const_pointer)this; }

			public:
				friend bool operator==(const Vector3<T>& v1, const Vector3<T>& v2) noexcept
				{
					return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
				}

				friend bool operator!=(const Vector3<T>& v1, const Vector3<T>& v2) noexcept
				{
					return !(v1 == v2);
				}

				friend bool operator==(const T value, const Vector3<T>& v) noexcept
				{
					return v.x == value && v.y == value && v.z == value;
				}

				friend bool operator!=(const T value, const Vector3<T>& v) noexcept
				{
					return v.x != value && v.y != value && v.z != value;
				}

				friend bool operator==(const Vector3<T>& v, const T value) noexcept
				{
					return v.x == value && v.y == value && v.z == value;
				}

				friend bool operator!=(const Vector3<T>& v, const T value) noexcept
				{
					return v.x != value || v.y != value || v.z != value;
				}

				friend bool operator<(const Vector3<T>& v1, const Vector3<T>& v2) noexcept
				{
					return v1.x < v2.x && v1.y < v2.y && v1.z < v2.z;
				}

				friend bool operator>(const Vector3<T>& v1, const Vector3<T>& v2) noexcept
				{
					return v1.x > v2.x && v1.y > v2.y && v1.z > v2.z;
				}

				friend bool operator<=(const Vector3<T>& v1, const Vector3<T>& v2) noexcept
				{
					return v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z;
				}

				friend bool operator>=(const Vector3<T>& v1, const Vector3<T>& v2) noexcept
				{
					return v1.x >= v2.x && v1.y >= v2.y && v1.z >= v2.z;
				}

				friend bool operator<(const Vector3<T>& v, const T value) noexcept
				{
					return v.x < value && v.y < value && v.z < value;
				}

				friend bool operator>(const Vector3<T>& v, const T value) noexcept
				{
					return v.x > value && v.y > value && v.z > value;
				}

				friend bool operator<=(const Vector3<T>& v, const T value) noexcept
				{
					return v.x <= value && v.y <= value && v.z <= value;
				}

				friend bool operator>=(const Vector3<T>& v, const T value) noexcept
				{
					return v.x >= value && v.y >= value && v.z >= value;
				}

				friend bool operator<(const T value, const Vector3<T>& v) noexcept
				{
					return value < v.x && value < v.y && value < v.z;
				}

				friend bool operator>(const T value, const Vector3<T>& v) noexcept
				{
					return value > v.x && value > v.y && value > v.z;
				}

				friend bool operator<=(const T value, const Vector3<T>& v) noexcept
				{
					return value <= v.x && value <= v.y && value <= v.z;
				}

				friend bool operator>=(const T value, const Vector3<T>& v) noexcept
				{
					return value >= v.x && value >= v.y && value >= v.z;
				}

				friend Vector3<T> operator-(const Vector3<T>& v) noexcept
				{
					return Vector3<T>(-v.x, -v.y, -v.z);
				}

				friend Vector3<T> operator+(const Vector3<T>& v1, const Vector3<T>& v2) noexcept
				{
					return Vector3<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
				}

				friend Vector3<T> operator-(const Vector3<T>& v1, const Vector3<T>& v2) noexcept
				{
					return Vector3<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
				}

				friend Vector3<T> operator*(const Vector3<T>& v1, const Vector3<T>& v2) noexcept
				{
					return Vector3<T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
				}

				friend Vector3<T> operator/(const Vector3<T>& v1, const Vector3<T>& v2) noexcept
				{
					return Vector3<T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
				}

				friend Vector3<T> operator+(const Vector3<T>& v, const T value) noexcept
				{
					return Vector3<T>((v.x + value), (v.y + value), (v.z + value));
				}

				friend Vector3<T> operator-(const Vector3<T>& v, const T value) noexcept
				{
					return Vector3<T>((v.x - value), (v.y - value), (v.z - value));
				}

				friend Vector3<T> operator*(const Vector3<T>& v, const T value) noexcept
				{
					return Vector3<T>((v.x * value), (v.y * value), (v.z * value));
				}

				friend Vector3<T> operator/(const Vector3<T>& v, const T value) noexcept
				{
					return Vector3<T>((v.x / value), (v.y / value), (v.z / value));
				}

				friend Vector3<T> operator+(const T value, const Vector3<T>& v) noexcept
				{
					return Vector3<T>((value + v.x), (value + v.y), (value + v.z));
				}

				friend Vector3<T> operator-(const T value, const Vector3<T>& v) noexcept
				{
					return Vector3<T>((value - v.x), (value - v.y), (value - v.z));
				}

				friend Vector3<T> operator*(const T value, const Vector3<T>& v) noexcept
				{
					return Vector3<T>((value * v.x), (value * v.y), (value * v.z));
				}

				friend Vector3<T> operator/(const T value, const Vector3<T>& v) noexcept
				{
					return Vector3<T>((value / v.x), (value / v.y), (value / v.z));
				}

				template<typename ostream, std::enable_if_t<trait::has_left_shift<ostream, T>::value, int> = 0>
				friend ostream& operator << (ostream& os, const Vector3<T>& v)
				{
					os << v.x << ", " << v.y << ", " << v.z;
					return os;
				}

				template<typename istream, std::enable_if_t<trait::has_right_shift<istream>::value, int> = 0>
				friend istream& operator >> (istream& is, Vector3<T>& v)
				{
					is >> v.x;
					is.ignore(2);
					is >> v.y;
					is.ignore(2);
					is >> v.z;
					return is;
				}
			};

			template<typename T> const Vector3<T> Vector3<T>::Zero = Vector3<T>((T)0, (T)0, (T)0);
			template<typename T> const Vector3<T> Vector3<T>::One = Vector3<T>((T)1, (T)1, (T)1);
			template<typename T> const Vector3<T> Vector3<T>::UnitX = Vector3<T>((T)1, (T)0, (T)0);
			template<typename T> const Vector3<T> Vector3<T>::UnitY = Vector3<T>((T)0, (T)1, (T)0);
			template<typename T> const Vector3<T> Vector3<T>::UnitZ = Vector3<T>((T)0, (T)0, (T)1);
			template<typename T> const Vector3<T> Vector3<T>::Right = Vector3<T>((T)1, (T)0, (T)0);
			template<typename T> const Vector3<T> Vector3<T>::Up = Vector3<T>((T)0, (T)1, (T)0);
			template<typename T> const Vector3<T> Vector3<T>::Forward = Vector3<T>((T)0, (T)0, (T)1);
		}

		template<typename T>
		inline T dot(const detail::Vector3<T>& v1, const detail::Vector3<T>& v2) noexcept
		{
			return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		}

		template<typename T>
		inline detail::Vector3<T> cross(const detail::Vector3<T>& v1, const detail::Vector3<T>& v2) noexcept
		{
			return detail::Vector3<T>(
				v1.y * v2.z - v1.z * v2.y,
				v1.z * v2.x - v1.x * v2.z,
				v1.x * v2.y - v1.y * v2.x
				);
		}

		template<typename T>
		inline T length2(const detail::Vector3<T>& v) noexcept
		{
			return dot(v, v);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline T length(const detail::Vector3<T>& v) noexcept
		{
			return std::sqrt(length2(v));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline T distance(const detail::Vector3<T>& v1, const detail::Vector3<T>& v2) noexcept
		{
			return length(v1 - v2);
		}

		template<typename T>
		inline T sqrDistance(const detail::Vector3<T>& v1, const detail::Vector3<T>& v2) noexcept
		{
			return length2(v1 - v2);
		}

		template <typename T>
		inline T min3(const detail::Vector3<T>& v) noexcept
		{
			return std::min(std::min(v.x, v.y), v.z);
		}

		template <typename T>
		inline T max3(const detail::Vector3<T>& v) noexcept
		{
			return std::max(std::max(v.x, v.y), v.z);
		}

		template<typename T>
		inline bool equal(const detail::Vector3<T>& v1, const detail::Vector3<T>& v2) noexcept
		{
			return v1 == v2;
		}

		template<typename T>
		inline bool any(const detail::Vector3<T>& v) noexcept
		{
			return v.x != 0.0f || v.y != 0.0f || v.z != 0.0f;
		}

		template<typename T>
		inline bool all(const detail::Vector3<T>& v) noexcept
		{
			return !(v.x == 0.0f && v.y == 0.0f && v.z == 0.0f);
		}

		template<typename T>
		inline bool isfinite(const detail::Vector3<T>& v) noexcept
		{
			return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
		}

		template<typename T>
		inline detail::Vector3<T> negate(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(-v.x, -v.y, -v.z);
		}

		template<typename T>
		inline detail::Vector3<T> clamp(const detail::Vector3<T>& t, T min, T max) noexcept
		{
			return detail::Vector3<T>(
				std::max(min, std::min(max, t.x)),
				std::max(min, std::min(max, t.y)),
				std::max(min, std::min(max, t.z))
				);
		}

		template<typename T>
		inline detail::Vector3<T> saturate(const detail::Vector3<T>& v) noexcept
		{
			return clamp(v, T(0), T(1));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> normalize(const detail::Vector3<T>& v) noexcept
		{
			T magSq = length2(v);
			if (magSq > 0.0f)
			{
				T invSqrt = 1.0f / std::sqrt(magSq);
				return v * invSqrt;
			}

			return v;
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> snorm2unorm(const detail::Vector3<T>& v) noexcept
		{
			return v * T(0.5f) + T(0.5f);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> unorm2snorm(const detail::Vector3<T>& v) noexcept
		{
			return v * T(2.0f) - T(1.0f);
		}

		template <typename T>
		inline detail::Vector3<T> min(const detail::Vector3<T>& a, const detail::Vector3<T>& b) noexcept
		{
			return detail::Vector3<T>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
		}

		template <typename T>
		inline detail::Vector3<T> max(const detail::Vector3<T>& a, const detail::Vector3<T>& b) noexcept
		{
			return detail::Vector3<T>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
		}

		template<typename T>
		inline detail::Vector3<T> abs(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> cos(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::cos(v.x), std::cos(v.y), std::cos(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> sin(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::sin(v.x), std::sin(v.y), std::sin(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> tan(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::tan(v.x), std::tan(v.y), std::tan(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> acos(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::acos(v.x), std::acos(v.y), std::acos(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> asin(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::asin(v.x), std::asin(v.y), std::asin(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> atan(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::atan(v.x), std::atan(v.y), std::atan(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> exp(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::exp(v.x), std::exp(v.y), std::exp(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> exp2(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::exp2(v.x), std::exp2(v.y), std::exp2(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> sqrt(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> log(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::log(v.x), std::log(v.y), std::log(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> log2(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::log2(v.x), std::log2(v.y), std::log2(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> log10(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::log10(v.x), std::log10(v.y), std::log10(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> pow(const detail::Vector3<T>& v1, T v2) noexcept
		{
			return detail::Vector3<T>(std::pow(v1.x, v2), std::pow(v1.y, v2), std::pow(v1.z, v2));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> pow(const detail::Vector3<T>& v1, const detail::Vector3<T>& v2) noexcept
		{
			return detail::Vector3<T>(std::pow(v1.x, v2.x), std::pow(v1.y, v2.y), std::pow(v1.z, v2.z));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
		inline detail::Vector3<T> reflect(const detail::Vector3<T>& I, const detail::Vector3<T>& N) noexcept
		{
			return I - 2 * (dot(I, N) * N);
		}

		template<typename T>
		inline detail::Vector3<T> random(const detail::Vector3<T>& min, const detail::Vector3<T>& max) noexcept
		{
			return detail::Vector3<T>(random(min.x, max.x), random(min.y, max.y), random(min.z, max.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> random_in_sphere() noexcept
		{
			float z = 2.0f * std::rand() / RAND_MAX - 1.0f;
			float t = 2.0f * std::rand() / RAND_MAX * PI;
			float r = std::sqrt(1.0f - z * z);
			return detail::Vector3<T>(r * std::cos(t), r * std::sin(t), z);
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> ceil(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::ceil(v.x), std::ceil(v.y), std::ceil(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> floor(const detail::Vector3<T>& v) noexcept
		{
			return detail::Vector3<T>(std::floor(v.x), std::floor(v.y), std::floor(v.z));
		}

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> linear2srgb(const detail::Vector3<T>& srgb) noexcept
		{
			const float ALPHA = 0.055f;
			return detail::Vector3<T>(
				srgb.x < 0.0031308f ? 12.92f * srgb.x : (1 + ALPHA) * math::pow(srgb.x, 1 / 2.4f) - ALPHA,
				srgb.y < 0.0031308f ? 12.92f * srgb.y : (1 + ALPHA) * math::pow(srgb.y, 1 / 2.4f) - ALPHA,
				srgb.z < 0.0031308f ? 12.92f * srgb.z : (1 + ALPHA) * math::pow(srgb.z, 1 / 2.4f) - ALPHA);
		};

		template<typename T, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		inline detail::Vector3<T> srgb2linear(const detail::Vector3<T>& rgb) noexcept
		{
			const float ALPHA = 0.055f;
			return detail::Vector3<T>(
				rgb.x < 0.04045f ? rgb.x / 12.92f : math::pow((rgb.x + ALPHA) / (1 + ALPHA), 2.4f),
				rgb.y < 0.04045f ? rgb.y / 12.92f : math::pow((rgb.y + ALPHA) / (1 + ALPHA), 2.4f),
				rgb.z < 0.04045f ? rgb.z / 12.92f : math::pow((rgb.z + ALPHA) / (1 + ALPHA), 2.4f));
		}
	}
}

#endif