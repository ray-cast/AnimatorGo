#ifndef OCTOON_VECTOR2_H_
#define OCTOON_VECTOR2_H_

#include <octoon/math/trait.h>
#include <octoon/math/mathfwd.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T>
			class Vector2 final
			{
			public:
				typedef typename trait::type_addition<T>::value_type value_type;
				typedef typename trait::type_addition<T>::pointer pointer;
				typedef typename trait::type_addition<T>::const_pointer const_pointer;
				typedef typename trait::type_addition<T>::reference reference;
				typedef typename trait::type_addition<T>::const_reference const_reference;

				T x, y;

				static const Vector2<T> Zero;
				static const Vector2<T> One;
				static const Vector2<T> UnitX;
				static const Vector2<T> UnitY;
				static const Vector2<T> Right;
				static const Vector2<T> Forward;

				Vector2() = default;
				Vector2(T xx, T yy) noexcept : x(xx), y(yy) {}
				explicit Vector2(T xy) noexcept : x(xy), y(xy) {}
				explicit Vector2(const T xy[2]) noexcept : x(xy[0]), y(xy[1]) {}
				~Vector2() = default;

				Vector2<T>& operator+=(T scale) noexcept { x += scale; y += scale; return *this; }
				Vector2<T>& operator-=(T scale) noexcept { x -= scale; y -= scale; return *this; }
				Vector2<T>& operator*=(T scale) noexcept { x *= scale; y *= scale; return *this; }
				Vector2<T>& operator/=(T scale) noexcept { x /= scale; y /= scale; return *this; }

				Vector2<T>& operator+=(const Vector2<T>& v) noexcept { x += v.x; y += v.y; return *this; }
				Vector2<T>& operator-=(const Vector2<T>& v) noexcept { x -= v.x; y -= v.y; return *this; }
				Vector2<T>& operator*=(const Vector2<T>& v) noexcept { x *= v.x; y *= v.y; return *this; }
				Vector2<T>& operator/=(const Vector2<T>& v) noexcept { x /= v.x; y /= v.y; return *this; }

				template<typename S, typename = std::enable_if_t<std::is_integral_v<S> || std::is_floating_point_v<S>>>
				explicit operator Vector2<S>() const noexcept
				{
					return Vector2<S>(static_cast<S>(x), static_cast<S>(y));
				}

				T operator[](std::uint8_t i) const noexcept { return *(&x + i); }
				reference operator[](std::uint8_t i) noexcept { return *(&x + i); }

				void set(T xx, T yy) noexcept { x = xx; y = yy; }
				void set(T xy[2]) noexcept { x = xy[0]; y = xy[1]; }
				void set(const Vector2<T>& v) noexcept { x = v.x; y = v.y; }

				constexpr Vector2<T> xx() const noexcept { return Vector2<T>(x, x); }
				constexpr Vector2<T> xy() const noexcept { return Vector2<T>(x, y); }
				constexpr Vector2<T> yx() const noexcept { return Vector2<T>(y, x); }
				constexpr Vector2<T> yy() const noexcept { return Vector2<T>(y, y); }
				constexpr Vector3<T> xxx() const noexcept { return Vector3<T>(x, x, x); }
				constexpr Vector3<T> xxy() const noexcept { return Vector3<T>(x, x, y); }
				constexpr Vector3<T> xyx() const noexcept { return Vector3<T>(x, y, x); }
				constexpr Vector3<T> xyy() const noexcept { return Vector3<T>(x, y, y); }
				constexpr Vector3<T> yxx() const noexcept { return Vector3<T>(y, x, x); }
				constexpr Vector3<T> yxy() const noexcept { return Vector3<T>(y, x, y); }
				constexpr Vector3<T> yyx() const noexcept { return Vector3<T>(y, y, x); }
				constexpr Vector3<T> yyy() const noexcept { return Vector3<T>(y, y, y); }
				constexpr Vector4<T> xxxx() const noexcept { return Vector4<T>(x, x, x, x); }
				constexpr Vector4<T> xxyx() const noexcept { return Vector4<T>(x, x, y, x); }
				constexpr Vector4<T> xyxx() const noexcept { return Vector4<T>(x, y, x, x); }
				constexpr Vector4<T> xyyx() const noexcept { return Vector4<T>(x, y, y, x); }
				constexpr Vector4<T> xxxy() const noexcept { return Vector4<T>(x, x, x, y); }
				constexpr Vector4<T> xxyy() const noexcept { return Vector4<T>(x, x, y, y); }
				constexpr Vector4<T> xyxy() const noexcept { return Vector4<T>(x, y, x, y); }
				constexpr Vector4<T> xyyy() const noexcept { return Vector4<T>(x, y, y, y); }
				constexpr Vector4<T> yxxx() const noexcept { return Vector4<T>(y, x, x, x); }
				constexpr Vector4<T> yxyx() const noexcept { return Vector4<T>(y, x, y, x); }
				constexpr Vector4<T> yyxx() const noexcept { return Vector4<T>(y, y, x, x); }
				constexpr Vector4<T> yyyx() const noexcept { return Vector4<T>(y, y, y, x); }
				constexpr Vector4<T> yxxy() const noexcept { return Vector4<T>(y, x, x, y); }
				constexpr Vector4<T> yxyy() const noexcept { return Vector4<T>(y, x, y, y); }
				constexpr Vector4<T> yyxy() const noexcept { return Vector4<T>(y, y, x, y); }
				constexpr Vector4<T> yyyy() const noexcept { return Vector4<T>(y, y, y, y); }

				constexpr reference get(std::uint8_t index) noexcept
				{
					switch (index)
					{
					case 0: return x;
					case 1: return y;
					case 'x': return x;
					case 'y': return y;
					default:
						assert(false);
					}
				}

				pointer ptr() noexcept { return (pointer)this; }
				const_pointer ptr() const noexcept { return (const_pointer)this; }
				pointer data() noexcept { return (pointer)this; }
				const_pointer data() const noexcept { return (const_pointer)this; }
			};

			template<typename T> const Vector2<T> Vector2<T>::Zero = Vector2<T>((T)0, (T)0);
			template<typename T> const Vector2<T> Vector2<T>::One = Vector2<T>((T)1, (T)1);
			template<typename T> const Vector2<T> Vector2<T>::UnitX = Vector2<T>((T)1, (T)0);
			template<typename T> const Vector2<T> Vector2<T>::UnitY = Vector2<T>((T)0, (T)1);
			template<typename T> const Vector2<T> Vector2<T>::Right = Vector2<T>((T)1, (T)0);
			template<typename T> const Vector2<T> Vector2<T>::Forward = Vector2<T>((T)0, (T)1);
		}

		template <typename T>
		inline bool operator==(const detail::Vector2<T>& v1, const detail::Vector2<T>& v2) noexcept
		{
			return v1.x == v2.x && v1.y == v2.y;
		}

		template <typename T>
		inline bool operator!=(const detail::Vector2<T>& v1, const detail::Vector2<T>& v2) noexcept
		{
			return !(v1 == v2);
		}

		template <typename T>
		inline detail::Vector2<T> operator-(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(-v.x, -v.y);
		}

		template <typename T>
		inline detail::Vector2<T> operator+(const detail::Vector2<T>& v1, const detail::Vector2<T>& v2) noexcept
		{
			return detail::Vector2<T>(v1.x + v2.x, v1.y + v2.y);
		}

		template <typename T>
		inline detail::Vector2<T> operator-(const detail::Vector2<T>& v1, const detail::Vector2<T>& v2) noexcept
		{
			return detail::Vector2<T>(v1.x - v2.x, v1.y - v2.y);
		}

		template <typename T>
		inline detail::Vector2<T> operator*(const detail::Vector2<T>& v1, const detail::Vector2<T>& v2) noexcept
		{
			return detail::Vector2<T>(v1.x * v2.x, v1.y * v2.y);
		}

		template <typename T>
		inline detail::Vector2<T> operator/(const detail::Vector2<T>& v1, const detail::Vector2<T>& v2) noexcept
		{
			return detail::Vector2<T>(v1.x / v2.x, v1.y / v2.y);
		}

		template <typename T>
		inline detail::Vector2<T> operator+(const detail::Vector2<T>& v, T scale) noexcept
		{
			return detail::Vector2<T>(v.x + scale, v.y + scale);
		}

		template <typename T>
		inline detail::Vector2<T> operator-(const detail::Vector2<T>& v, T scale) noexcept
		{
			return detail::Vector2<T>(v.x - scale, v.y - scale);
		}

		template <typename T>
		inline detail::Vector2<T> operator*(const detail::Vector2<T>& v, T scale) noexcept
		{
			return detail::Vector2<T>(v.x * scale, v.y * scale);
		}

		template <typename T>
		inline detail::Vector2<T> operator/(const detail::Vector2<T>& v, T scale) noexcept
		{
			return detail::Vector2<T>(v.x / scale, v.y / scale);
		}

		template <typename T>
		inline detail::Vector2<T> operator*(T scale, const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(scale * v.x, scale * v.y);
		}

		template <typename T>
		inline detail::Vector2<T> operator/(T scale, const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(scale / v.x, scale / v.y);
		}

		template<typename ostream, typename T, std::enable_if_t<trait::has_left_shift<ostream, T>::value, int> = 0>
		inline ostream& operator << (ostream& os, const detail::Vector2<T>& v)
		{
			os << v.x << ", " << v.y;
			return os;
		}

		template<typename istream, typename T, std::enable_if_t<trait::has_right_shift<istream>::value, int> = 0>
		inline istream& operator >> (istream& is, detail::Vector2<T>& v)
		{
			is >> v.x;
			is.ignore(2);
			is >> v.y;
			return is;
		}

		template<typename T>
		inline T dot(const detail::Vector2<T>& v1, const detail::Vector2<T>& v2) noexcept
		{
			return v1.x * v2.x + v1.y * v2.y;
		}

		template<typename T>
		inline T length2(const detail::Vector2<T>& v) noexcept
		{
			return dot(v, v);
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline T length(const detail::Vector2<T>& v) noexcept
		{
			return std::sqrt(length2(v));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline T distance(const detail::Vector2<T>& v1, const detail::Vector2<T>& v2) noexcept
		{
			return length(v1 - v2);
		}

		template<typename T>
		inline T sqr_distance(const detail::Vector2<T>& v1, const detail::Vector2<T>& v2) noexcept
		{
			return length2(v1 - v2);
		}

		template <typename T>
		inline T min(const detail::Vector2<T>& v) noexcept
		{
			return std::min(v.x, v.y);
		}

		template <typename T>
		inline T max(const detail::Vector2<T>& v) noexcept
		{
			return std::max(v.x, v.y);
		}

		template<typename T>
		inline bool equal(const detail::Vector2<T>& v1, const detail::Vector2<T>& v2) noexcept
		{
			return v1 == v2;
		}

		template<typename T>
		inline bool isfinite(const detail::Vector2<T>& v) noexcept
		{
			return std::isfinite(v.x) && std::isfinite(v.y);
		}

		template<typename T>
		inline detail::Vector2<T> cross(const detail::Vector2<T>& v1, const detail::Vector2<T>& v2) noexcept
		{
			return detail::Vector2<T>(
				v1.y * v2.x - v1.x * v2.y,
				v1.x * v2.y - v1.y * v2.x);
		}

		template<typename T>
		inline detail::Vector2<T> negate(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(-v.x, -v.y);
		}

		template<typename T>
		inline detail::Vector2<T> clamp(const detail::Vector2<T>& t, T min, T max) noexcept
		{
			return detail::Vector2<T>(
				std::max(min, std::min(max, t.x)),
				std::max(min, std::min(max, t.y))
				);
		}

		template<typename T>
		inline detail::Vector2<T> saturate(const detail::Vector2<T>& v) noexcept
		{
			return clamp(v, T(0), T(1));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> normalize(const detail::Vector2<T>& v) noexcept
		{
			T magSq = length2(v);
			if (magSq > 0.0f)
			{
				T invSqrt = 1.0f / std::sqrt(magSq);
				return v * invSqrt;
			}

			return v;
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> snorm2unorm(const detail::Vector2<T>& v) noexcept
		{
			return v * 0.5f + 0.5f;
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> unorm2snorm(const detail::Vector2<T>& v) noexcept
		{
			return v * 2.0f - 1.0f;
		}

		template <typename T>
		inline detail::Vector2<T> min(const detail::Vector2<T>& a, const detail::Vector2<T>& b) noexcept
		{
			return detail::Vector2<T>(std::min(a.x, b.x), std::min(a.y, b.y));
		}

		template <typename T>
		inline detail::Vector2<T> max(const detail::Vector2<T>& a, const detail::Vector2<T>& b) noexcept
		{
			return detail::Vector2<T>(std::max(a.x, b.x), std::max(a.y, b.y));
		}

		template<typename T>
		inline detail::Vector2<T> abs(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(std::abs(v.x), std::abs(v.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> cos(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(std::cos(v.x), std::cos(v.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> sin(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(std::sin(v.x), std::sin(v.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> tan(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(std::tan(v.x), std::tan(v.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> acos(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(std::acos(v.x), std::acos(v.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> asin(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(std::asin(v.x), std::asin(v.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> atan(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(std::atan(v.x), std::atan(v.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> exp(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(std::exp(v.x), std::exp(v.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> exp2(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(std::exp2(v.x), std::exp2(v.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> log(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(std::log(v.x), std::log(v.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> log2(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(std::log2(v.x), std::log2(v.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> log10(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(std::log10(v.x), std::log10(v.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> sqrt(const detail::Vector2<T>& v1) noexcept
		{
			return detail::Vector2<T>(std::sqrt(v1.x), std::sqrt(v1.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> pow(const detail::Vector2<T>& v1, T v2) noexcept
		{
			return detail::Vector2<T>(std::pow(v1.x, v2), std::pow(v1.y, v2));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> pow(const detail::Vector2<T>& v1, const detail::Vector2<T>& v2) noexcept
		{
			return detail::Vector2<T>(std::pow(v1.x, v2.x), std::pow(v1.y, v2.y));
		}

		template<typename T>
		inline detail::Vector2<T> random(const detail::Vector2<T>& min, const detail::Vector2<T>& max) noexcept
		{
			return detail::Vector2<T>(random(min.x, max.x), random(min.y, max.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> ceil(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(std::ceil(v.x), std::ceil(v.y));
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		inline detail::Vector2<T> floor(const detail::Vector2<T>& v) noexcept
		{
			return detail::Vector2<T>(std::floor(v.x), std::floor(v.y));
		}
	}
}

#endif