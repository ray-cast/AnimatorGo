#ifndef OCTOON_MATH_HAMMERSLEY_H_
#define OCTOON_MATH_HAMMERSLEY_H_

#include <octoon/math/vector2.h>

namespace octoon
{
	namespace math
	{
		uint1 ReverseBits32(uint1 bits)
		{
			bits = (bits << 16) | (bits >> 16);
			bits = ((bits & 0x00ff00ff) << 8) | ((bits & 0xff00ff00) >> 8);
			bits = ((bits & 0x0f0f0f0f) << 4) | ((bits & 0xf0f0f0f0) >> 4);
			bits = ((bits & 0x33333333) << 2) | ((bits & 0xcccccccc) >> 2);
			bits = ((bits & 0x55555555) << 1) | ((bits & 0xaaaaaaaa) >> 1);
			return bits;
		}

		template<typename T = float, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		detail::Vector2<T> Hammersley(uint1 i, uint1 samplesCount)
		{
			T E1 = (T)i / samplesCount;
			T E2 = ReverseBits32(i) * (T)2.3283064365386963e-10;
			return detail::Vector2<T>(E1, E2);
		}

		template<typename T = float, typename = std::enable_if_t<trait::is_floating_point_v<T>>>
		detail::Vector2<T> Hammersley(uint1 i, uint1 samplesCount, int2 random)
		{
			T E1 = fract((T)i / samplesCount + T(random.x & 0xffff) / (1 << 16));
			T E2 = T(ReverseBits32(i) ^ random.y) * (T)2.3283064365386963e-10;
			return detail::Vector2<T>(E1, E2);
		}
	}
}

#endif