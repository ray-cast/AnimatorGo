#include <octoon/model/property.h>

#include <cstring>

namespace octoon
{
	namespace model
	{
		Texture::Texture() noexcept
		{
		}

		Texture::Texture(std::string&& filename) noexcept
			: name(std::move(filename))
		{
		}

		Texture::Texture(const std::string& filename) noexcept
			: name(filename)
		{
		}

		Texture::Texture(const char* filename, std::size_t length) noexcept
			: name(filename, length)
		{
		}

		Material::Material() noexcept
		{
		}

		Material::~Material() noexcept
		{
			for (auto& it : _properties)
			{
				delete it->data;
				delete it;
			}
		}

		bool Material::set(const char* key, std::size_t type, std::size_t index, int value) noexcept
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

		bool Material::set(const char* key, std::size_t type, std::size_t index, float value) noexcept
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

		bool Material::set(const char* key, std::size_t type, std::size_t index, const math::Vector3& value) noexcept
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
			prop->length = sizeof(math::Vector3);
			prop->dataType = PropertyTypeInfoFloat | PropertyTypeInfoBuffer;
			prop->data = new char[prop->length];

			std::memcpy(prop->data, &value, prop->length);

			_properties.push_back(prop.release());

			return true;
		}

		bool Material::set(const char* key, std::size_t type, std::size_t index, const math::Vector4& value) noexcept
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
			prop->length = sizeof(math::Vector4);
			prop->dataType = PropertyTypeInfoFloat | PropertyTypeInfoBuffer;
			prop->data = new char[prop->length];

			std::memcpy(prop->data, &value, prop->length);

			_properties.push_back(prop.release());

			return true;
		}

		bool Material::set(const char* key, std::size_t type, std::size_t index, const char* value) noexcept
		{
			assert(nullptr != key);

			std::string str(value);
			return this->set(key, type, index, str);
		}

		bool Material::set(const char* key, std::size_t type, std::size_t index, const unsigned char* value) noexcept
		{
			assert(nullptr != key);

			std::string str((char*)value);
			return this->set(key, type, index, str);
		}

		bool Material::set(const char* key, std::size_t type, std::size_t index, const std::string& value) noexcept
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

		bool Material::get(const char* key, std::size_t type, std::size_t index, int& value) const noexcept
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

		bool Material::get(const char* key, std::size_t type, std::size_t index, float& value) const noexcept
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

		bool Material::get(const char* key, std::size_t type, std::size_t index, math::Vector3& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam* prop = nullptr;
			if (this->get(key, type, index, &prop))
			{
				if (prop->dataType & PropertyTypeInfoFloat &&
					prop->dataType & PropertyTypeInfoBuffer)
				{
					if (prop->length == sizeof(math::Vector3))
					{
						std::memcpy(&value, prop->data, prop->length);
						return true;
					}
				}
			}

			return false;
		}

		bool Material::get(const char* key, std::size_t type, std::size_t index, math::Vector4& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam* prop = nullptr;
			if (this->get(key, type, index, &prop))
			{
				if (prop->dataType & PropertyTypeInfoFloat &&
					prop->dataType & PropertyTypeInfoBuffer)
				{
					if (prop->length == sizeof(math::Vector4))
					{
						std::memcpy(&value, prop->data, prop->length);
						return true;
					}
				}
			}

			return false;
		}

		bool Material::get(const char* key, std::size_t type, std::size_t index, std::string& value) const noexcept
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

		bool Material::get(const char* key, std::size_t type, std::size_t index, MaterialParam** out) const noexcept
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
	}
}