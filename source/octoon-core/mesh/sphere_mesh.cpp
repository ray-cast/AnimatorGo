#include <octoon/mesh/sphere_mesh.h>

namespace octoon
{
	OctoonImplementSubClass(SphereMesh, Mesh, "SphereMesh");

	SphereMesh::SphereMesh() noexcept
		: SphereMesh(1)
	{
	}

	SphereMesh::SphereMesh(float radius, std::uint32_t widthSegments, std::uint32_t heightSegments, float phiStart, float phiLength, float thetaStart, float thetaLength) noexcept
	{
		std::vector<math::uint1> indices;
		std::vector<std::uint32_t> position;

		std::vector<math::float3> vertices;
		std::vector<math::float3> normals;
		std::vector<math::float2> texcoords;

		for (std::uint32_t y = 0; y <= heightSegments; y++)
		{
			for (std::uint32_t x = 0; x <= widthSegments; x++)
			{
				float u = (float)(x) / widthSegments;
				float v = (float)(y) / heightSegments;

				math::Vector3 vertex;
				vertex.x = radius * math::sin(thetaStart + v * thetaLength) * math::cos(phiStart + u * phiLength);
				vertex.y = radius * math::cos(thetaStart + v * thetaLength);
				vertex.z = radius * math::sin(thetaStart + v * thetaLength) * math::sin(phiStart + u * phiLength);

				vertices.push_back(vertex);
				normals.push_back(-math::normalize(vertex));
				texcoords.emplace_back(u, v);

				position.push_back((std::uint32_t)vertices.size() - 1);
			}
		}

		for (std::uint32_t y = 0; y < heightSegments; y++)
		{
			for (std::uint32_t x = 0; x < widthSegments; x++)
			{
				std::uint32_t v1 = position[y * (widthSegments + 1) + x];
				std::uint32_t v2 = position[y * (widthSegments + 1) + x + 1];
				std::uint32_t v3 = position[(y + 1) * (widthSegments + 1) + x];
				std::uint32_t v4 = position[(y + 1) * (widthSegments + 1) + x + 1];

				if (math::abs((vertices)[v2].y) == radius)
				{
					indices.push_back(v2);
					indices.push_back(v3);
					indices.push_back(v4);
				}
				else if (math::abs((vertices)[v3].y) == radius)
				{
					indices.push_back(v2);
					indices.push_back(v1);
					indices.push_back(v3);
				}
				else
				{
					indices.push_back(v2);
					indices.push_back(v3);
					indices.push_back(v4);

					indices.push_back(v2);
					indices.push_back(v1);
					indices.push_back(v3);
				}
			}
		}

		this->setVertexArray(std::move(vertices));
		this->setNormalArray(std::move(normals));
		this->setTexcoordArray(std::move(texcoords));
		this->setIndicesArray(std::move(indices));

		this->computeTangents();
		this->computeBoundingBox();
	}
}