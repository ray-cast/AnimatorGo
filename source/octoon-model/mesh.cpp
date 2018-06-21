#include <octoon/model/mesh.h>
#include <octoon/math/perlin_noise.h>

#include <map>
#include <cstring>

using namespace octoon::math;

namespace octoon
{
	namespace model
	{
		Mesh::Mesh() noexcept
		{
		}

		Mesh::Mesh(Mesh&& mesh) noexcept
			: _name(std::move(mesh._name))
			, _vertices(std::move(mesh._vertices))
			, _normals(std::move(mesh._normals))
			, _colors(std::move(mesh._colors))
			, _tangents(std::move(mesh._tangents))
			, _bindposes(std::move(mesh._bindposes))
			, _indices(std::move(mesh._indices))
			, _bones(std::move(mesh._bones))
			, _weights(std::move(mesh._weights))
			, _boundingBox(std::move(mesh._boundingBox))
		{
			for (int i = 0; i < TEXTURE_ARRAY_COUNT; ++i)
				_texcoords[i] = std::move(mesh._texcoords[i]);
		}

		Mesh::Mesh(const Mesh& mesh) noexcept
			: _name(mesh._name)
			, _vertices(mesh._vertices)
			, _normals(mesh._normals)
			, _colors(mesh._colors)
			, _tangents(mesh._tangents)
			, _bindposes(mesh._bindposes)
			, _indices(mesh._indices)
			, _bones(mesh._bones)
			, _weights(mesh._weights)
			, _boundingBox(mesh._boundingBox)
		{
			for (int i = 0; i < TEXTURE_ARRAY_COUNT; ++i)
				_texcoords[i] = mesh._texcoords[i];
		}

		Mesh::~Mesh() noexcept
		{
		}

		void
		Mesh::setName(std::string&& name) noexcept
		{
			_name = std::move(name);
		}

		void
		Mesh::setName(const std::string& name) noexcept
		{
			_name = name;
		}

		const std::string&
		Mesh::getName() const noexcept
		{
			return _name;
		}

		std::size_t
		Mesh::getNumVertices() const noexcept
		{
			return _vertices.size();
		}

		std::size_t
		Mesh::getNumIndices() const noexcept
		{
			return _indices.size();
		}

		std::size_t
		Mesh::getTexcoordNums() const noexcept
		{
			std::size_t count = 0;
			for (auto& it : _texcoords)
			{
				if (!it.empty())
					count++;
			}

			return count;
		}

		void
		Mesh::setVertexArray(const float3s& array) noexcept
		{
			_vertices = array;
		}

		void
		Mesh::setNormalArray(const float3s& array) noexcept
		{
			_normals = array;
		}

		void
		Mesh::setColorArray(const float4s& array) noexcept
		{
			_colors = array;
		}

		void
		Mesh::setTangentArray(const float4s& array) noexcept
		{
			_tangents = array;
		}

		void
		Mesh::setTexcoordArray(const float2s& array, std::uint8_t n) noexcept
		{
			assert(n < sizeof(_texcoords) / sizeof(float2s));
			_texcoords[n] = array;
		}

		void
		Mesh::setIndicesArray(const uint1s& array) noexcept
		{
			_indices = array;
		}

		void
		Mesh::setBindposes(const float4x4s& array) noexcept
		{
			_bindposes = array;
		}

		void
		Mesh::setWeightArray(const VertexWeights& array) noexcept
		{
			_weights = array;
		}

		void
		Mesh::setVertexArray(float3s&& array) noexcept
		{
			_vertices = std::move(array);
		}

		void
		Mesh::setNormalArray(float3s&& array) noexcept
		{
			_normals = std::move(array);
		}

		void
		Mesh::setColorArray(float4s&& array) noexcept
		{
			_colors = std::move(array);
		}

		void
		Mesh::setTangentArray(float4s&& array) noexcept
		{
			_tangents = std::move(array);
		}

		void
		Mesh::setTexcoordArray(float2s&& array, std::uint8_t n) noexcept
		{
			assert(n < sizeof(_texcoords) / sizeof(float2s));
			_texcoords[n] = std::move(array);
		}

		void
		Mesh::setIndicesArray(uint1s&& array) noexcept
		{
			_indices = std::move(array);
		}

		void
		Mesh::setWeightArray(VertexWeights&& array) noexcept
		{
			_weights = std::move(array);
		}

		void
		Mesh::setBindposes(float4x4s&& array) noexcept
		{
			_bindposes = std::move(array);
		}

		float3s&
		Mesh::getVertexArray() noexcept
		{
			return _vertices;
		}

		float3s&
		Mesh::getNormalArray() noexcept
		{
			return _normals;
		}

		float4s&
		Mesh::getTangentArray() noexcept
		{
			return _tangents;
		}

		float4s&
		Mesh::getColorArray() noexcept
		{
			return _colors;
		}

		float2s&
		Mesh::getTexcoordArray(std::uint8_t n) noexcept
		{
			assert(n < sizeof(_texcoords) / sizeof(float2s));
			return _texcoords[n];
		}

		VertexWeights&
		Mesh::getWeightArray() noexcept
		{
			return _weights;
		}

		uint1s&
		Mesh::getIndicesArray() noexcept
		{
			return _indices;
		}

		float4x4s&
		Mesh::getBindposes() noexcept
		{
			return _bindposes;
		}

		const float3s&
		Mesh::getVertexArray() const noexcept
		{
			return _vertices;
		}

		const float3s&
		Mesh::getNormalArray() const noexcept
		{
			return _normals;
		}

		const float4s&
		Mesh::getTangentArray() const noexcept
		{
			return _tangents;
		}

		const float4s&
		Mesh::getColorArray() const noexcept
		{
			return _colors;
		}

		const float2s&
		Mesh::getTexcoordArray(std::uint8_t n) const noexcept
		{
			assert(n < sizeof(_texcoords) / sizeof(float2s));
			return _texcoords[n];
		}

		const VertexWeights&
		Mesh::getWeightArray() const noexcept
		{
			return _weights;
		}

		const float4x4s&
		Mesh::getBindposes() const noexcept
		{
			return _bindposes;
		}

		const Bones&
		Mesh::getBoneArray(const Bones& array) const noexcept
		{
			return _bones;
		}

		const uint1s&
		Mesh::getIndicesArray() const noexcept
		{
			return _indices;
		}

		const BoundingBox&
		Mesh::getBoundingBox() const noexcept
		{
			return _boundingBox;
		}

		void
		Mesh::clear() noexcept
		{
			_vertices = float3s();
			_normals = float3s();
			_colors = float4s();
			_tangents = float4s();
			_indices = uint1s();

			for (std::size_t i = 0; i < 8; i++)
				_texcoords[i] = float2s();
		}

		MeshPtr
		Mesh::clone() noexcept
		{
			auto mesh = std::make_shared<Mesh>();
			mesh->setName(this->getName());
			mesh->setVertexArray(this->getVertexArray());
			mesh->setNormalArray(this->getNormalArray());
			mesh->setColorArray(this->getColorArray());
			mesh->setTexcoordArray(this->getTexcoordArray());
			mesh->setWeightArray(this->getWeightArray());
			mesh->setTangentArray(this->getTangentArray());
			mesh->setBindposes(this->getBindposes());
			mesh->setIndicesArray(this->getIndicesArray());
			mesh->_boundingBox = this->_boundingBox;

			return mesh;
		}

		void
		Mesh::makeCircle(float radius, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
		{
			this->clear();

			for (std::uint32_t i = 0; i <= segments; i++)
			{
				float3 v;

				float segment = thetaStart + (float)i / segments * thetaLength;

				v.x = radius * math::cos(segment);
				v.y = radius * math::sin(segment);
				v.z = 0;

				_vertices.push_back(v);

				_texcoords[0].emplace_back((v.x / radius + 1), (v.y / radius + 1) / 2);
			}

			for (std::uint32_t i = 1; i <= segments; i++)
			{
				std::uint32_t v1 = i;
				std::uint32_t v2 = i + 1;
				std::uint32_t v3 = 0;

				_indices.push_back(v1);
				_indices.push_back(v2);
				_indices.push_back(v3);

				_normals.push_back(float3::UnitZ);
				_normals.push_back(float3::UnitZ);
				_normals.push_back(float3::UnitZ);
			}

			this->computeBoundingBox();
		}

		void
		Mesh::makePlane(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
		{
			this->clear();

			float widthHalf = width * 0.5f;
			float heightHalf = height * 0.5f;

			std::uint32_t gridX = widthSegments;
			std::uint32_t gridY = heightSegments;

			std::uint32_t gridX1 = gridX + 1;
			std::uint32_t gridY1 = gridY + 1;

			float segmentWidth = width / gridX;
			float segmentHeight = height / gridY;

			const float3& normal = Vector3::UnitZ;

			for (std::uint32_t iz = 0; iz < gridY1; iz++)
			{
				for (std::uint32_t ix = 0; ix < gridX1; ix++)
				{
					float x = ix * segmentWidth - widthHalf;
					float z = iz * segmentHeight - heightHalf;

					_vertices.emplace_back(x, z, 0.0f);

					_normals.push_back(normal);
				}
			}

			for (std::uint32_t iy = 0; iy < gridY; iy++)
			{
				for (std::uint32_t ix = 0; ix < gridX; ix++)
				{
					_texcoords[0].emplace_back((float)ix / gridX, 1.0f - (float)(iy + 1) / gridY);
					_texcoords[0].emplace_back((float)(ix + 1) / gridX, 1.0f - (float)(iy + 1) / gridY);
					_texcoords[0].emplace_back((float)ix / gridX, 1.0f - (float)iy / gridY);
					_texcoords[0].emplace_back((float)(ix + 1) / gridX, 1.0f - (float)iy / gridY);

					std::int32_t a = static_cast<std::int32_t>(ix + gridX1 * iy);
					std::int32_t b = static_cast<std::int32_t>(ix + gridX1 * (iy + 1));
					std::int32_t c = static_cast<std::int32_t>(ix + gridX1 * (iy + 1) + 1);
					std::int32_t d = static_cast<std::int32_t>(ix + gridX1 * iy + 1);

					_indices.push_back(a);
					_indices.push_back(b);
					_indices.push_back(c);

					_indices.push_back(c);
					_indices.push_back(d);
					_indices.push_back(a);
				}
			}

			this->computeBoundingBox();
		}

		void
		Mesh::makePlane(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir, bool clear) noexcept
		{
			if (clear)
				this->clear();

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

			std::uint32_t offset = (std::uint32_t)_vertices.size();

			float segmentWidth = width / gridX;
			float segmentHeight = height / gridY;

			Vector3 n = Vector3::Zero;
			n.get(w) = depth >= 0.0f ? 1.0f : -1.0f;

			for (std::uint32_t iy = 0; iy < gridY1; iy++)
			{
				for (std::uint32_t ix = 0; ix < gridX1; ix++)
				{
					Vector3 vec;
					vec.get(u) = (ix * segmentWidth - widthHalf) * udir;
					vec.get(v) = (iy * segmentHeight - heightHalf) * vdir;
					vec.get(w) = depth;

					_vertices.push_back(vec);
					_normals.push_back(n);
					_texcoords[0].emplace_back((float)ix / gridX, 1.0f - (float)iy / gridY);
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

					_indices.push_back((std::int32_t)(a + offset));
					_indices.push_back((std::int32_t)(b + offset));
					_indices.push_back((std::int32_t)(c + offset));

					_indices.push_back((std::int32_t)(c + offset));
					_indices.push_back((std::int32_t)(d + offset));
					_indices.push_back((std::int32_t)(a + offset));
				}
			}
		}

		void
		Mesh::makePlaneWireframe(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir, bool clear) noexcept
		{
			if (clear)
				this->clear();

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

			std::uint32_t offset = (std::uint32_t)_vertices.size();

			float segmentWidth = width / gridX;
			float segmentHeight = height / gridY;

			Vector3 n = Vector3::Zero;
			n.get(w) = depth >= 0.0f ? 1.0f : -1.0f;

			for (std::uint32_t iy = 0; iy < gridY1; iy++)
			{
				for (std::uint32_t ix = 0; ix < gridX1; ix++)
				{
					Vector3 vec;
					vec.get(u) = (ix * segmentWidth - widthHalf) * udir;
					vec.get(v) = (iy * segmentHeight - heightHalf) * vdir;
					vec.get(w) = depth;

					_vertices.push_back(vec);
					_normals.push_back(n);
					_texcoords[0].emplace_back((float)ix / gridX, 1.0f - (float)iy / gridY);
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

					_indices.push_back((std::int32_t)(a + offset));
					_indices.push_back((std::int32_t)(b + offset));

					_indices.push_back((std::int32_t)(b + offset));
					_indices.push_back((std::int32_t)(c + offset));

					_indices.push_back((std::int32_t)(c + offset));
					_indices.push_back((std::int32_t)(d + offset));

					_indices.push_back((std::int32_t)(d + offset));
					_indices.push_back((std::int32_t)(a + offset));
				}
			}
		}

		void
		Mesh::makeFloor(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
		{
			this->clear();
			this->makePlane(width, height, 0, widthSegments, 0, heightSegments, 'x', 'z', 1.0, 1.0);

			this->computeTangents();
			this->computeBoundingBox();
		}

		void
		Mesh::makeNoise(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
		{
			this->clear();

			PerlinNoise2 PL;

			std::uint32_t gridX = widthSegments;
			std::uint32_t gridY = heightSegments;

			float widthHalf = width * 0.5f;
			float heightHalf = height * 0.5f;
			float widthSegment = width / widthSegments;
			float heightSegment = height / heightSegments;

			float invfre = 0.013f;

			auto makePosition = [&](float x, float y, float z) -> Vector3
			{
				float posX = -widthHalf + x * widthSegment;
				float posZ = -heightHalf + z * heightSegment;

				return Vector3(posX, y, posZ);
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

					_vertices.push_back(makePosition((float)x, accum, (float)y));
					_texcoords[0].push_back(Vector2((float)x, (float)y));
				}
			}

			for (std::uint32_t iy = 0; iy < gridX; iy++)
			{
				for (std::uint32_t ix = 0; ix < gridY; ix++)
				{
					_indices.push_back(ix + gridX1 * iy);
					_indices.push_back(ix + gridX1 * (iy + 1));
					_indices.push_back(ix + gridX1 * (iy + 1) + 1);

					_indices.push_back(ix + gridX1 * iy);
					_indices.push_back(ix + gridX1 * (iy + 1) + 1);
					_indices.push_back(ix + gridX1 * iy + 1);
				}
			}

			this->computeVertexNormals();
			this->computeTangents();
			this->computeBoundingBox();
		}

		void
		Mesh::makeCube(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments) noexcept
		{
			this->clear();

			float widthHalf = width * 0.5f;
			float heightHalf = height * 0.5f;
			float depthHalf = depth * 0.5f;

			this->makePlane(depth, height, widthHalf, widthSegments, heightSegments, depthSegments, 'z', 'y', -1, -1, false); // px
			this->makePlane(depth, height, -widthHalf, widthSegments, heightSegments, depthSegments, 'z', 'y', 1, -1, false); // nx
			this->makePlane(width, depth, heightHalf, widthSegments, heightSegments, depthSegments, 'x', 'z', 1, 1, false); // py
			this->makePlane(width, depth, -heightHalf, widthSegments, heightSegments, depthSegments, 'x', 'z', 1, -1, false); // ny
			this->makePlane(width, height, depthHalf, widthSegments, heightSegments, depthSegments, 'x', 'y', 1, -1, false); // pz
			this->makePlane(width, height, -depthHalf, widthSegments, heightSegments, depthSegments, 'x', 'y', -1, -1, false); // nz

			this->computeTangents();
			this->computeBoundingBox();
		}

		void
		Mesh::makeCubeWireframe(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments) noexcept
		{
			this->clear();

			float widthHalf = width * 0.5f;
			float heightHalf = height * 0.5f;
			float depthHalf = depth * 0.5f;

			this->makePlaneWireframe(depth, height, widthHalf, widthSegments, heightSegments, depthSegments, 'z', 'y', -1, -1, false); // px
			this->makePlaneWireframe(depth, height, -widthHalf, widthSegments, heightSegments, depthSegments, 'z', 'y', 1, -1, false); // nx
			this->makePlaneWireframe(width, depth, heightHalf, widthSegments, heightSegments, depthSegments, 'x', 'z', 1, 1, false); // py
			this->makePlaneWireframe(width, depth, -heightHalf, widthSegments, heightSegments, depthSegments, 'x', 'z', 1, -1, false); // ny
			this->makePlaneWireframe(width, height, depthHalf, widthSegments, heightSegments, depthSegments, 'x', 'y', 1, -1, false); // pz
			this->makePlaneWireframe(width, height, -depthHalf, widthSegments, heightSegments, depthSegments, 'x', 'y', -1, -1, false); // nz

			this->computeTangents();
			this->computeBoundingBox();
		}

		void
		Mesh::makeRing(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart, float thetaLength) noexcept
		{
			this->clear();

			innerRadius = innerRadius || 0;
			outerRadius = outerRadius || 50;

			thetaStart = thetaStart ? thetaStart : 0;
			thetaLength = thetaLength ? thetaLength : PI * 2;

			thetaSegments = thetaSegments ? std::max((std::uint32_t)3, thetaSegments) : 8;
			phiSegments = phiSegments ? std::max((std::uint32_t)3, phiSegments) : 8;

			for (size_t i = 0; i <= phiSegments; i++)
			{
				for (size_t j = 0; j <= thetaSegments; j++)
				{
				}
			}
		}

		void
		Mesh::makeSphere(float radius, std::uint32_t widthSegments, std::uint32_t heightSegments, float phiStart, float phiLength, float thetaStart, float thetaLength) noexcept
		{
			this->clear();

			std::vector<std::uint32_t> vertices;

			for (std::uint32_t y = 0; y <= heightSegments; y++)
			{
				for (std::uint32_t x = 0; x <= widthSegments; x++)
				{
					float u = (float)(x) / widthSegments;
					float v = (float)(y) / heightSegments;

					Vector3 vertex;
					vertex.x = -radius * math::sin(thetaStart + v * thetaLength) * math::cos(phiStart + u * phiLength);
					vertex.y = radius * math::cos(thetaStart + v * thetaLength);
					vertex.z = radius * math::sin(thetaStart + v * thetaLength) * math::sin(phiStart + u * phiLength);

					_vertices.push_back(vertex);
					_normals.push_back(math::normalize(vertex));
					_texcoords[0].emplace_back(u, v);

					vertices.push_back((std::uint32_t)_vertices.size() - 1);
				}
			}

			for (std::uint32_t y = 0; y < heightSegments; y++)
			{
				for (std::uint32_t x = 0; x < widthSegments; x++)
				{
					std::uint32_t v1 = vertices[y * (widthSegments + 1) + x];
					std::uint32_t v2 = vertices[y * (widthSegments + 1) + x + 1];
					std::uint32_t v3 = vertices[(y + 1) * (widthSegments + 1) + x];
					std::uint32_t v4 = vertices[(y + 1) * (widthSegments + 1) + x + 1];

					if (math::abs((_vertices)[v2].y) == radius)
					{
						_indices.push_back(v2);
						_indices.push_back(v3);
						_indices.push_back(v4);
					}
					else if (math::abs((_vertices)[v3].y) == radius)
					{
						_indices.push_back(v2);
						_indices.push_back(v1);
						_indices.push_back(v3);
					}
					else
					{
						_indices.push_back(v2);
						_indices.push_back(v3);
						_indices.push_back(v4);

						_indices.push_back(v2);
						_indices.push_back(v1);
						_indices.push_back(v3);
					}
				}
			}

			this->computeTangents();
			this->computeBoundingBox();
		}

		void
		Mesh::makeVolumes(float fovy, float znear, float zfar) noexcept
		{
			this->clear();

			float tanFovy_2 = math::tan(fovy * PI / 360.0f);

			_vertices.emplace_back(tanFovy_2 * znear, tanFovy_2 * znear, -znear);
			_vertices.emplace_back(-tanFovy_2 * znear, tanFovy_2 * znear, -znear);
			_vertices.emplace_back(-tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
			_vertices.emplace_back(tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);

			_vertices.emplace_back(tanFovy_2 * znear, -tanFovy_2 * znear, -znear);
			_vertices.emplace_back(tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);
			_vertices.emplace_back(-tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);
			_vertices.emplace_back(-tanFovy_2 * znear, -tanFovy_2 * znear, -znear);

			_vertices.emplace_back(tanFovy_2 * znear, -tanFovy_2 * znear, -znear);
			_vertices.emplace_back(tanFovy_2 * znear, tanFovy_2 * znear, -znear);
			_vertices.emplace_back(tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
			_vertices.emplace_back(tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);

			_vertices.emplace_back(-tanFovy_2 * znear, -tanFovy_2 * znear, -znear);
			_vertices.emplace_back(-tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);
			_vertices.emplace_back(-tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
			_vertices.emplace_back(-tanFovy_2 * znear, tanFovy_2 * znear, -znear);

			_vertices.emplace_back(-tanFovy_2 * znear, tanFovy_2 * znear, -znear);
			_vertices.emplace_back(tanFovy_2 * znear, tanFovy_2 * znear, -znear);
			_vertices.emplace_back(tanFovy_2 * znear, -tanFovy_2 * znear, -znear);
			_vertices.emplace_back(-tanFovy_2 * znear, -tanFovy_2 * znear, -znear);

			_vertices.emplace_back(tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
			_vertices.emplace_back(-tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
			_vertices.emplace_back(-tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);
			_vertices.emplace_back(tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);

			this->computeBoundingBox();
		}

		void
		Mesh::makeCone(float radius, float height, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
		{
			this->clear();

			_vertices.emplace_back(0.0f, 0.0f, 0.0f);
			_vertices.emplace_back(0.0f, 0.0f, -height);

			_normals.emplace_back(0.0f, 0.0f, 0.0f);
			_normals.emplace_back(0.0f, 0.0f, -1.0f);

			_texcoords[0].emplace_back(0.0f, 0.0f);
			_texcoords[0].emplace_back(1.0f, 1.0f);

			float segment = thetaLength / segments;

			for (std::uint32_t i = 0; i <= segments; i++)
			{
				float sin;
				float cos;

				math::sinCos(&sin, &cos, thetaStart + i * segment);

				Vector3 v;
				v.x = radius * cos;
				v.y = -radius * sin;
				v.z = 0;

				_vertices.push_back(v);
				_normals.push_back(math::normalize(v));

				_texcoords[0].emplace_back((v.x / radius + 1), (v.y / radius + 1) / 2);
			}

			for (std::uint32_t i = 2; i <= segments + 1; i++)
			{
				std::uint32_t v1 = i;
				std::uint32_t v2 = 0;
				std::uint32_t v3 = i + 1;

				_indices.push_back(v1);
				_indices.push_back(v2);
				_indices.push_back(v3);
			}

			for (std::uint32_t i = 2; i <= segments + 1; i++)
			{
				std::uint32_t v1 = i;
				std::uint32_t v2 = 1;
				std::uint32_t v3 = i + 1;

				_indices.push_back(v3);
				_indices.push_back(v2);
				_indices.push_back(v1);
			}

			this->computeTangents();
			this->computeBoundingBox();
		}

		bool
		Mesh::combineMeshes(const Mesh& mesh, bool force) noexcept
		{
			if (!force)
			{
				if (_vertices.empty() != mesh._vertices.empty()) return false;
				if (_normals.empty() != mesh._normals.empty()) return false;
				if (_colors.empty() != mesh._colors.empty()) return false;
				if (_tangents.empty() != mesh._tangents.empty()) return false;
				if (_bindposes.empty() != mesh._bindposes.empty()) return false;
				if (_indices.empty() != mesh._indices.empty()) return false;
				if (_bones.empty() != mesh._bones.empty()) return false;
				if (_weights.empty() != mesh._weights.empty()) return false;

				for (std::size_t i = 0; i < TEXTURE_ARRAY_COUNT; i++)
				{
					if (_texcoords[i].empty() != mesh._texcoords[i].empty())
						return false;
				}
			}

			_vertices.insert(_vertices.end(), mesh._vertices.begin(), mesh._vertices.end());
			_normals.insert(_normals.end(), mesh._normals.begin(), mesh._normals.end());
			_colors.insert(_colors.end(), mesh._colors.begin(), mesh._colors.end());
			_tangents.insert(_tangents.end(), mesh._tangents.begin(), mesh._tangents.end());
			_bindposes.insert(_bindposes.end(), mesh._bindposes.begin(), mesh._bindposes.end());
			_indices.insert(_indices.end(), mesh._indices.begin(), mesh._indices.end());
			_bones.insert(_bones.end(), mesh._bones.begin(), mesh._bones.end());
			_weights.insert(_weights.end(), mesh._weights.begin(), mesh._weights.end());

			for (std::size_t i = 0; i < TEXTURE_ARRAY_COUNT; i++)
				_texcoords[i].insert(_texcoords[i].end(), mesh._texcoords[i].begin(), mesh._texcoords[i].end());

			return true;
		}

		bool
		Mesh::combineMeshes(const CombineMesh instances[], std::size_t numInstance, bool merge) noexcept
		{
			this->clear();

			std::size_t maxIndices = 0;
			std::size_t maxVertices = 0;

			bool hasVertices = false;
			bool hasNormal = false;
			bool hasTangent = false;
			bool hasTexcoord[TEXTURE_ARRAY_COUNT] = { false };
			bool hasIndices = false;
			bool hasWeight = false;

			for (std::size_t i = 0; i < numInstance; i++)
			{
				auto mesh = instances[i].getMesh();
				if (!mesh)
					continue;

				maxVertices += mesh->getNumVertices();
				maxIndices += mesh->getNumIndices();

				hasVertices |= !mesh->getVertexArray().empty();
				hasNormal |= !mesh->getNormalArray().empty();
				hasTangent |= !mesh->getTangentArray().empty();
				hasIndices |= !mesh->getIndicesArray().empty();
				hasWeight |= !mesh->getWeightArray().empty();

				for (std::uint8_t j = 0; j < TEXTURE_ARRAY_COUNT; j++)
					hasTexcoord[j] |= !mesh->getTexcoordArray().empty();
			}

			for (std::size_t i = 0; i < numInstance; i++)
			{
				auto mesh = instances[i].getMesh();
				if (!mesh)
					continue;

				if (hasVertices) if (mesh->getVertexArray().empty()) return false;
				if (hasNormal)  if (mesh->getNormalArray().empty()) return false;
				if (hasTangent) if (mesh->getTangentArray().empty()) return false;
				if (hasIndices) if (mesh->getIndicesArray().empty()) return false;
				if (hasWeight) if (mesh->getWeightArray().empty()) return false;

				for (std::uint8_t j = 0; j < TEXTURE_ARRAY_COUNT; j++)
				{
					if (hasTexcoord[j])
					{
						if (mesh->getTexcoordArray(j).empty())
							return false;
					}
				}
			}

			if (hasVertices) this->_vertices.resize(maxVertices);
			if (hasNormal)   this->_normals.resize(maxVertices);
			if (hasTangent)  this->_tangents.resize(maxVertices);
			if (hasWeight)   this->_weights.resize(maxVertices);
			if (hasIndices)  this->_indices.resize(maxIndices);

			for (std::uint8_t i = 0; i < TEXTURE_ARRAY_COUNT; i++)
			{
				if (hasTexcoord[i])
					this->_texcoords[i].resize(maxVertices);
			}

			std::size_t offsetVertices = 0;
			std::size_t offsetIndices = 0;

			for (std::size_t i = 0; i < numInstance; i++)
			{
				auto mesh = instances[i].getMesh();
				if (!mesh)
					continue;

				if (hasVertices) std::memcpy(&_vertices[offsetVertices], mesh->_vertices.data(), mesh->_vertices.size());
				if (hasNormal)   std::memcpy(&_normals[offsetVertices], mesh->_normals.data(), mesh->_normals.size());
				if (hasTangent)  std::memcpy(&_tangents[offsetVertices], mesh->_tangents.data(), mesh->_tangents.size());
				if (hasWeight)   std::memcpy(&_weights[offsetVertices], mesh->_weights.data(), mesh->_weights.size());
				if (hasIndices)  std::memcpy(&_indices[offsetVertices], mesh->_indices.data(), mesh->_indices.size());

				for (std::uint8_t j = 0; j < TEXTURE_ARRAY_COUNT; j++)
				{
					if (hasTexcoord[j])
						std::memcpy(&_texcoords[j][offsetVertices], mesh->_texcoords[j].data(), mesh->_texcoords[j].size());
				}

				offsetVertices += mesh->getNumVertices();
				offsetIndices += mesh->getNumIndices();
			}

			this->computeBoundingBox();

			return true;
		}

		bool
		Mesh::combineMeshes(const CombineMeshes& instances, bool merge) noexcept
		{
			return this->combineMeshes(instances.data(), instances.size(), merge);
		}

		void
		Mesh::mergeVertices() noexcept
		{
			if (_vertices.empty())
				return;

			if (_normals.empty())
				this->computeVertexNormals();

			std::map<std::pair<float, float>, std::uint32_t> vectorMap;

			float3s changeVertex;
			float3s changeNormal;

			for (auto& it : _indices)
			{
				const Vector3& v = (_vertices)[it];
				const Vector3& n = (_normals)[it];

				float vkey = math::hash_float(v.x, v.y, v.z);
				float nkey = math::hash_float(n.z, n.y, n.x);

				std::uint32_t value = vectorMap[std::make_pair(vkey, nkey)];
				if (value == 0)
				{
					changeVertex.push_back(v);
					changeNormal.push_back(n);

					math::uint1 size = (math::uint1)changeVertex.size();
					it = size - 1;
					vectorMap[std::make_pair(vkey, nkey)] = size;
				}
				else
				{
					it = value - 1;
				}
			}

			_vertices.swap(changeVertex);
			_normals.swap(changeNormal);
		}

		void
		Mesh::computeFaceNormals(float3s& faceNormals) noexcept
		{
			assert(!_vertices.empty() && !_indices.empty());

			faceNormals.resize(_indices.size());

			std::size_t size = _indices.size();
			for (std::size_t i = 0; i < size; i += 3)
			{
				std::size_t f1 = _indices[i];
				std::size_t f2 = _indices[i + 1];
				std::size_t f3 = _indices[i + 2];

				const Vector3& a = _vertices[f1];
				const Vector3& b = _vertices[f2];
				const Vector3& c = _vertices[f3];

				Vector3 edge1 = c - b;
				Vector3 edge2 = a - b;

				Vector3 normal = math::normalize(math::cross(edge1, edge2));

				faceNormals[i] = normal;
				faceNormals[i + 1] = normal;
				faceNormals[i + 2] = normal;
			}
		}

		void
		Mesh::computeVertexNormals() noexcept
		{
			assert(!_vertices.empty());

			_normals.resize(_vertices.size());

			if (_indices.empty())
			{
				for (std::size_t i = 0; i < _vertices.size(); i += 3)
				{
					auto& a = _vertices[i];
					auto& b = _vertices[i + 1];
					auto& c = _vertices[i + 2];

					auto ab = a - b;
					auto ac = a - c;

					auto n = math::normalize(math::cross(ac, ab));

					_normals[i + 0] = n;
					_normals[i + 1] = n;
					_normals[i + 2] = n;
				}
			}
			else
			{
				std::memset(_normals.data(), 0, _normals.size() * sizeof(float3));

				for (std::size_t i = 0; i < _indices.size(); i += 3)
				{
					std::uint32_t f1 = (_indices)[i];
					std::uint32_t f2 = (_indices)[i + 1];
					std::uint32_t f3 = (_indices)[i + 2];

					auto& a = _vertices.at(f1);
					auto& b = _vertices.at(f2);
					auto& c = _vertices.at(f3);

					auto edge1 = c - b;
					auto edge2 = a - b;

					auto n(math::normalize(math::cross(edge1, edge2)));

					_normals[f1] += n;
					_normals[f2] += n;
					_normals[f3] += n;
				}

				for (auto& it : _normals)
					it = math::normalize(it);
			}
		}

		void
		Mesh::computeVertexNormals(const float3s& faceNormals) noexcept
		{
			assert(faceNormals.size() == _indices.size());
			assert(!_vertices.empty() && !_indices.empty());

			float3s normal;
			normal.resize(_vertices.size());
			std::memset(normal.data(), 0, normal.size() * sizeof(float3));

			std::size_t size = _indices.size();
			for (size_t i = 0; i < size; i += 3)
			{
				std::uint32_t a = (_indices)[i];
				std::uint32_t b = (_indices)[i + 1];
				std::uint32_t c = (_indices)[i + 2];

				normal[a] += faceNormals[i];
				normal[b] += faceNormals[i + 1];
				normal[c] += faceNormals[i + 2];
			}

			for (auto& it : normal)
				it = math::normalize(it);

			_normals.swap(normal);
		}

		void
		Mesh::computeVertexNormals(std::size_t width, std::size_t height) noexcept
		{
			Vector3 left;
			Vector3 right;
			Vector3 up;
			Vector3 down;

			auto getVertex = [&](std::size_t x, std::size_t y) -> Vector3
			{
				return (_vertices)[y * width + x];
			};

			for (std::size_t y = 0; y < width; ++y)
			{
				for (std::size_t x = 0; x < height; ++x)
				{
					Vector3 cur = getVertex(x, y);

					if (x > 0)
						left = getVertex(x - 1, y) - cur;

					if (x + 1 < width)
						right = getVertex(x + 1, y) - cur;

					if (y + 1 < height)
						up = getVertex(x, y + 1) - cur;

					if (y > 0)
						down = getVertex(x, y - 1) - cur;

					float3 lu = math::cross(left, up);
					float3 ru = math::cross(up, right);
					float3 rd = math::cross(right, down);
					float3 ld = math::cross(down, left);

					int average = 0;

					if (x > 0 && y > 0)
					{
						ld = math::normalize(ld);
						average++;
					}

					if (x > 0 && y + 1 < height)
					{
						lu = math::normalize(lu);
						average++;
					}

					if (y > 0 && x + 1 < width)
					{
						rd = math::normalize(rd);
						average++;
					}

					if (x + 1 < width && y + 1 < height)
					{
						ru = math::normalize(ru);
						average++;
					}

					_normals.push_back(math::normalize((lu + ru + ld + rd) / (float)average));
				}
			}
		}

		void
		Mesh::computeTangents(std::uint8_t n) noexcept
		{
			assert(!_texcoords[n].empty());

			float3s tan1(_vertices.size(), float3::Zero);
			float3s tan2(_vertices.size(), float3::Zero);

			std::size_t size = _indices.size();
			for (std::size_t i = 0; i < size; i += 3)
			{
				std::uint32_t f1 = (_indices)[i];
				std::uint32_t f2 = (_indices)[i + 1];
				std::uint32_t f3 = (_indices)[i + 2];

				auto& v1 = _vertices[f1];
				auto& v2 = _vertices[f2];
				auto& v3 = _vertices[f3];

				auto& w1 = _texcoords[n][f1];
				auto& w2 = _texcoords[n][f2];
				auto& w3 = _texcoords[n][f3];

				auto x1 = v2.x - v1.x;
				auto x2 = v3.x - v1.x;
				auto y1 = v2.y - v1.y;
				auto y2 = v3.y - v1.y;
				auto z1 = v2.z - v1.z;
				auto z2 = v3.z - v1.z;

				auto s1 = w2.x - w1.x;
				auto s2 = w3.x - w1.x;
				auto t1 = w2.y - w1.y;
				auto t2 = w3.y - w1.y;

				auto r = 1.0f / (s1 * t2 - s2 * t1);
				if (!std::isinf(r))
				{
					float3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
					float3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

					tan1[f1] += sdir;
					tan1[f2] += sdir;
					tan1[f3] += sdir;

					tan2[f1] += tdir;
					tan2[f2] += tdir;
					tan2[f3] += tdir;
				}
			}

			_tangents.resize(_normals.size());

			for (std::size_t i = 0; i < _normals.size(); i++)
			{
				auto& nor = _normals[i];
				auto& tan = tan1[i];

				float handedness = math::dot(math::cross(nor, tan), tan2[i]) < 0.0f ? 1.0f : -1.0f;

				_tangents[i] = float4(math::normalize(tan - nor * math::dot(nor, tan)), handedness);
			}
		}

		void
		Mesh::computeTangentQuats(float4s& tangentQuat) const noexcept
		{
			assert(_tangents.size() > 1);
			assert(_tangents.size() == _normals.size());

			tangentQuat.resize(_tangents.size());

			std::size_t numTangent = _tangents.size();
			for (std::size_t i = 0; i < numTangent; i++)
			{
				auto& normal = _normals[i];

				auto tangent = _tangents[i].xyz();
				auto binormal = math::cross(normal, tangent);

				Quaternion quat;
				quat.makeRotation(normal, binormal, tangent);

				if (quat.w < 0.0f)
					quat = -quat;

				if (_tangents[i].w < 0.0f)
					quat = -quat;

				tangentQuat[i].set(quat.x, quat.y, quat.z, quat.w);
			}
		}

		void
		Mesh::computeBoundingBox() noexcept
		{
			_boundingBox.reset();
			_boundingBox.encapsulate(_vertices.data(), _vertices.size());
		}
	}
}