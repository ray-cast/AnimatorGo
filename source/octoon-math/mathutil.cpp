#include <octoon/math/mathutil.h>
#include <time.h>

namespace octoon
{
	namespace math
	{
		void randomize() noexcept
		{
			srand((unsigned int)time(NULL));
		}

		void randomize(unsigned int seed) noexcept
		{
			srand(seed);
		}

		int random(int min, int max) noexcept
		{
			int seed = rand();
			return min + (seed ^ seed >> 15) % (max - min + 1);
		}

		float random(float min, float max) noexcept
		{
			int seed = rand();
			return min + (max - min) * (1.0f / RAND_MAX) * seed;
		}

		double random(double min, double max) noexcept
		{
			int seed = rand();
			return min + (max - min) * (1.0f / RAND_MAX) * seed;
		}

		std::uint32_t morton2(std::uint32_t x, std::uint32_t y) noexcept
		{
			static auto part1By1 = [](std::uint32_t n)
			{
				n = (n ^ (n << 8)) & 0x00ff00ff;
				n = (n ^ (n << 4)) & 0x0f0f0f0f;
				n = (n ^ (n << 2)) & 0x33333333;
				n = (n ^ (n << 1)) & 0x55555555;

				return n;
			};

			return (part1By1(y) << 1) + part1By1(x);
		}

		std::uint32_t morton3(std::uint32_t x, std::uint32_t y, std::uint32_t z) noexcept
		{
			static auto part1By2 = [](std::uint32_t n)
			{
				n = (n ^ (n << 16)) & 0xff0000ff;
				n = (n ^ (n << 8)) & 0x0300f00f;
				n = (n ^ (n << 4)) & 0x030c30c3;
				n = (n ^ (n << 2)) & 0x09249249;

				return n;
			};

			return (part1By2(z) << 2) + (part1By2(y) << 1) + part1By2(x);
		}
	}
}