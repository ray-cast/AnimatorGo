#ifndef OCTOON_MATH_MONTECARLO_H_
#define OCTOON_MATH_MONTECARLO_H_

#include <octoon/math/vector3.h>
#include <octoon/math/hammersley.h>

namespace octoon
{
	namespace math
	{
		template<typename T = float, typename = std::enable_if_t<std::is_floating_point<T>::value>>
		inline detail::Vector3<T> HammersleySampleCos(const detail::Vector2<T>& Xi)
		{
			T phi = 2 * PI * Xi.x;

			T cosTheta = std::sqrt(Xi.y);
			T sinTheta = std::sqrt(1 - cosTheta * cosTheta);

			detail::Vector3<T> H;
			H.x = sinTheta * std::cos(phi);
			H.y = sinTheta * std::sin(phi);
			H.z = cosTheta;

			return H;
		}

		template<typename T = float, typename = std::enable_if_t<std::is_floating_point<T>::value>>
		inline detail::Vector3<T> HammersleySampleGGX(const detail::Vector2<T>& Xi, T roughness)
		{
			T m = roughness * roughness;
			T m2 = m * m;
			T u = (1 - Xi.y) / (1 + (m2 - 1) * Xi.y);

			return HammersleySampleCos<T>(const detail::Vector2<T>(Xi.x, u));
		}

		template<typename T = float, typename = std::enable_if_t<std::is_floating_point<T>::value>>
		inline detail::Vector3<T> TangentToWorld(const detail::Vector3<T>& N, const detail::Vector3<T>& H)
		{
			detail::Vector3<T> Y = std::abs(N.z) < 0.999 ? detail::Vector3<T>::UnitZ : detail::Vector3<T>::UnitX;
			detail::Vector3<T> X = normalize(cross(Y, N));
			return X * H.x + cross(N, X) * H.y + N * H.z;
		}
	}
}

#endif