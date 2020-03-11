#ifndef OCTOON_MATH_PERLIN_NOISE_H_
#define OCTOON_MATH_PERLIN_NOISE_H_

#include <octoon/math/vector2.h>

#include <cstdint>

namespace octoon
{
	namespace math
	{
		void simplex_seed(unsigned int x);

		float simplex2(float x, float y, int octaves, float persistence, float lacunarity);
		float simplex3(float x, float y, float z, int octaves, float persistence, float lacunarity);

		class PerlinNoise2
		{
		public:
			enum table
			{
				size = 256,
				mask = size - 1
			};

			PerlinNoise2() noexcept;

			float noise(float x, float y, float scale);
			float noise(const Vector2 pos, float scale);

		private:
			void setup() noexcept;
			const Vector2& getVec(int x, int y) const noexcept;

		private:
			Vector2 _table[size];
			std::uint8_t _lut[size];
		};
	}
}

#endif