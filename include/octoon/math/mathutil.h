#ifndef OCTOON_MATHUTIL_H_
#define OCTOON_MATHUTIL_H_

#include <cmath>
#include <cfloat>
#include <cstdint>
#include <algorithm>
#include <octoon/runtime/platform.h>

namespace octoon
{
	namespace math
	{
		constexpr float PI = 3.14159265358979323846f;
		constexpr float PI_2 = 6.28318530717958647692f;
		constexpr float PI_4 = 12.56637061435917295384f;
		constexpr float PI_8 = 25.13274122871834590768f;
		constexpr float PI_OVER_3 = 1.047197551196597746f;
		constexpr float PI_INV = 0.318309886183790671538f;
		constexpr float PI_INV_2 = 0.636619772367581343076f;

		constexpr float EPSILON = 0.001f;
		constexpr float EPSILON_E2 = 1E-2f;
		constexpr float EPSILON_E3 = 1E-3f;
		constexpr float EPSILON_E4 = 1E-4f;
		constexpr float EPSILON_E5 = 1E-5f;
		constexpr float EPSILON_E6 = 1E-6f;

		template<typename T>
		constexpr auto wrap_pi(const T theta) noexcept
		{
			theta += math::PI;
			theta -= std::floor(theta * math::PI_2);
			theta -= math::PI;
			return theta;
		}

		template<typename T>
		constexpr T min(const T t1, const T t2) noexcept
		{
			return t1 < t2 ? t1 : t2;
		}

		template<typename T>
		constexpr T max(const T t1, const T t2) noexcept
		{
			return t1 > t2 ? t1 : t2;
		}

		template<typename T>
		constexpr T middle(const T t1, const T t2, const T t3) noexcept
		{
			if (t1 < t2)
			{
				if (t2 < t3)
					return t2;
				else
					return t3;
			}
			else
			{
				if (t1 < t3)
					return t1;
				else
					return t3;
			}
		}

		template<typename T>
		constexpr std::uint8_t bit_scan_reverse(T number) noexcept
		{
			std::uint8_t result = 0;
			T n = 1;

			for (; n < 32; n++)
			{
				T val = static_cast<T>(1) << n;

				result++;

				if (number & val)
					break;
			}

			return result;
		}

		template<typename T>
		constexpr T ceil_to_power_of_two(T number) noexcept
		{
			T input = number;
			T result = bit_scan_reverse(number);
			T test = ~(static_cast<T>(1) << result);

			return test & input;
		}

		template<typename T>
		constexpr T clamp(const T t, const T min, const T max) noexcept
		{
			return std::max(min, std::min(max, t));
		}

		template<typename T>
		constexpr T saturate(const T v) noexcept
		{
			return clamp(v, 0.0f, 1.0f);
		}

		template<typename _Tx, typename _Ty>
		constexpr _Tx lerp(const _Tx t1, const _Tx t2, const _Ty t3) noexcept
		{
			return t1 + (t2 - t1) * t3;
		}

		template<typename T>
		constexpr auto smoothlerp(const T x, const T x1, const T x2, const T q00, const T q01) noexcept
		{
			return ((x2 - x) / (x2 - x1)) * q00 + ((x - x1) / (x2 - x1)) * q01;
		}

		template<typename T>
		constexpr T biLerp(const T x, const T y, const T q11, const T q12, const T q21, const T q22, const T x1, const T x2, const T y1, const T y2) noexcept
		{
			T r1 = smoothlerp(x, x1, x2, q11, q21);
			T r2 = smoothlerp(x, x1, x2, q12, q22);

			return smoothlerp(y, y1, y2, r1, r2);
		}

		template<typename T>
		constexpr T triLerp(const T x, const T y, const T z, const T q000, const T q001, const T q010, const T q011, const T q100, const T q101, const T q110, const T q111, const T x1, const T x2, const T y1, const T y2, const T z1, const T z2) noexcept
		{
			T x00 = smoothlerp(x, x1, x2, q000, q100);
			T x10 = smoothlerp(x, x1, x2, q010, q110);
			T x01 = smoothlerp(x, x1, x2, q001, q101);
			T x11 = smoothlerp(x, x1, x2, q011, q111);
			T r0 = smoothlerp(y, y1, y2, x00, x01);
			T r1 = smoothlerp(y, y1, y2, x10, x11);

			return smoothlerp(z, z1, z2, r0, r1);
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
		constexpr bool equal(T a, T b, T epsilon = EPSILON) noexcept
		{
			return (a + epsilon) > b && (a - epsilon) < b;
		}

		template<typename T>
		constexpr auto radians(T x) noexcept
		{
			return (x * math::PI / 180.0f);
		}

		template<typename T>
		constexpr auto degress(T x) noexcept
		{
			return (x * 180.0f / math::PI);
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
		constexpr void sinCos(T* returnSin, T* returnCos, T theta) noexcept
		{
			*returnSin = std::sin(theta);
			*returnCos = std::cos(theta);
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
		constexpr T modf(T x, T* y) noexcept
		{
			T d;
			T f = std::modf(x, &d);
			*y = d;
			return f;
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
		constexpr T frac(T v) noexcept
		{
			T intPart;
			return std::modf(v, &intPart);
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
		constexpr T fract(T v) noexcept
		{
			T intPart;
			return std::modf(v, &intPart);
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
		constexpr T fraction(T v) noexcept
		{
			T intPart;
			return std::modf(v, &intPart);
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
		constexpr T safe_acos(T x) noexcept
		{
			if (x <= -1.0f) { return math::PI; }
			if (x >= 1.0f) { return 0.0f; }
			return std::acos(x);
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
		constexpr T snorm2unorm(T x) noexcept
		{
			return x * 0.5f + 0.5f;
		}

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
		constexpr T unorm2snorm(T x) noexcept
		{
			return x * 2.0f - 1.0f;
		}

		inline float hash_float(float x, float y, float z) noexcept
		{
			return x + y * (1.0f / 255.0f) + z * (1.0f / 65025.0f);
		}

		inline double hash_double(double x, double y, double z) noexcept
		{
			return x + y * (1.0f / 255.0f) + z * (1.0f / 65025.0f);
		}

		inline float abs(float v) noexcept
		{
			return std::abs(v);
		}

		inline float cos(float v) noexcept
		{
			return std::cos(v);
		}

		inline float sin(float v) noexcept
		{
			return std::sin(v);
		}

		inline float tan(float v) noexcept
		{
			return std::tan(v);
		}

		inline float acos(float v) noexcept
		{
			return std::acos(v);
		}

		inline float asin(float v) noexcept
		{
			return std::asin(v);
		}

		inline float atan(float v) noexcept
		{
			return std::atan(v);
		}

		inline float pow(float a, float b) noexcept
		{
			return std::pow(a, b);
		}

		inline float exp(float v) noexcept
		{
			return std::exp(v);
		}

		inline float exp2(float v) noexcept
		{
			return std::exp2(v);
		}

		inline float log2(float v) noexcept
		{
			return std::log2(v);
		}

		inline float log10(float v) noexcept
		{
			return std::log10(v);
		}

		inline float floor(float v) noexcept
		{
			return std::floor(v);
		}

		inline float ceil(float v) noexcept
		{
			return std::ceil(v);
		}

		inline float round(float v) noexcept
		{
			return std::round(v);
		}

		inline double cos(double v) noexcept
		{
			return std::cos(v);
		}

		inline double sin(double v) noexcept
		{
			return std::sin(v);
		}

		inline double tan(double v) noexcept
		{
			return std::tan(v);
		}

		inline double acos(double v) noexcept
		{
			return std::acos(v);
		}

		inline double asin(double v) noexcept
		{
			return std::asin(v);
		}

		inline double atan(double v) noexcept
		{
			return std::atan(v);
		}

		inline double pow(double a, double b) noexcept
		{
			return std::pow(a, b);
		}

		inline double exp(double v) noexcept
		{
			return std::exp(v);
		}

		inline double exp2(double v) noexcept
		{
			return std::exp2(v);
		}

		inline double log2(double v) noexcept
		{
			return std::log2(v);
		}

		inline double log10(double v) noexcept
		{
			return std::log10(v);
		}

		inline double floor(double v) noexcept
		{
			return std::floor(v);
		}

		inline double ceil(double v) noexcept
		{
			return std::ceil(v);
		}

		inline double round(double v) noexcept
		{
			return std::round(v);
		}

		inline float fast_exp2(float x) noexcept
		{
			constexpr float c[3] = { 5.79526f, 12.82461f, -2.88611f };

			int e = int(std::round(x));
			float t = x - e;
			float m = (t * t + c[0] * t + c[1]) / (c[2] * t * c[1]);

			return std::ldexp(m, e);
		}

		inline double fast_exp2(double x) noexcept
		{
			constexpr double c[3] = { 5.79526, 12.82461, -2.88611 };

			int e = int(std::round(x));
			double t = x - e;
			double m = (t * t + c[0] * t + c[1]) / (c[2] * t * c[1]);

			return std::ldexp(m, e);
		}

		constexpr float fast_rsqrt(float x) noexcept
		{
			float xhalf = 0.5f*x;
			int i = *(int*)&x;
			i = 0x5f3759df - (i >> 1);
			x = *(float*)&i;
			x = x * (1.5f - xhalf * x*x);
			x = x * (1.5f - xhalf * x*x);
			return x;
		}

		constexpr double fast_rsqrt(double y) noexcept
		{
			const double threehalfs = 1.5F;
			double x2 = y * 0.5F;
			long long i = *(long long*)&y;
			i = 0x5fe6ec85e7de30dall - (i >> 1);
			y = *(double*)&i;
			y = y * (threehalfs - (x2*y*y));
			y = y * (threehalfs - (x2*y*y));
			return y;
		}

		template<typename T>
		constexpr std::int8_t fpToInt8SNORM(T f) noexcept
		{
			return (std::int8_t)((1 << 7) * f);
		}

		template<typename T>
		constexpr std::uint8_t fpToInt8UNORM(T f) noexcept
		{
			return (std::uint8_t)(f * 255);
		}

		template<typename T>
		constexpr std::int16_t fpToInt16SNORM(T f) noexcept
		{
			return (std::int16_t)((1 << 15) * f);
		}

		template<typename T>
		constexpr std::uint16_t fpToInt16UNORM(T f) noexcept
		{
			return (std::uint16_t)(f * 65535);
		}

		constexpr float fpFromIEEE(std::uint32_t raw) noexcept
		{
			return *reinterpret_cast<float*>(&raw);
		}

		constexpr std::uint32_t fpToIEEE(float fp) noexcept
		{
			return *reinterpret_cast<std::uint32_t*>(&fp);
		}

		constexpr double fpFromIEEE(std::uint64_t raw) noexcept
		{
			return *reinterpret_cast<double*>(&raw);
		}

		constexpr std::uint64_t fpToIEEE(double fp) noexcept
		{
			return *reinterpret_cast<std::uint64_t*>(&fp);
		}

		OCTOON_EXPORT void randomize() noexcept;
		OCTOON_EXPORT void randomize(unsigned int) noexcept;
		OCTOON_EXPORT int random(int min, int max) noexcept;
		OCTOON_EXPORT float random(float min, float max) noexcept;
		OCTOON_EXPORT double random(double min, double max) noexcept;

		OCTOON_EXPORT std::uint32_t morton2(std::uint32_t x, std::uint32_t y) noexcept;
		OCTOON_EXPORT std::uint32_t morton3(std::uint32_t x, std::uint32_t y, std::uint32_t z) noexcept;
	}
}

#endif