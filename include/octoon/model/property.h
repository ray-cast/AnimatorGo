#ifndef OCTOON_MODEL_PROPERTY_H_
#define OCTOON_MODEL_PROPERTY_H_

#include <octoon/model/modtypes.h>
#include <octoon/model/bone.h>
#include <octoon/model/combine_mesh.h>

#include <octoon/math/mathfwd.h>
#include <octoon/math/vector2.h>
#include <octoon/math/vector3.h>
#include <octoon/math/boundingbox.h>

#include <string>

namespace octoon
{
	namespace model
	{
		enum OCTOON_EXPORT ShapeType
		{
			ShapeTypeCircle,
			ShapeTypeSquare,
			ShapeTypeCapsule
		};

		class OCTOON_EXPORT Rigidbody
		{
		public:
			std::string name;

			std::uint32_t bone;

			std::uint8_t group;
			std::uint16_t groupMask;
			ShapeType shape;

			math::float3 scale;
			math::float3 position;
			math::float3 rotation;

			float mass;
			float movementDecay;
			float rotationDecay;
			float elasticity;
			float friction;

			std::uint8_t physicsOperation; //0:Follow Bone (static), 1:Physics Calc. (dynamic), 2: Physics Calc. + Bone position matching
		};

		class OCTOON_EXPORT Joint
		{
		public:
			std::string name;

			math::float3 position;
			math::float3 rotation;

			std::uint32_t bodyIndexA;
			std::uint32_t bodyIndexB;

			math::float3 movementLowerLimit;
			math::float3 movementUpperLimit;
			math::float3 rotationLowerLimit;
			math::float3 rotationUpperLimit;

			math::float3 springMovementConstant;
			math::float3 springRotationConstant;
		};

		class OCTOON_EXPORT Texture final
		{
		public:
			Texture() noexcept;
			Texture(std::string&& filename) noexcept;
			Texture(const std::string& filename) noexcept;
			Texture(const char* filename, std::size_t length) noexcept;

			std::string   name;
			std::uint32_t width;
			std::uint32_t height;
		};

		class OCTOON_EXPORT Material final
		{
		public:
			struct MaterialParam
			{
				std::string key;

				std::size_t type;
				std::size_t index;
				std::size_t length;
				std::size_t dataType;

				char* data;
			};

		public:
			Material() noexcept;
			~Material() noexcept;

			bool set(const char* key, std::size_t type, std::size_t index, int value) noexcept;
			bool set(const char* key, std::size_t type, std::size_t index, float value) noexcept;
			bool set(const char* key, std::size_t type, std::size_t index, const math::Vector3& value) noexcept;
			bool set(const char* key, std::size_t type, std::size_t index, const math::Vector4& value) noexcept;
			bool set(const char* key, std::size_t type, std::size_t index, const char* value) noexcept;
			bool set(const char* key, std::size_t type, std::size_t index, const unsigned char* value) noexcept;
			bool set(const char* key, std::size_t type, std::size_t index, const std::string& value) noexcept;

			bool get(const char* key, std::size_t type, std::size_t index, int& value) const noexcept;
			bool get(const char* key, std::size_t type, std::size_t index, float& value) const noexcept;
			bool get(const char* key, std::size_t type, std::size_t index, math::Vector3& value) const noexcept;
			bool get(const char* key, std::size_t type, std::size_t index, math::Vector4& value) const noexcept;
			bool get(const char* key, std::size_t type, std::size_t index, std::string& value) const noexcept;
			bool get(const char* key, std::size_t type, std::size_t index, MaterialParam** out) const noexcept;

		private:
			std::vector<MaterialParam*> _properties;
		};
	}
}
#endif