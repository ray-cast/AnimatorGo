#include <octoon/model/property.h>

#include <octoon/math/perlin_noise.h>

#include <map>
#include <cstring>

using namespace octoon::math;

namespace octoon
{
	namespace model
	{
		// ----------------------------------------------------------------------------------------
		// TextureProperty
		TextureProperty::TextureProperty() noexcept
		{
		}

		TextureProperty::TextureProperty(const std::string& filename) noexcept
			: name(filename)
		{
		}

		TextureProperty::TextureProperty(const char* filename, std::size_t length) noexcept
			: name(filename, length)
		{
		}

		// ----------------------------------------------------------------------------------------
		// MaterialProperty
		MaterialProperty::MaterialProperty() noexcept
		{
		}

		MaterialProperty::~MaterialProperty() noexcept
		{
			for (auto& it : _properties)
			{
				delete it->data;
				delete it;
			}
		}

		bool MaterialProperty::set(const char* key, std::size_t type, std::size_t index, int value) noexcept
		{
			assert(nullptr != key);

			auto it = _properties.begin();
			auto end = _properties.end();

			for (; it != end; ++it)
			{
				if ((*it)->key == key &&
					(*it)->type == type &&
					(*it)->index == index)
				{
					delete (*it);
					_properties.erase(it);
					break;
				}
			}

			auto prop = std::make_unique<MaterialParam>();
			prop->key = key;
			prop->type = type;
			prop->index = index;
			prop->length = sizeof(int);
			prop->dataType = PropertyTypeInfoInt;
			prop->data = new char[prop->length];

			std::memcpy(prop->data, &value, prop->length);

			_properties.push_back(prop.release());

			return true;
		}

		bool MaterialProperty::set(const char* key, std::size_t type, std::size_t index, float value) noexcept
		{
			assert(nullptr != key);

			auto it = _properties.begin();
			auto end = _properties.end();

			for (; it != end; ++it)
			{
				if ((*it)->key == key &&
					(*it)->type == type &&
					(*it)->index == index)
				{
					delete (*it);
					_properties.erase(it);
					break;
				}
			}

			auto prop = std::make_unique<MaterialParam>();

			prop->key = key;
			prop->type = type;
			prop->index = index;
			prop->length = sizeof(float);
			prop->dataType = PropertyTypeInfoFloat;
			prop->data = new char[prop->length];

			std::memcpy(prop->data, &value, prop->length);

			_properties.push_back(prop.release());

			return true;
		}

		bool MaterialProperty::set(const char* key, std::size_t type, std::size_t index, const Vector3& value) noexcept
		{
			assert(nullptr != key);

			auto it = _properties.begin();
			auto end = _properties.end();

			for (; it != end; ++it)
			{
				if ((*it)->key == key &&
					(*it)->type == type &&
					(*it)->index == index)
				{
					delete (*it);
					_properties.erase(it);
					break;
				}
			}

			auto prop = std::make_unique<MaterialParam>();

			prop->key = key;
			prop->type = type;
			prop->index = index;
			prop->length = sizeof(Vector3);
			prop->dataType = PropertyTypeInfoFloat | PropertyTypeInfoBuffer;
			prop->data = new char[prop->length];

			std::memcpy(prop->data, &value, prop->length);

			_properties.push_back(prop.release());

			return true;
		}

		bool MaterialProperty::set(const char* key, std::size_t type, std::size_t index, const Vector4& value) noexcept
		{
			assert(nullptr != key);

			auto it = _properties.begin();
			auto end = _properties.end();

			for (; it != end; ++it)
			{
				if ((*it)->key == key &&
					(*it)->type == type &&
					(*it)->index == index)
				{
					delete (*it);
					_properties.erase(it);
					break;
				}
			}

			auto prop = std::make_unique<MaterialParam>();

			prop->key = key;
			prop->type = type;
			prop->index = index;
			prop->length = sizeof(Vector4);
			prop->dataType = PropertyTypeInfoFloat | PropertyTypeInfoBuffer;
			prop->data = new char[prop->length];

			std::memcpy(prop->data, &value, prop->length);

			_properties.push_back(prop.release());

			return true;
		}

		bool MaterialProperty::set(const char* key, std::size_t type, std::size_t index, const char* value) noexcept
		{
			assert(nullptr != key);

			std::string str(value);
			return this->set(key, type, index, str);
		}

		bool MaterialProperty::set(const char* key, std::size_t type, std::size_t index, const unsigned char* value) noexcept
		{
			assert(nullptr != key);

			std::string str((char*)value);
			return this->set(key, type, index, str);
		}

		bool MaterialProperty::set(const char* key, std::size_t type, std::size_t index, const std::string& value) noexcept
		{
			assert(nullptr != key);

			auto it = _properties.begin();
			auto end = _properties.end();

			for (; it != end; ++it)
			{
				if ((*it)->key == key &&
					(*it)->type == type &&
					(*it)->index == index)
				{
					delete (*it);
					_properties.erase(it);
					break;
				}
			}

			auto prop = std::make_unique<MaterialParam>();

			prop->key = key;
			prop->type = type;
			prop->index = index;
			prop->length = value.length();
			prop->dataType = PropertyTypeInfoString;
			prop->data = new char[prop->length];

			std::memcpy(prop->data, value.data(), prop->length);

			_properties.push_back(prop.release());

			return true;
		}

		bool MaterialProperty::get(const char* key, std::size_t type, std::size_t index, int& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam* prop = nullptr;
			if (this->get(key, type, index, &prop))
			{
				if (prop->dataType == PropertyTypeInfoInt)
				{
					if (prop->length == sizeof(int))
					{
						std::memcpy(&value, prop->data, prop->length);
						return true;
					}
				}
			}

			return false;
		}

		bool MaterialProperty::get(const char* key, std::size_t type, std::size_t index, float& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam* prop = nullptr;
			if (this->get(key, type, index, &prop))
			{
				if (prop->dataType == PropertyTypeInfoFloat)
				{
					if (prop->length == sizeof(float))
					{
						std::memcpy(&value, prop->data, prop->length);
						return true;
					}
				}
			}

			return false;
		}

		bool MaterialProperty::get(const char* key, std::size_t type, std::size_t index, Vector3& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam* prop = nullptr;
			if (this->get(key, type, index, &prop))
			{
				if (prop->dataType & PropertyTypeInfoFloat &&
					prop->dataType & PropertyTypeInfoBuffer)
				{
					if (prop->length == sizeof(Vector3))
					{
						std::memcpy(&value, prop->data, prop->length);
						return true;
					}
				}
			}

			return false;
		}

		bool MaterialProperty::get(const char* key, std::size_t type, std::size_t index, Vector4& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam* prop = nullptr;
			if (this->get(key, type, index, &prop))
			{
				if (prop->dataType & PropertyTypeInfoFloat &&
					prop->dataType & PropertyTypeInfoBuffer)
				{
					if (prop->length == sizeof(Vector4))
					{
						std::memcpy(&value, prop->data, prop->length);
						return true;
					}
				}
			}

			return false;
		}

		bool MaterialProperty::get(const char* key, std::size_t type, std::size_t index, std::string& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam* prop = nullptr;
			if (this->get(key, type, index, &prop))
			{
				if (prop->dataType == PropertyTypeInfoString)
				{
					value.assign(prop->data, prop->length);
					return true;
				}
			}

			return false;
		}

		bool MaterialProperty::get(const char* key, std::size_t type, std::size_t index, MaterialParam** out) const noexcept
		{
			assert(nullptr != key);
			assert(nullptr != out);

			for (auto& it : _properties)
			{
				if (it->key == key &&
					it->type == type &&
					it->index == index)
				{
					*out = it;
					return true;
				}
			}

			return false;
		}

		// ----------------------------------------------------------------------------------------
		// MeshSubset
		MeshSubset::MeshSubset() noexcept
			: startVertices(0)
			, startIndices(0)
			, indicesCount(0)
			, offsetVertices(0)
			, offsetIndices(0)
		{
		}

		MeshSubset::MeshSubset(std::uint32_t _startVertices, std::uint32_t _startIndices, std::uint32_t _indicesCount, std::uint32_t _verticesOffset, std::uint32_t _indicesOffset) noexcept
			: startVertices(_startVertices)
			, startIndices(_startIndices)
			, indicesCount(_indicesCount)
			, offsetVertices(_verticesOffset)
			, offsetIndices(_indicesOffset)
		{
		}

		// ----------------------------------------------------------------------------------------
		// MeshProperty
		MeshProperty::MeshProperty() noexcept
		{
		}

		MeshProperty::~MeshProperty() noexcept
		{
		}

		void MeshProperty::setName(const std::string& name) noexcept
		{
			_name = name;
		}

		const std::string& MeshProperty::getName() const noexcept
		{
			return _name;
		}

		std::size_t MeshProperty::getNumVertices() const noexcept
		{
			return _vertices.size();
		}

		std::size_t MeshProperty::getNumIndices() const noexcept
		{
			return _indices.size();
		}

		std::size_t MeshProperty::getTexcoordNums() const noexcept
		{
			std::size_t count = 0;
			for (auto& it : _texcoords)
			{
				if (!it.empty())
					count++;
			}

			return count;
		}

		void MeshProperty::setVertexArray(const float3s& array) noexcept
		{
			_vertices = array;
		}

		void MeshProperty::setNormalArray(const float3s& array) noexcept
		{
			_normals = array;
		}

		void MeshProperty::setColorArray(const float4s& array) noexcept
		{
			_colors = array;
		}

		void MeshProperty::setTangentArray(const float4s& array) noexcept
		{
			_tangents = array;
		}

		void MeshProperty::setTexcoordArray(const float2s& array, std::uint8_t n) noexcept
		{
			assert(n < sizeof(_texcoords) / sizeof(float2s));
			_texcoords[n] = array;
		}

		void MeshProperty::setIndicesArray(const Uint1Array& array) noexcept
		{
			_indices = array;
		}

		void MeshProperty::setBindposes(const float4x4s& array) noexcept
		{
			_bindposes = array;
		}

		void MeshProperty::setMeshSubsets(const MeshSubsets& subsets) noexcept
		{
			_meshSubsets = subsets;
		}

		void MeshProperty::setWeightArray(const VertexWeights& array) noexcept
		{
			_weights = array;
		}

		void MeshProperty::setVertexArray(float3s&& array) noexcept
		{
			_vertices = std::move(array);
		}

		void
			MeshProperty::setNormalArray(float3s&& array) noexcept
		{
			_normals = std::move(array);
		}

		void MeshProperty::setColorArray(float4s&& array) noexcept
		{
			_colors = std::move(array);
		}

		void MeshProperty::setTangentArray(float4s&& array) noexcept
		{
			_tangents = std::move(array);
		}

		void MeshProperty::setTexcoordArray(float2s&& array, std::uint8_t n) noexcept
		{
			assert(n < sizeof(_texcoords) / sizeof(float2s));
			_texcoords[n] = std::move(array);
		}

		void MeshProperty::setIndicesArray(Uint1Array&& array) noexcept
		{
			_indices = std::move(array);
		}

		void MeshProperty::setWeightArray(VertexWeights&& array) noexcept
		{
			_weights = std::move(array);
		}

		void MeshProperty::setBindposes(float4x4s&& array) noexcept
		{
			_bindposes = std::move(array);
		}

		void MeshProperty::setMeshSubsets(MeshSubsets&& subsets) noexcept
		{
			_meshSubsets = std::move(subsets);
		}

		float3s& MeshProperty::getVertexArray() noexcept
		{
			return _vertices;
		}

		float3s& MeshProperty::getNormalArray() noexcept
		{
			return _normals;
		}

		float4s& MeshProperty::getTangentArray() noexcept
		{
			return _tangents;
		}

		float4s& MeshProperty::getColorArray() noexcept
		{
			return _colors;
		}

		float2s& MeshProperty::getTexcoordArray(std::uint8_t n) noexcept
		{
			assert(n < sizeof(_texcoords) / sizeof(float2s));
			return _texcoords[n];
		}

		VertexWeights& MeshProperty::getWeightArray() noexcept
		{
			return _weights;
		}

		Uint1Array& MeshProperty::getIndicesArray() noexcept
		{
			return _indices;
		}

		float4x4s& MeshProperty::getBindposes() noexcept
		{
			return _bindposes;
		}

		MeshSubsets& MeshProperty::getMeshSubsets() noexcept
		{
			return _meshSubsets;
		}

		const float3s& MeshProperty::getVertexArray() const noexcept
		{
			return _vertices;
		}

		const float3s& MeshProperty::getNormalArray() const noexcept
		{
			return _normals;
		}

		const float4s& MeshProperty::getTangentArray() const noexcept
		{
			return _tangents;
		}

		const float4s& MeshProperty::getColorArray() const noexcept
		{
			return _colors;
		}

		const float2s& MeshProperty::getTexcoordArray(std::uint8_t n) const noexcept
		{
			assert(n < sizeof(_texcoords) / sizeof(float2s));
			return _texcoords[n];
		}

		const VertexWeights& MeshProperty::getWeightArray() const noexcept
		{
			return _weights;
		}

		const float4x4s& MeshProperty::getBindposes() const noexcept
		{
			return _bindposes;
		}

		const MeshSubsets& MeshProperty::getMeshSubsets() const noexcept
		{
			return _meshSubsets;
		}

		const Bones& MeshProperty::getBoneArray(const Bones& array) const noexcept
		{
			return _bones;
		}

		const Uint1Array& MeshProperty::getIndicesArray() const noexcept
		{
			return _indices;
		}

		const BoundingBox& MeshProperty::getBoundingBox() const noexcept
		{
			return _boundingBox;
		}

		void MeshProperty::clear() noexcept
		{
			_vertices = float3s();
			_normals = float3s();
			_colors = float4s();
			_tangents = float4s();
			_indices = Uint1Array();

			for (std::size_t i = 0; i < 8; i++)
				_texcoords[i] = float2s();
		}

		MeshPropertyPtr MeshProperty::clone() noexcept
		{
			auto mesh = std::make_shared<MeshProperty>();
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
			mesh->_meshSubsets = this->_meshSubsets;

			return mesh;
		}

		void MeshProperty::makeCircle(float radius, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
		{
			this->clear();

			for (std::uint32_t i = 0; i <= segments; i++)
			{
				Vector3 v;

				float segment = thetaStart + i / segments * thetaLength;

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

				_normals.push_back(Vector3::UnitZ);
				_normals.push_back(Vector3::UnitZ);
				_normals.push_back(Vector3::UnitZ);
			}

			this->computeBoundingBox();
		}

		void MeshProperty::makePlane(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
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

			const Vector3& normal = Vector3::UnitZ;

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
					_texcoords[0].emplace_back((float)ix / gridX, (float)(1 - (iy + 1)) / gridY);
					_texcoords[0].emplace_back((float)(ix + 1) / gridX, (float)(1 - (iy + 1)) / gridY);
					_texcoords[0].emplace_back((float)ix / gridX, (float)(1 - iy) / gridY);
					_texcoords[0].emplace_back((float)(ix + 1) / gridX, (float)(1 - iy) / gridY);

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
			MeshProperty::makePlane(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir, bool clear) noexcept
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

		void MeshProperty::makePlaneWireframe(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir, bool clear) noexcept
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

		void MeshProperty::makeFloor(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
		{
			this->clear();
			this->makePlane(width, height, 0, widthSegments, 0, heightSegments, 'x', 'z', 1.0, 1.0);

			this->computeTangents();
			this->computeBoundingBox();
		}

		void MeshProperty::makeNoise(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
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

		void MeshProperty::makeCube(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments) noexcept
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

		void MeshProperty::makeCubeWireframe(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments) noexcept
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

		void MeshProperty::makeRing(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart, float thetaLength) noexcept
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

		void MeshProperty::makeSphere(float radius, std::uint32_t widthSegments, std::uint32_t heightSegments, float phiStart, float phiLength, float thetaStart, float thetaLength) noexcept
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

		void MeshProperty::makeVolumes(float fovy, float znear, float zfar) noexcept
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

		void MeshProperty::makeCone(float radius, float height, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
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

		bool MeshProperty::combineMeshes(const CombineMesh instances[], std::size_t numInstance, bool merge) noexcept
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
				if (hasTexcoord) this->_texcoords[i].resize(maxVertices);

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

		bool MeshProperty::combineMeshes(const CombineMeshes& instances, bool merge) noexcept
		{
			return this->combineMeshes(instances.data(), instances.size(), merge);
		}

		void MeshProperty::mergeVertices() noexcept
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

					std::size_t size = changeVertex.size();
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

		void MeshProperty::computeFaceNormals(float3s& faceNormals) noexcept
		{
			assert(!_vertices.empty() && !_indices.empty());

			faceNormals.resize(_vertices.size());

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

				faceNormals[f1] = normal;
				faceNormals[f2] = normal;
				faceNormals[f3] = normal;
			}
		}

		void MeshProperty::computeVertexNormals() noexcept
		{
			assert(!_vertices.empty() && !_indices.empty());

			std::size_t size = _indices.size();

			_normals.resize(_vertices.size());
			std::memset(_normals.data(), 0, _normals.size() * sizeof(float3));

			for (size_t i = 0; i < size; i += 3)
			{
				std::uint32_t f1 = (_indices)[i];
				std::uint32_t f2 = (_indices)[i + 1];
				std::uint32_t f3 = (_indices)[i + 2];

				Vector3& a = _vertices.at(f1);
				Vector3& b = _vertices.at(f2);
				Vector3& c = _vertices.at(f3);

				Vector3 edge1 = c - b;
				Vector3 edge2 = a - b;

				Vector3 n(math::normalize(math::cross(edge1, edge2)));

				_normals[f1] += n;
				_normals[f2] += n;
				_normals[f3] += n;
			}

			for (auto& it : _normals)
				it = math::normalize(it);
		}

		void MeshProperty::computeVertexNormals(const float3s& faceNormals) noexcept
		{
			assert(faceNormals.size() == _vertices.size());
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

				normal[a] += faceNormals[a];
				normal[b] += faceNormals[b];
				normal[c] += faceNormals[c];
			}

			for (auto& it : normal)
				it = math::normalize(it);

			_normals.swap(normal);
		}

		void MeshProperty::computeVertexNormals(std::size_t width, std::size_t height) noexcept
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

					Vector3 lu = math::cross(left, up);
					Vector3 ru = math::cross(up, right);
					Vector3 rd = math::cross(right, down);
					Vector3 ld = math::cross(down, left);

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

		void MeshProperty::computeTangents(std::uint8_t n) noexcept
		{
			assert(!_texcoords[n].empty());

			Vector3Array tan1(_vertices.size(), Vector3::Zero);
			Vector3Array tan2(_vertices.size(), Vector3::Zero);

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
					Vector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
					Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

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

		void MeshProperty::computeTangentQuats(float4s& tangentQuat) const noexcept
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
				quat.make_rotation(normal, binormal, tangent);

				if (quat.w < 0.0f)
					quat = -quat;

				if (_tangents[i].w < 0.0f)
					quat = -quat;

				tangentQuat[i].set(quat.x, quat.y, quat.z, quat.w);
			}
		}

		void MeshProperty::computeBoundingBox() noexcept
		{
			if (_meshSubsets.empty() && !_indices.empty())
				_meshSubsets.push_back(MeshSubset(0, 0, _indices.size(), 0, 0));

			_boundingBox.reset();

			for (auto& it : _meshSubsets)
				it.boundingBox.encapsulate(_vertices.data(), _indices.data() + it.startIndices, it.indicesCount);

			for (auto& it : _meshSubsets)
				_boundingBox.encapsulate(it.boundingBox.sphere());
		}
	}
}