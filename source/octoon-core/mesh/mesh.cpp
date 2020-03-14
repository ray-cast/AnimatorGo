#include <octoon/mesh/mesh.h>

#include <map>
#include <cstring>

using namespace octoon::math;

namespace octoon
{
	namespace mesh
	{
		OctoonImplementSubClass(Mesh, runtime::RttiInterface, "Mesh");

		Mesh::Mesh() noexcept
		{
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
		Mesh::getNumSubsets() const noexcept
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
		Mesh::setIndicesArray(const uint1s& array, std::size_t n) noexcept
		{
			if (_indices.size() <= n)
				_indices.resize(n + 1);
			_indices[n] = array;
		}

		void
		Mesh::setBindposes(const float4x4s& array) noexcept
		{
			_bindposes = array;
		}

		void
		Mesh::setWeightArray(const model::VertexWeights& array) noexcept
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
		Mesh::setIndicesArray(uint1s&& array, std::size_t n) noexcept
		{
			if (_indices.size() <= n)
				_indices.resize(n + 1);
			_indices[n] = std::move(array);
		}

		void
		Mesh::setWeightArray(model::VertexWeights&& array) noexcept
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

		model::VertexWeights&
		Mesh::getWeightArray() noexcept
		{
			return _weights;
		}

		uint1s&
		Mesh::getIndicesArray(std::size_t n) noexcept
		{
			return _indices[n];
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

		const model::VertexWeights&
		Mesh::getWeightArray() const noexcept
		{
			return _weights;
		}

		const float4x4s&
		Mesh::getBindposes() const noexcept
		{
			return _bindposes;
		}

		const model::Bones&
		Mesh::getBoneArray(const model::Bones& array) const noexcept
		{
			return _bones;
		}

		const uint1s&
		Mesh::getIndicesArray(std::size_t n) const noexcept
		{
			return _indices[n];
		}

		const BoundingBox&
		Mesh::getBoundingBox(std::size_t n) const noexcept
		{
			return _boundingBoxs[n];
		}

		const BoundingBox&
		Mesh::getBoundingBoxAll() const noexcept
		{
			return _boundingBox;
		}

		void
		Mesh::raycast(const math::Raycast& ray, std::vector<RaycastHit>& hits) noexcept
		{
			if (!math::intersect(ray, this->getBoundingBoxAll()))
				return;

			for (std::size_t i = 0; i < this->getNumSubsets(); i++)
			{
				if (math::intersect(ray, this->getBoundingBox(i)))
				{
					auto& indices = _indices[i];

					for (std::size_t j = 0; j < _indices[i].size(); j += 3)
					{
						std::size_t f1 = indices[j];
						std::size_t f2 = indices[j + 1];
						std::size_t f3 = indices[j + 2];

						auto& v0 = _vertices[f1];
						auto& v1 = _vertices[f2];
						auto& v2 = _vertices[f3];

						RaycastHit hit;
						if (math::intersect(ray, math::Triangle(v0, v1, v2), hit.point, hit.distance))
						{
							hit.mesh = i;
							hit.object = this;

							hits.emplace_back(hit);
						}
					}
				}
			}
		}

		void
		Mesh::clear() noexcept
		{
			_vertices.shrink_to_fit();
			_normals.shrink_to_fit();
			_colors.shrink_to_fit();
			_tangents.shrink_to_fit();

			for (auto& it : _indices)
				it.shrink_to_fit();

			for (auto& it : _texcoords)
				it.shrink_to_fit();
		}

		std::shared_ptr<Mesh>
		Mesh::clone() const noexcept
		{
			auto mesh = std::make_shared<Mesh>();
			mesh->setName(this->getName());
			mesh->setVertexArray(this->getVertexArray());
			mesh->setNormalArray(this->getNormalArray());
			mesh->setColorArray(this->getColorArray());
			mesh->setWeightArray(this->getWeightArray());
			mesh->setTangentArray(this->getTangentArray());
			mesh->setBindposes(this->getBindposes());
			mesh->_boundingBox = this->_boundingBox;
			mesh->_boundingBoxs = this->_boundingBoxs;

			for (std::size_t i = 0; i < TEXTURE_ARRAY_COUNT; i++)
				mesh->setTexcoordArray(this->getTexcoordArray());

			for (std::size_t i = 0; i < this->getNumSubsets(); i++)
				mesh->setIndicesArray(this->getIndicesArray(i), i);

			return mesh;
		}

		bool
		Mesh::mergeMeshes(const Mesh& mesh, bool force) noexcept
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
		Mesh::mergeMeshes(const mesh::CombineMesh instances[], std::size_t numInstance, bool merge) noexcept
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
			}

			this->computeBoundingBox();

			return true;
		}

		bool
		Mesh::mergeMeshes(const std::vector<CombineMesh>& instances, bool merge) noexcept
		{
			return this->mergeMeshes(instances.data(), instances.size(), merge);
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

			for (auto& indices : this->_indices)
			{
				for (auto& it : indices)
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
			}

			_vertices.swap(changeVertex);
			_normals.swap(changeNormal);
		}

		void
		Mesh::computeFaceNormals(std::vector<math::float3s>& faceNormals) noexcept
		{
			assert(!_vertices.empty());

			faceNormals.resize(_indices.size());

			for (std::size_t i = 0; i < _indices.size(); i += 3)
			{
				auto& indices = _indices[i];
				faceNormals[i].resize(indices.size());

				for (std::size_t j = 0; j < _indices[i].size(); j += 3)
				{
					std::size_t f1 = indices[j];
					std::size_t f2 = indices[j + 1];
					std::size_t f3 = indices[j + 2];

					const Vector3& a = _vertices[f1];
					const Vector3& b = _vertices[f2];
					const Vector3& c = _vertices[f3];

					Vector3 edge1 = c - b;
					Vector3 edge2 = a - b;

					Vector3 normal = math::normalize(math::cross(edge1, edge2));

					faceNormals[i][j] = normal;
					faceNormals[i][j + 1] = normal;
					faceNormals[i][j + 2] = normal;
				}
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

				for (auto& indices : _indices)
				{
					for (std::size_t i = 0; i < indices.size(); i += 3)
					{
						std::uint32_t f1 = indices[i];
						std::uint32_t f2 = indices[i + 1];
						std::uint32_t f3 = indices[i + 2];

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
				}

				for (auto& it : _normals)
					it = math::normalize(it);
			}
		}

		void
		Mesh::computeVertexNormals(std::size_t n) noexcept
		{
			auto& indices = _indices[n];
			
			for (auto& i : indices)
				_normals[i] = math::float3::Zero;

			for (std::size_t i = 0; i < indices.size(); i += 3)
			{
				std::uint32_t f1 = indices[i];
				std::uint32_t f2 = indices[i + 1];
				std::uint32_t f3 = indices[i + 2];

				auto& a = _vertices.at(f1);
				auto& b = _vertices.at(f2);
				auto& c = _vertices.at(f3);

				auto edge1 = c - b;
				auto edge2 = a - b;

				auto normal = math::normalize(math::cross(edge1, edge2));

				_normals[f1] += normal;
				_normals[f2] += normal;
				_normals[f3] += normal;
			}

			for (auto& i : indices)
				_normals[i] = math::normalize(_normals[i]);
		}

		void
		Mesh::computeVertexNormals(const float3s& faceNormals) noexcept
		{
			assert(faceNormals.size() == _indices.size());
			assert(!_vertices.empty() && !_indices.empty());

			float3s normal;
			normal.resize(_vertices.size());
			std::memset(normal.data(), 0, normal.size() * sizeof(float3));

			for (auto& indices : _indices)
			{
				for (size_t i = 0; i < indices.size(); i += 3)
				{
					auto a = indices[i];
					auto b = indices[i + 1];
					auto c = indices[i + 2];

					normal[a] += faceNormals[i];
					normal[b] += faceNormals[i + 1];
					normal[c] += faceNormals[i + 2];
				}
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

			for (auto& indices : _indices)
			{
				for (std::size_t i = 0; i < indices.size(); i += 3)
				{
					std::uint32_t f1 = indices[i];
					std::uint32_t f2 = indices[i + 1];
					std::uint32_t f3 = indices[i + 2];

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
			_boundingBoxs.resize(_indices.size());

			for (std::size_t i = 0; i < _indices.size(); i++)
			{
				auto& indices = _indices[i];
				for (auto& index : indices)
					_boundingBoxs[i].encapsulate(_vertices[index]);

				_boundingBox.encapsulate(_boundingBoxs[i]);
			}
		}
	}
}