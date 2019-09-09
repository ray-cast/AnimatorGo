#ifndef OCTOON_MODEL_MATERIAL_H_
#define OCTOON_MODEL_MATERIAL_H_

#include <octoon/model/modtypes.h>

namespace octoon
{
	namespace model
	{
		class OCTOON_EXPORT Material final
		{
		public:
			struct MaterialParam
			{
				std::string key;

				std::size_t length;
				std::size_t dataType;

				char* data;
			};

		public:
			Material() noexcept;
			~Material() noexcept;

			bool set(const char* key, int value) noexcept;
			bool set(const char* key, float value) noexcept;
			bool set(const char* key, const math::Vector3& value) noexcept;
			bool set(const char* key, const math::Vector4& value) noexcept;
			bool set(const char* key, const char* value) noexcept;
			bool set(const char* key, const unsigned char* value) noexcept;
			bool set(const char* key, const std::string& value) noexcept;
			bool set(const MaterialParam& value) noexcept;

			bool get(const char* key, int& value) const noexcept;
			bool get(const char* key, float& value) const noexcept;
			bool get(const char* key, math::Vector3& value) const noexcept;
			bool get(const char* key, math::Vector4& value) const noexcept;
			bool get(const char* key, std::string& value) const noexcept;
			bool get(const char* key, MaterialParam& out) const noexcept;

			std::size_t hash() const noexcept;

			MaterialPtr clone() const noexcept;

		private:
			std::vector<MaterialParam> _properties;
		};
	}
}

#endif