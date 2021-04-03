#include <octoon/mesh/cone_mesh.h>

namespace octoon
{
	OctoonImplementSubClass(ConeMesh, Mesh, "ConeMesh");

	ConeMesh::ConeMesh() noexcept
		: ConeMesh(1, 1)
	{
	}

	ConeMesh::ConeMesh(float radius, float height, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
	{
		std::vector<math::float3> vertices;
		std::vector<math::float3> normals;
		std::vector<math::float2> texcoords;

		vertices.emplace_back(0.0f, 0.0f, 0.0f);
		vertices.emplace_back(0.0f, 0.0f, -height);

		normals.emplace_back(0.0f, 0.0f, 0.0f);
		normals.emplace_back(0.0f, 0.0f, -1.0f);

		texcoords.emplace_back(0.0f, 0.0f);
		texcoords.emplace_back(1.0f, 1.0f);

		float segment = thetaLength / segments;

		for (std::uint32_t i = 0; i <= segments; i++)
		{
			float sin;
			float cos;

			math::sinCos(&sin, &cos, thetaStart + i * segment);

			math::Vector3 v;
			v.x = radius * cos;
			v.y = -radius * sin;
			v.z = 0;

			vertices.emplace_back(v);
			normals.emplace_back(math::normalize(v));
			texcoords.emplace_back((v.x / radius + 1), (v.y / radius + 1) / 2);
		}

		math::uint1s indices;

		for (std::uint32_t i = 2; i <= segments + 1; i++)
		{
			std::uint32_t v1 = i;
			std::uint32_t v2 = 0;
			std::uint32_t v3 = i + 1;

			indices.push_back(v1);
			indices.push_back(v2);
			indices.push_back(v3);
		}

		for (std::uint32_t i = 2; i <= segments + 1; i++)
		{
			std::uint32_t v1 = i;
			std::uint32_t v2 = 1;
			std::uint32_t v3 = i + 1;

			indices.push_back(v3);
			indices.push_back(v2);
			indices.push_back(v1);
		}

		this->setVertexArray(vertices);
		this->setNormalArray(normals);
		this->setTexcoordArray(texcoords);
		this->setIndicesArray(indices);

		this->computeTangents();
		this->computeBoundingBox();
	}
}