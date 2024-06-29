#include <octoon/mesh/plane_mesh.h>

namespace octoon
{
	OctoonImplementSubClass(PlaneMesh, Mesh, "PlaneMesh");

	PlaneMesh::PlaneMesh() noexcept
		: PlaneMesh(10, 10)
	{
	}

	PlaneMesh::PlaneMesh(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments, bool wireframe) noexcept
	{
		std::vector<math::float3> vertices;
		std::vector<math::float3> normals;
		std::vector<math::float2> texcoords;

		std::uint32_t gridX = widthSegments;
		std::uint32_t gridY = heightSegments;

		std::uint32_t gridX1 = gridX + 1;
		std::uint32_t gridY1 = gridY + 1;

		float segmentWidth = width / gridX;
		float segmentHeight = height / gridY;

		float widthHalf = width * 0.5f;
		float heightHalf = height * 0.5f;

		const math::float3& normal = math::float3::UnitZ;

		for (std::uint32_t iz = 0; iz < gridY1; iz++)
		{
			for (std::uint32_t ix = 0; ix < gridX1; ix++)
			{
				float x = ix * segmentWidth - widthHalf;
				float z = iz * segmentHeight - heightHalf;

				vertices.emplace_back(x, z, 0.0f);
				normals.emplace_back(normal);
				texcoords.emplace_back((float)ix / gridX, (float)iz / gridY);
			}
		}

		math::uint1s indices;

		for (std::uint32_t iy = 0; iy < gridY; iy++)
		{
			for (std::uint32_t ix = 0; ix < gridX; ix++)
			{
				std::int32_t a = static_cast<std::int32_t>(ix + gridX1 * iy);
				std::int32_t b = static_cast<std::int32_t>(ix + gridX1 * (iy + 1));
				std::int32_t c = static_cast<std::int32_t>(ix + gridX1 * (iy + 1) + 1);
				std::int32_t d = static_cast<std::int32_t>(ix + gridX1 * iy + 1);

				if (wireframe) {
					indices.emplace_back(a);
					indices.emplace_back(b);

					indices.emplace_back(b);
					indices.emplace_back(c);

					indices.emplace_back(c);
					indices.emplace_back(d);

					indices.emplace_back(d);
					indices.emplace_back(a);
				}
				else
				{
					indices.emplace_back(a);
					indices.emplace_back(b);
					indices.emplace_back(c);

					indices.emplace_back(c);
					indices.emplace_back(d);
					indices.emplace_back(a);
				}
			}
		}

		this->setVertexArray(vertices);
		this->setNormalArray(normals);
		this->setTexcoordArray(texcoords);
		this->setIndicesArray(indices);

		if (!wireframe)
			this->computeTangents();

		this->computeBoundingBox();
	}
}