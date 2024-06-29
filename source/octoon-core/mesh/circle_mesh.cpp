#include <octoon/mesh/circle_mesh.h>

namespace octoon
{
	OctoonImplementSubClass(CircleMesh, Mesh, "CircleMesh");

	CircleMesh::CircleMesh() noexcept
		: CircleMesh(1)
	{
	}

	CircleMesh::CircleMesh(float radius, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
	{
		std::vector<math::uint1> indices;
		std::vector<std::uint32_t> position;

		std::vector<math::float3> vertices;
		std::vector<math::float3> normals;
		std::vector<math::float2> texcoords;

		for (std::uint32_t i = 0; i <= segments; i++)
		{
			float segment = thetaStart + (float)i / segments * thetaLength;

			math::float3 v;
			v.x = radius * math::cos(segment);
			v.y = radius * math::sin(segment);
			v.z = 0;

			vertices.emplace_back(v);
			texcoords.emplace_back((v.x / radius + 1), (v.y / radius + 1) / 2);
		}

		for (std::uint32_t i = 1; i <= segments; i++)
		{
			std::uint32_t v1 = i;
			std::uint32_t v2 = i + 1;
			std::uint32_t v3 = 0;

			indices.push_back(v1);
			indices.push_back(v2);
			indices.push_back(v3);

			normals.push_back(math::float3::UnitZ);
			normals.push_back(math::float3::UnitZ);
			normals.push_back(math::float3::UnitZ);
		}

		this->setVertexArray(std::move(vertices));
		this->setNormalArray(std::move(normals));
		this->setTexcoordArray(std::move(texcoords));
		this->setIndicesArray(std::move(indices));

		this->computeTangents();
		this->computeBoundingBox();
	}
}