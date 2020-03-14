#include <octoon/material/material.h>
#include <functional>

namespace octoon
{
	namespace material
	{
		Material::Material() noexcept
		{
		}

		Material::~Material() noexcept
		{
			for (auto& it : _properties)
				delete it.data;
		}

		bool
		Material::set(const char* key, int value) noexcept
		{
			assert(nullptr != key);

			auto it = _properties.begin();
			auto end = _properties.end();

			for (; it != end; ++it)
			{
				if ((*it).key == key)
				{
					_properties.erase(it);
					break;
				}
			}

			MaterialParam prop;
			prop.key = key;
			prop.length = sizeof(int);
			prop.dataType = PropertyTypeInfoInt;
			prop.data = new char[prop.length];

			std::memcpy(prop.data, &value, prop.length);

			_properties.push_back(prop);

			return true;
		}

		bool
		Material::set(const char* key, float value) noexcept
		{
			assert(nullptr != key);

			auto it = _properties.begin();
			auto end = _properties.end();

			for (; it != end; ++it)
			{
				if ((*it).key == key)
				{
					_properties.erase(it);
					break;
				}
			}

			MaterialParam prop;
			prop.key = key;
			prop.length = sizeof(float);
			prop.dataType = PropertyTypeInfoFloat;
			prop.data = new char[prop.length];

			std::memcpy(prop.data, &value, prop.length);

			_properties.push_back(prop);

			return true;
		}

		bool
		Material::set(const char* key, const math::Vector3& value) noexcept
		{
			assert(nullptr != key);

			auto it = _properties.begin();
			auto end = _properties.end();

			for (; it != end; ++it)
			{
				if ((*it).key == key)
				{
					_properties.erase(it);
					break;
				}
			}

			MaterialParam prop;
			prop.key = key;
			prop.length = sizeof(math::float3);
			prop.dataType = PropertyTypeInfoFloat | PropertyTypeInfoBuffer;
			prop.data = new char[prop.length];

			std::memcpy(prop.data, &value, prop.length);

			_properties.push_back(prop);

			return true;
		}

		bool
		Material::set(const char* key, const math::Vector4& value) noexcept
		{
			assert(nullptr != key);

			auto it = _properties.begin();
			auto end = _properties.end();

			for (; it != end; ++it)
			{
				if ((*it).key == key)
				{
					_properties.erase(it);
					break;
				}
			}

			MaterialParam prop;
			prop.key = key;
			prop.length = sizeof(math::float4);
			prop.dataType = PropertyTypeInfoFloat | PropertyTypeInfoBuffer;
			prop.data = new char[prop.length];

			std::memcpy(prop.data, &value, prop.length);

			_properties.push_back(prop);

			return true;
		}

		bool
		Material::set(const char* key, const char* value) noexcept
		{
			assert(nullptr != key);

			std::string str(value);
			return this->set(key, str);
		}

		bool
		Material::set(const char* key, const unsigned char* value) noexcept
		{
			assert(nullptr != key);

			std::string str((char*)value);
			return this->set(key, str);
		}

		bool
		Material::set(const char* key, const std::string& value) noexcept
		{
			assert(nullptr != key);

			auto it = _properties.begin();
			auto end = _properties.end();

			for (; it != end; ++it)
			{
				if ((*it).key == key)
				{
					_properties.erase(it);
					break;
				}
			}

			MaterialParam prop;
			prop.key = key;
			prop.length = value.length();
			prop.dataType = PropertyTypeInfoString;
			prop.data = new char[prop.length];

			std::memcpy(prop.data, value.data(), prop.length);

			_properties.push_back(prop);

			return true;
		}

		bool
		Material::set(const MaterialParam& value) noexcept
		{
			_properties.push_back(value);
			return true;
		}

		bool
		Material::get(const char* key, int& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam prop;
			if (this->get(key, prop))
			{
				if (prop.dataType == PropertyTypeInfoInt)
				{
					if (prop.length == sizeof(int))
					{
						std::memcpy(&value, prop.data, prop.length);
						return true;
					}
				}
			}

			return false;
		}

		bool
		Material::get(const char* key, float& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam prop;
			if (this->get(key, prop))
			{
				if (prop.dataType == PropertyTypeInfoFloat)
				{
					if (prop.length == sizeof(float))
					{
						std::memcpy(&value, prop.data, prop.length);
						return true;
					}
				}
			}

			return false;
		}

		bool
		Material::get(const char* key, math::Vector3& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam prop;
			if (this->get(key, prop))
			{
				if (prop.dataType & PropertyTypeInfoFloat &&
					prop.dataType & PropertyTypeInfoBuffer)
				{
					if (prop.length == sizeof(math::Vector3))
					{
						std::memcpy(&value, prop.data, prop.length);
						return true;
					}
				}
			}

			return false;
		}

		bool
		Material::get(const char* key, math::Vector4& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam prop;
			if (this->get(key, prop))
			{
				if (prop.dataType & PropertyTypeInfoFloat &&
					prop.dataType & PropertyTypeInfoBuffer)
				{
					if (prop.length == sizeof(math::Vector4))
					{
						std::memcpy(&value, prop.data, prop.length);
						return true;
					}
				}
			}

			return false;
		}

		bool
		Material::get(const char* key, std::string& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam prop;
			if (this->get(key, prop))
			{
				if (prop.dataType == PropertyTypeInfoString)
				{
					value.assign(prop.data, prop.length);
					return true;
				}
			}

			return false;
		}

		bool
		Material::get(const char* key, MaterialParam& out) const noexcept
		{
			assert(nullptr != key);

			for (auto& it : _properties)
			{
				if (it.key == key)
				{
					out = it;
					return true;
				}
			}

			return false;
		}

		std::size_t
		Material::hash() const noexcept
		{
			std::hash<std::string> hash;
			std::string hash_string;

			for (auto& it : _properties)
			{
				if (it.key == MATKEY_NAME || it.key == MATKEY_PATH)
					continue;

				hash_string += it.key;

				switch (it.dataType)
				{
				case PropertyTypeInfoFloat:
				{
					float value;
					std::memcpy(&value, it.data, it.length);
					hash_string += std::to_string(value);
				}
				break;
				case PropertyTypeInfoFloat | PropertyTypeInfoBuffer:
				{
					for (std::size_t i = 0; i < it.length; i += 4)
					{
						float value;
						std::memcpy(&value, it.data + i, 4);
						hash_string += std::to_string(value);
					}
				}
				break;
				case PropertyTypeInfoInt:
				{
					int value;
					std::memcpy(&value, it.data, it.length);
					hash_string += std::to_string(value);
				}
				break;
				case PropertyTypeInfoInt | PropertyTypeInfoBuffer:
				{
					for (std::size_t i = 0; i < it.length; i += 4)
					{
						int value;
						std::memcpy(&value, it.data + i, 4);
						hash_string += std::to_string(value);
					}
				}
				break;
				case PropertyTypeInfoString:
				{
					hash_string += std::string_view(it.data, it.length);
				}
				break;
				default:
					break;
				}
					
			}

			return hash(hash_string);
		}

		std::shared_ptr<Material>
		Material::clone() const noexcept
		{
			auto material = std::make_shared<Material>();
			for (auto& prop : this->_properties)
				material->set(prop);
			return material;
		}
	}
}