#include <octoon/model/material.h>

namespace octoon
{
	namespace model
	{
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

		bool
		Material::set(const char* key, int value) noexcept
		{
			assert(nullptr != key);

			auto it = _properties.begin();
			auto end = _properties.end();

			for (; it != end; ++it)
			{
				if ((*it)->key == key)
				{
					delete (*it);
					_properties.erase(it);
					break;
				}
			}

			auto prop = std::make_unique<MaterialParam>();
			prop->key = key;
			prop->length = sizeof(int);
			prop->dataType = PropertyTypeInfoInt;
			prop->data = new char[prop->length];

			std::memcpy(prop->data, &value, prop->length);

			_properties.push_back(prop.release());

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
				if ((*it)->key == key)
				{
					delete (*it);
					_properties.erase(it);
					break;
				}
			}

			auto prop = std::make_unique<MaterialParam>();

			prop->key = key;
			prop->length = sizeof(float);
			prop->dataType = PropertyTypeInfoFloat;
			prop->data = new char[prop->length];

			std::memcpy(prop->data, &value, prop->length);

			_properties.push_back(prop.release());

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
				if ((*it)->key == key)
				{
					delete (*it);
					_properties.erase(it);
					break;
				}
			}

			auto prop = std::make_unique<MaterialParam>();

			prop->key = key;
			prop->length = sizeof(math::Vector3);
			prop->dataType = PropertyTypeInfoFloat | PropertyTypeInfoBuffer;
			prop->data = new char[prop->length];

			std::memcpy(prop->data, &value, prop->length);

			_properties.push_back(prop.release());

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
				if ((*it)->key == key)
				{
					delete (*it);
					_properties.erase(it);
					break;
				}
			}

			auto prop = std::make_unique<MaterialParam>();

			prop->key = key;
			prop->length = sizeof(math::Vector4);
			prop->dataType = PropertyTypeInfoFloat | PropertyTypeInfoBuffer;
			prop->data = new char[prop->length];

			std::memcpy(prop->data, &value, prop->length);

			_properties.push_back(prop.release());

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
				if ((*it)->key == key)
				{
					delete (*it);
					_properties.erase(it);
					break;
				}
			}

			auto prop = std::make_unique<MaterialParam>();

			prop->key = key;
			prop->length = value.length();
			prop->dataType = PropertyTypeInfoString;
			prop->data = new char[prop->length];

			std::memcpy(prop->data, value.data(), prop->length);

			_properties.push_back(prop.release());

			return true;
		}

		bool
		Material::get(const char* key, int& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam* prop = nullptr;
			if (this->get(key, &prop))
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

		bool
		Material::get(const char* key, float& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam* prop = nullptr;
			if (this->get(key, &prop))
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

		bool
		Material::get(const char* key, math::Vector3& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam* prop = nullptr;
			if (this->get(key, &prop))
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

		bool
		Material::get(const char* key, math::Vector4& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam* prop = nullptr;
			if (this->get(key, &prop))
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

		bool
		Material::get(const char* key, std::string& value) const noexcept
		{
			assert(nullptr != key);

			MaterialParam* prop = nullptr;
			if (this->get(key, &prop))
			{
				if (prop->dataType == PropertyTypeInfoString)
				{
					value.assign(prop->data, prop->length);
					return true;
				}
			}

			return false;
		}

		bool
		Material::get(const char* key, MaterialParam** out) const noexcept
		{
			assert(nullptr != key);
			assert(nullptr != out);

			for (auto& it : _properties)
			{
				if (it->key == key)
				{
					*out = it;
					return true;
				}
			}

			return false;
		}
	}
}