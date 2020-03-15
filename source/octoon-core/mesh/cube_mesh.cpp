#include <octoon/mesh/cube_mesh.h>

namespace octoon::mesh
{
	OctoonImplementSubClass(CubeMesh, Mesh, "CubeMesh");

	CubeMesh::CubeMesh() noexcept
		: CubeMesh(1, 1, 1)
	{
	}

	CubeMesh::CubeMesh(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments) noexcept
	{
		float widthHalf = width * 0.5f;
		float heightHalf = height * 0.5f;
		float depthHalf = depth * 0.5f;

		std::vector<math::float3> vertices;
		std::vector<math::float3> normals;
		std::vector<math::float2> texcoords;

		std::vector<std::uint32_t> indices;

		auto makePlane = [&](float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir, bool clear) noexcept
		{
			std::uint32_t gridX = widthSegments;
			std::uint32_t gridY = heightSegments;

			float widthHalf = width * 0.5f;
			float heightHalf = height * 0.5f;

			std::uint8_t w = 0;
			if ((u == 'x' && v == 'y') || (u == 'y' && v == 'x'))
			{
				w = 'z';
			}
			else if ((u == 'x' && v == 'z') || (u == 'z' && v == 'x'))
			{
				w = 'y';
				gridY = depthSegments;
			}
			else if ((u == 'z' && v == 'y') || (u == 'y' && v == 'z'))
			{
				w = 'x';
				gridX = depthSegments;
			}
			else
			{
				assert(false);
			}

			std::uint32_t gridX1 = gridX + 1;
			std::uint32_t gridY1 = gridY + 1;

			std::uint32_t offset = (std::uint32_t)vertices.size();

			float segmentWidth = width / gridX;
			float segmentHeight = height / gridY;

			math::Vector3 n = math::Vector3::Zero;
			n.get(w) = depth >= 0.0f ? 1.0f : -1.0f;

			for (std::uint32_t iy = 0; iy < gridY1; iy++)
			{
				for (std::uint32_t ix = 0; ix < gridX1; ix++)
				{
					math::Vector3 vec;
					vec.get(u) = (ix * segmentWidth - widthHalf) * udir;
					vec.get(v) = (iy * segmentHeight - heightHalf) * vdir;
					vec.get(w) = depth;

					vertices.push_back(vec);
					normals.push_back(n);
					texcoords.emplace_back((float)ix / gridX, 1.0f - (float)iy / gridY);
				}
			}

			for (std::uint32_t iy = 0; iy < gridY; iy++)
			{
				for (std::uint32_t ix = 0; ix < gridX; ix++)
				{
					std::int32_t a = static_cast<std::int32_t>(ix + gridX1 * iy);
					std::int32_t b = static_cast<std::int32_t>(ix + gridX1 * (iy + 1));
					std::int32_t c = static_cast<std::int32_t>(ix + gridX1 * (iy + 1) + 1);
					std::int32_t d = static_cast<std::int32_t>(ix + gridX1 * iy + 1);

					indices.push_back((std::int32_t)(a + offset));
					indices.push_back((std::int32_t)(b + offset));
					indices.push_back((std::int32_t)(c + offset));

					indices.push_back((std::int32_t)(c + offset));
					indices.push_back((std::int32_t)(d + offset));
					indices.push_back((std::int32_t)(a + offset));
				}
			}
		};

		makePlane(depth, height, widthHalf, widthSegments, heightSegments, depthSegments, 'z', 'y', -1, -1, false); // px
		makePlane(depth, height, -widthHalf, widthSegments, heightSegments, depthSegments, 'z', 'y', 1, -1, false); // nx
		makePlane(width, depth, heightHalf, widthSegments, heightSegments, depthSegments, 'x', 'z', 1, 1, false); // py
		makePlane(width, depth, -heightHalf, widthSegments, heightSegments, depthSegments, 'x', 'z', 1, -1, false); // ny
		makePlane(width, height, depthHalf, widthSegments, heightSegments, depthSegments, 'x', 'y', 1, -1, false); // pz
		makePlane(width, height, -depthHalf, widthSegments, heightSegments, depthSegments, 'x', 'y', -1, -1, false); // nz

		this->setVertexArray(std::move(vertices));
		this->setNormalArray(std::move(normals));
		this->setTexcoordArray(std::move(texcoords));
		this->setIndicesArray(std::move(indices));

		this->computeTangents();
		this->computeBoundingBox();
	}
}