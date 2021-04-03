#include <octoon/mesh/torusknot_mesh.h>

namespace octoon
{
	OctoonImplementSubClass(TorusKnotMesh, Mesh, "TorusKnotMesh");

	TorusKnotMesh::TorusKnotMesh() noexcept
		: TorusKnotMesh(100, 40, 64, 8, 2, 3)
	{
	}

	TorusKnotMesh::TorusKnotMesh(float radius, float tube, std::uint32_t tubularSegments, std::uint32_t radialSegments, std::uint32_t p, std::uint32_t q) noexcept
	{
		std::vector<math::uint1> indices;
		std::vector<std::uint32_t> position;

		std::vector<math::float3> vertices;
		std::vector<math::float3> normals;
		std::vector<math::float2> texcoords;

		auto calculatePositionOnCurve = [](float u, std::uint32_t p, std::uint32_t q, float radius) {

			auto cu = std::cos(u);
			auto su = std::sin(u);
			auto quOverP = float(q) / p * u;
			auto cs = std::cos(quOverP);

			math::float3 position;
			position.x = radius * (2.f + cs) * 0.5f * cu;
			position.y = radius * (2.f + cs) * su * 0.5f;
			position.z = radius * std::sin(quOverP) * 0.5f;

			return position;
		};

		for (std::uint32_t i = 0; i <= tubularSegments; i++)
		{
			auto u = float(i) / tubularSegments * p * math::PI_2;

			auto P1 = calculatePositionOnCurve(u, p, q, radius);
			auto P2 = calculatePositionOnCurve(u + 0.01f, p, q, radius);

			auto T = P2 - P1;
			auto N = P2 + P1;
			auto B = math::cross(T, N);
			N = math::cross(B, T);

			B = math::normalize(B);
			N = math::normalize(N);

			for (std::uint32_t j = 0; j <= radialSegments; ++j)
			{
				auto v = float(j) / radialSegments * math::PI * 2;
				auto cx = -tube * std::cos(v);
				auto cy = tube * std::sin(v);

				math::float3 vertex;
				vertex.x = P1.x + (cx * N.x + cy * B.x);
				vertex.y = P1.y + (cx * N.y + cy * B.y);
				vertex.z = P1.z + (cx * N.z + cy * B.z);

				vertices.push_back(vertex);
				normals.push_back(math::normalize(vertex - P1));
				texcoords.push_back(math::float2(float(i) / tubularSegments, float(j) / radialSegments));
			}
		}

		for (std::uint32_t j = 1; j <= tubularSegments; j++)
		{
			for (std::uint32_t i = 1; i <= radialSegments; i++)
			{
				auto a = (radialSegments + 1) * (j - 1) + (i - 1);
				auto b = (radialSegments + 1) * j + (i - 1);
				auto c = (radialSegments + 1) * j + i;
				auto d = (radialSegments + 1) * (j - 1) + i;

				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(d);

				indices.push_back(b);
				indices.push_back(c);
				indices.push_back(d);
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