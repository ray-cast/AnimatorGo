#include <octoon/mesh/mesh.h>
#include <octoon/lightmap/lightmap_pack.h>

#include <map>
#include <cstring>
#include <iostream>

using namespace octoon::math;

namespace octoon
{
	OctoonImplementSubClass(Mesh, Object, "Mesh");

	Mesh::Mesh() noexcept
		: dirty_(true)
	{
	}

	Mesh::~Mesh() noexcept
	{
	}

	void
	Mesh::setName(std::string_view name) noexcept
	{
		name_ = name;
	}

	const std::string&
	Mesh::getName() const noexcept
	{
		return name_;
	}

	std::size_t
	Mesh::getNumVertices() const noexcept
	{
		return vertices_.size();
	}

	std::size_t
	Mesh::getNumIndices() const noexcept
	{
		std::size_t count = 0;

		for (auto& it : triangles_)
			count += it.size();

		return count;
	}

	std::size_t
	Mesh::getNumSubsets() const noexcept
	{
		return triangles_.size();
	}

	std::size_t
	Mesh::getTexcoordNums() const noexcept
	{
		std::size_t count = 0;
		for (auto& it : texcoords_)
		{
			if (!it.empty())
				count++;
		}

		return count;
	}

	void
	Mesh::setVertexArray(const float3s& array) noexcept
	{
		vertices_ = array;
	}

	void
	Mesh::setNormalArray(const float3s& array) noexcept
	{
		normals_ = array;
	}

	void
	Mesh::setColorArray(const float4s& array) noexcept
	{
		colors_ = array;
	}

	void
	Mesh::setTangentArray(const float4s& array) noexcept
	{
		tangents_ = array;
	}

	void
	Mesh::setTexcoordArray(const float2s& array, std::uint8_t n) noexcept
	{
		assert(n < sizeof(texcoords_) / sizeof(float2s));
		texcoords_[n] = array;
	}

	void
	Mesh::setIndicesArray(const uint1s& array, std::size_t n) noexcept
	{
		if (triangles_.size() <= n)
			triangles_.resize(n + 1);
		triangles_[n] = array;
	}

	void
	Mesh::setBindposes(const float4x4s& array) noexcept
	{
		bindposes_ = array;
	}

	void
	Mesh::setWeightArray(const std::vector<VertexWeight>& array) noexcept
	{
		weights_ = array;
	}

	void
	Mesh::setVertexArray(float3s&& array) noexcept
	{
		vertices_ = std::move(array);
	}

	void
	Mesh::setNormalArray(float3s&& array) noexcept
	{
		normals_ = std::move(array);
	}

	void
	Mesh::setColorArray(float4s&& array) noexcept
	{
		colors_ = std::move(array);
	}

	void
	Mesh::setTangentArray(float4s&& array) noexcept
	{
		tangents_ = std::move(array);
	}

	void
	Mesh::setTexcoordArray(float2s&& array, std::uint8_t n) noexcept
	{
		assert(n < sizeof(texcoords_) / sizeof(float2s));
		texcoords_[n] = std::move(array);
	}

	void
	Mesh::setIndicesArray(uint1s&& array, std::size_t n) noexcept
	{
		if (triangles_.size() <= n)
			triangles_.resize(n + 1);
		triangles_[n] = std::move(array);
	}

	void
	Mesh::setWeightArray(std::vector<VertexWeight>&& array) noexcept
	{
		weights_ = std::move(array);
	}

	void
	Mesh::setBindposes(float4x4s&& array) noexcept
	{
		bindposes_ = std::move(array);
	}

	float3s&
	Mesh::getVertexArray() noexcept
	{
		return vertices_;
	}

	float3s&
	Mesh::getNormalArray() noexcept
	{
		return normals_;
	}

	float4s&
	Mesh::getTangentArray() noexcept
	{
		return tangents_;
	}

	float4s&
	Mesh::getColorArray() noexcept
	{
		return colors_;
	}

	float2s&
	Mesh::getTexcoordArray(std::uint8_t n) noexcept
	{
		assert(n < sizeof(texcoords_) / sizeof(float2s));
		return texcoords_[n];
	}

	std::vector<VertexWeight>&
	Mesh::getWeightArray() noexcept
	{
		return weights_;
	}

	uint1s&
	Mesh::getIndicesArray(std::size_t n) noexcept
	{
		return triangles_[n];
	}

	float4x4s&
	Mesh::getBindposes() noexcept
	{
		return bindposes_;
	}

	const float3s&
	Mesh::getVertexArray() const noexcept
	{
		return vertices_;
	}

	const float3s&
	Mesh::getNormalArray() const noexcept
	{
		return normals_;
	}

	const float4s&
	Mesh::getTangentArray() const noexcept
	{
		return tangents_;
	}

	const float4s&
	Mesh::getColorArray() const noexcept
	{
		return colors_;
	}

	const float2s&
	Mesh::getTexcoordArray(std::uint8_t n) const noexcept
	{
		assert(n < sizeof(texcoords_) / sizeof(float2s));
		return texcoords_[n];
	}

	const std::vector<VertexWeight>&
	Mesh::getWeightArray() const noexcept
	{
		return weights_;
	}

	const float4x4s&
	Mesh::getBindposes() const noexcept
	{
		return bindposes_;
	}

	const uint1s&
	Mesh::getIndicesArray(std::size_t n) const noexcept
	{
		return triangles_[n];
	}

	const BoundingBox&
	Mesh::getBoundingBox(std::size_t n) const noexcept
	{
		if (boundingBoxs_.empty() && n == 0)
			return boundingBox_;
		return boundingBoxs_[n];
	}

	const BoundingBox&
	Mesh::getBoundingBoxAll() const noexcept
	{
		return boundingBox_;
	}

	void
	Mesh::setDirty(bool dirty) noexcept
	{
		this->dirty_ = dirty;
	}

	bool
	Mesh::isDirty() const noexcept
	{
		return this->dirty_;
	}

	bool
	Mesh::raycast(const math::Raycast& ray, MeshHit& hit) noexcept
	{
		for (std::size_t i = 0; i < this->getNumSubsets(); i++)
		{
			if (math::intersect(ray, this->getBoundingBox(i).box()))
			{
				auto& indices = this->triangles_[i];

				for (std::size_t j = 0; j < indices.size(); j += 3)
				{
					auto& v0 = this->vertices_[indices[j]];
					auto& v1 = this->vertices_[indices[j+1]];
					auto& v2 = this->vertices_[indices[j+2]];

					if (math::intersect(ray, math::Triangle(v0, v1, v2), hit.point, hit.distance))
					{
						if (hit.distance > 0 && hit.distance < ray.maxDistance)
						{
							hit.mesh = i;
							hit.object = this;
							return true;
						}
					}
				}
			}
		}

		return false;
	}

	bool
	Mesh::raycastAll(const math::Raycast& ray, std::vector<MeshHit>& hits) noexcept
	{
		if (!math::intersect(ray, this->getBoundingBoxAll().sphere()))
			return false;

		if (this->triangles_.empty())
		{
			for (std::size_t i = 0; i < vertices_.size(); i += 3)
			{
				auto& v0 = vertices_[i];
				auto& v1 = vertices_[i + 1];
				auto& v2 = vertices_[i + 2];

				MeshHit hit;
				if (math::intersect(ray, math::Triangle(v0, v1, v2), hit.point, hit.distance))
				{
					if (hit.distance > 0 && hit.distance < ray.maxDistance)
					{
						hit.mesh = 0;
						hit.object = this;

						hits.emplace_back(hit);
					}
				}
			}
		}
		else
		{
			for (std::size_t i = 0; i < this->getNumSubsets(); i++)
			{
				if (math::intersect(ray, this->getBoundingBox(i).sphere()))
				{
					auto& indices = triangles_[i];

					for (std::size_t j = 0; j < triangles_[i].size(); j += 3)
					{
						std::size_t f1 = indices[j];
						std::size_t f2 = indices[j + 1];
						std::size_t f3 = indices[j + 2];

						auto& v0 = vertices_[f1];
						auto& v1 = vertices_[f2];
						auto& v2 = vertices_[f3];

						MeshHit hit;
						if (math::intersect(ray, math::Triangle(v0, v1, v2), hit.point, hit.distance))
						{
							if (hit.distance > 0 && hit.distance < ray.maxDistance)
							{
								hit.mesh = i;
								hit.object = this;

								hits.emplace_back(hit);
							}
						}
					}
				}
			}
		}

		return !hits.empty();
	}

	void
	Mesh::clear() noexcept
	{
		vertices_.shrink_to_fit();
		normals_.shrink_to_fit();
		colors_.shrink_to_fit();
		tangents_.shrink_to_fit();

		for (auto& it : triangles_)
			it.shrink_to_fit();

		for (auto& it : texcoords_)
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
		mesh->boundingBox_ = this->boundingBox_;
		mesh->boundingBoxs_ = this->boundingBoxs_;

		for (std::uint8_t i = 0; i < TEXTURE_ARRAY_COUNT; i++)
			mesh->setTexcoordArray(this->getTexcoordArray(i), i);

		for (std::uint8_t i = 0; i < this->getNumSubsets(); i++)
			mesh->setIndicesArray(this->getIndicesArray(i), i);

		return mesh;
	}

	bool
	Mesh::mergeMeshes(const Mesh& mesh, bool force) noexcept
	{
		if (!force)
		{
			if (vertices_.empty() != mesh.vertices_.empty()) return false;
			if (normals_.empty() != mesh.normals_.empty()) return false;
			if (colors_.empty() != mesh.colors_.empty()) return false;
			if (tangents_.empty() != mesh.tangents_.empty()) return false;
			if (bindposes_.empty() != mesh.bindposes_.empty()) return false;
			if (triangles_.empty() != mesh.triangles_.empty()) return false;
			if (weights_.empty() != mesh.weights_.empty()) return false;

			for (std::size_t i = 0; i < TEXTURE_ARRAY_COUNT; i++)
			{
				if (texcoords_[i].empty() != mesh.texcoords_[i].empty())
					return false;
			}
		}

		vertices_.insert(vertices_.end(), mesh.vertices_.begin(), mesh.vertices_.end());
		normals_.insert(normals_.end(), mesh.normals_.begin(), mesh.normals_.end());
		colors_.insert(colors_.end(), mesh.colors_.begin(), mesh.colors_.end());
		tangents_.insert(tangents_.end(), mesh.tangents_.begin(), mesh.tangents_.end());
		bindposes_.insert(bindposes_.end(), mesh.bindposes_.begin(), mesh.bindposes_.end());
		triangles_.insert(triangles_.end(), mesh.triangles_.begin(), mesh.triangles_.end());
		weights_.insert(weights_.end(), mesh.weights_.begin(), mesh.weights_.end());

		for (std::size_t i = 0; i < TEXTURE_ARRAY_COUNT; i++)
			texcoords_[i].insert(texcoords_[i].end(), mesh.texcoords_[i].begin(), mesh.texcoords_[i].end());

		return true;
	}

	bool
	Mesh::mergeMeshes(const CombineMesh instances[], std::size_t numInstance, bool merge) noexcept
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

		if (hasVertices) this->vertices_.resize(maxVertices);
		if (hasNormal)   this->normals_.resize(maxVertices);
		if (hasTangent)  this->tangents_.resize(maxVertices);
		if (hasWeight)   this->weights_.resize(maxVertices);
		if (hasIndices)  this->triangles_.resize(maxIndices);

		for (std::uint8_t i = 0; i < TEXTURE_ARRAY_COUNT; i++)
		{
			if (hasTexcoord[i])
				this->texcoords_[i].resize(maxVertices);
		}

		std::size_t offsetVertices = 0;

		for (std::size_t i = 0; i < numInstance; i++)
		{
			auto mesh = instances[i].getMesh();
			if (!mesh)
				continue;

			if (hasVertices) std::memcpy(&vertices_[offsetVertices], mesh->vertices_.data(), mesh->vertices_.size());
			if (hasNormal)   std::memcpy(&normals_[offsetVertices], mesh->normals_.data(), mesh->normals_.size());
			if (hasTangent)  std::memcpy(&tangents_[offsetVertices], mesh->tangents_.data(), mesh->tangents_.size());
			if (hasWeight)   std::memcpy(&weights_[offsetVertices], mesh->weights_.data(), mesh->weights_.size());
			if (hasIndices)  std::memcpy(&triangles_[offsetVertices], mesh->triangles_.data(), mesh->triangles_.size());

			for (std::uint8_t j = 0; j < TEXTURE_ARRAY_COUNT; j++)
			{
				if (hasTexcoord[j])
					std::memcpy(&texcoords_[j][offsetVertices], mesh->texcoords_[j].data(), mesh->texcoords_[j].size());
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
		if (vertices_.empty())
			return;

		if (normals_.empty())
			this->computeVertexNormals();

		std::map<std::pair<float, float>, std::uint32_t> vectorMap;

		float3s changeVertex;
		float3s changeNormal;

		for (auto& indices : this->triangles_)
		{
			for (auto& it : indices)
			{
				const Vector3& v = (vertices_)[it];
				const Vector3& n = (normals_)[it];

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

		vertices_.swap(changeVertex);
		normals_.swap(changeNormal);
	}

	void
	Mesh::computeFaceNormals(std::vector<math::float3s>& faceNormals) noexcept
	{
		assert(!vertices_.empty());

		faceNormals.resize(triangles_.size());

		for (std::size_t i = 0; i < triangles_.size(); i += 3)
		{
			auto& indices = triangles_[i];
			faceNormals[i].resize(indices.size());

			for (std::size_t j = 0; j < triangles_[i].size(); j += 3)
			{
				std::size_t f1 = indices[j];
				std::size_t f2 = indices[j + 1];
				std::size_t f3 = indices[j + 2];

				const Vector3& a = vertices_[f1];
				const Vector3& b = vertices_[f2];
				const Vector3& c = vertices_[f3];

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
		assert(!vertices_.empty());

		normals_.resize(vertices_.size());

		if (triangles_.empty())
		{
			for (std::size_t i = 0; i < vertices_.size(); i += 3)
			{
				auto& a = vertices_[i];
				auto& b = vertices_[i + 1];
				auto& c = vertices_[i + 2];

				auto ab = a - b;
				auto ac = a - c;

				auto n = math::normalize(math::cross(ac, ab));

				normals_[i + 0] = n;
				normals_[i + 1] = n;
				normals_[i + 2] = n;
			}
		}
		else
		{
			std::memset(normals_.data(), 0, normals_.size() * sizeof(float3));

			for (auto& indices : triangles_)
			{
				for (std::size_t i = 0; i < indices.size(); i += 3)
				{
					std::uint32_t f1 = indices[i];
					std::uint32_t f2 = indices[i + 1];
					std::uint32_t f3 = indices[i + 2];

					auto& v1 = vertices_.at(f1);
					auto& v2 = vertices_.at(f2);
					auto& v3 = vertices_.at(f3);
					
					auto n = math::normalize(math::cross(v1 - v2, v1 - v3));

					// https://www.bytehazard.com/articles/vertnorm.html
					normals_[f1] += n * std::acos(math::dot(math::normalize(v1 - v2), math::normalize(v1 - v3)));
					normals_[f2] += n * std::acos(math::dot(math::normalize(v2 - v1), math::normalize(v2 - v3)));
					normals_[f3] += n * std::acos(math::dot(math::normalize(v3 - v1), math::normalize(v3 - v2)));
				}
			}

			for (auto& it : normals_)
				it = math::normalize(it);
		}
	}

	void
	Mesh::computeVertexNormals(std::size_t n) noexcept
	{
		auto& indices = triangles_[n];
			
		for (auto& i : indices)
			normals_[i] = math::float3::Zero;

		for (std::size_t i = 0; i < indices.size(); i += 3)
		{
			std::uint32_t f1 = indices[i];
			std::uint32_t f2 = indices[i + 1];
			std::uint32_t f3 = indices[i + 2];

			auto& a = vertices_.at(f1);
			auto& b = vertices_.at(f2);
			auto& c = vertices_.at(f3);

			auto edge1 = c - b;
			auto edge2 = a - b;

			auto normal = math::normalize(math::cross(edge1, edge2));

			normals_[f1] += normal;
			normals_[f2] += normal;
			normals_[f3] += normal;
		}

		for (auto& i : indices)
			normals_[i] = math::normalize(normals_[i]);
	}

	void
	Mesh::computeVertexNormals(const float3s& faceNormals) noexcept
	{
		assert(faceNormals.size() == triangles_.size());
		assert(!vertices_.empty() && !triangles_.empty());

		float3s normal;
		normal.resize(vertices_.size());
		std::memset(normal.data(), 0, normal.size() * sizeof(float3));

		for (auto& indices : triangles_)
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

		normals_.swap(normal);
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
			return (vertices_)[y * width + x];
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

				normals_.push_back(math::normalize((lu + ru + ld + rd) / (float)average));
			}
		}
	}

	void
	Mesh::computeTangents(std::uint8_t n) noexcept
	{
		assert(!texcoords_[n].empty());

		float3s tan1(vertices_.size(), float3::Zero);
		float3s tan2(vertices_.size(), float3::Zero);

		for (auto& indices : triangles_)
		{
			for (std::size_t i = 0; i < indices.size(); i += 3)
			{
				std::uint32_t f1 = indices[i];
				std::uint32_t f2 = indices[i + 1];
				std::uint32_t f3 = indices[i + 2];

				auto& v1 = vertices_[f1];
				auto& v2 = vertices_[f2];
				auto& v3 = vertices_[f3];

				auto& w1 = texcoords_[n][f1];
				auto& w2 = texcoords_[n][f2];
				auto& w3 = texcoords_[n][f3];

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

		tangents_.resize(normals_.size());

		for (std::size_t i = 0; i < normals_.size(); i++)
		{
			auto& nor = normals_[i];
			auto& tan = tan1[i];

			float handedness = math::dot(math::cross(nor, tan), tan2[i]) < 0.0f ? 1.0f : -1.0f;

			tangents_[i] = float4(math::normalize(tan - nor * math::dot(nor, tan)), handedness);
		}
	}

	void
	Mesh::computeTangentQuats(float4s& tangentQuat) const noexcept
	{
		assert(tangents_.size() > 1);
		assert(tangents_.size() == normals_.size());

		tangentQuat.resize(tangents_.size());

		std::size_t numTangent = tangents_.size();
		for (std::size_t i = 0; i < numTangent; i++)
		{
			auto& normal = normals_[i];

			auto tangent = tangents_[i].xyz();
			auto binormal = math::cross(normal, tangent);

			Quaternion quat;
			quat.makeRotation(normal, binormal, tangent);

			if (quat.w < 0.0f)
				quat = -quat;

			if (tangents_[i].w < 0.0f)
				quat = -quat;

			tangentQuat[i].set(quat.x, quat.y, quat.z, quat.w);
		}
	}

	void
	Mesh::computeBoundingBox() noexcept
	{
		boundingBox_.reset();
		boundingBoxs_.resize(triangles_.size());

		if (triangles_.empty())
		{
			boundingBox_.encapsulate(vertices_);
		}
		else
		{
			auto vertices = vertices_.data();
			auto numSubsets = triangles_.size();

			for (int i = 0; i < numSubsets; i++)
			{
				math::AABB aabb;

				for (auto& j : triangles_[i])
					aabb.encapsulate(vertices[j]);

				boundingBoxs_[i].set(aabb);
			}

			for (std::size_t i = 0; i < numSubsets; i++)
				boundingBox_.encapsulate(boundingBoxs_[i]);
		}
	}

	void
	Mesh::computeLightMap(std::uint32_t width, std::uint32_t height) noexcept
	{
		std::vector<std::uint32_t> totalIndices;
		std::vector<math::float2> texcoords(this->vertices_.size());

		for (std::size_t i = 0; i < this->getNumSubsets(); i++)
		{
			for (auto& it : this->getIndicesArray(i))
				totalIndices.push_back(it);
		}

		std::size_t count = 0;

		std::vector<math::float2> uvs(totalIndices.size()); // allocate buffer for each output uv

		std::vector<std::uint32_t> remap(totalIndices.size()); // allocate buffer for each vertex index
		std::vector<std::uint32_t> outIndices(totalIndices.size()); // allocate buffer for each output uv

		if (uvmapper::lightmappack<std::uint32_t>((float*)this->vertices_.data(), totalIndices.data(), totalIndices.size(), width, height, 4, remap.data(), (float*)uvs.data(), outIndices.data(), count))
		{
			for (std::size_t i = 0; i < count; i++)
				texcoords[remap[i]] = uvs[i];

			this->setTexcoordArray(std::move(texcoords), 1);
		}
		else
		{
			std::cerr << "Failed to pack all triangles into the map!" << std::endl;
		}
	}
}