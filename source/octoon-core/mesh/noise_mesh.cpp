#include <octoon/mesh/noise_mesh.h>
#include <octoon/math/perlin_noise.h>

namespace octoon
{
	OctoonImplementSubClass(NoiseMesh, Mesh, "NoiseMesh");

	NoiseMesh::NoiseMesh() noexcept
		: NoiseMesh(10, 10)
	{
	}

	NoiseMesh::NoiseMesh(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
	{
		std::vector<math::uint1> indices;

		std::vector<math::float3> vertices;
		std::vector<math::float3> normals;
		std::vector<math::float2> texcoords;

		math::PerlinNoise2 PL;

		std::uint32_t gridX = widthSegments;
		std::uint32_t gridY = heightSegments;

		float widthHalf = width * 0.5f;
		float heightHalf = height * 0.5f;
		float widthSegment = width / widthSegments;
		float heightSegment = height / heightSegments;

		float invfre = 0.013f;

		auto makePosition = [&](float x, float y, float z) -> math::Vector3
		{
			float posX = -widthHalf + x * widthSegment;
			float posZ = -heightHalf + z * heightSegment;

			return math::Vector3(posX, y, posZ);
		};

		std::uint32_t gridX1 = gridX + 1;
		std::uint32_t gridY1 = gridY + 1;

		for (std::uint32_t y = 0; y < gridX1; y++)
		{
			for (std::uint32_t x = 0; x < gridY1; x++)
			{
				float accum = 0;

				accum += PL.noise(x * invfre, y * invfre, 0.8f) * 1.0f;
				accum += PL.noise(x * invfre, y * invfre, 3.0f) * 0.17f;

				accum *= 0.1f;
				accum += 0.5f;

				accum *= width;

				accum -= heightHalf;

				vertices.push_back(makePosition((float)x, accum, (float)y));
				texcoords.push_back(math::Vector2((float)x, (float)y));
			}
		}

		for (std::uint32_t iy = 0; iy < gridX; iy++)
		{
			for (std::uint32_t ix = 0; ix < gridY; ix++)
			{
				indices.push_back(ix + gridX1 * iy);
				indices.push_back(ix + gridX1 * (iy + 1));
				indices.push_back(ix + gridX1 * (iy + 1) + 1);

				indices.push_back(ix + gridX1 * iy);
				indices.push_back(ix + gridX1 * (iy + 1) + 1);
				indices.push_back(ix + gridX1 * iy + 1);
			}
		}

		this->setIndicesArray(std::move(indices));

		this->computeVertexNormals();
		this->computeTangents();
		this->computeBoundingBox();
	}
}