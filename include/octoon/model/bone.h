#ifndef OCTOON_BONE_H_
#define OCTOON_BONE_H_

#include <octoon/model/modtypes.h>

#include <octoon/math/vector3.h>
#include <octoon/math/mat4.h>
#include <octoon/math/quat.h>

#include <cstdint>

namespace octoon
{
	namespace model
	{
		class VertexWeight
		{
		public:
			float weight1;
			float weight2;
			float weight3;
			float weight4;

			std::uint8_t bone1;
			std::uint8_t bone2;
			std::uint8_t bone3;
			std::uint8_t bone4;
		};

		class IKChild
		{
		public:
			std::uint16_t boneIndex;
			std::uint8_t rotateLimited;

			float angleWeight;

			::octoon::math::float3 minimumDegrees;
			::octoon::math::float3 maximumDegrees;
		};

		class IKAttr
		{
		public:
			std::uint16_t boneIndex;
			std::uint16_t targetBoneIndex;
			std::uint32_t iterations;
			std::uint32_t chainLength;

			std::vector<IKChild> child;
		};


		class Bone final
		{
		public:
			Bone() noexcept;
			Bone(const std::string& name) noexcept;
			~Bone() noexcept;

			void setName(const std::string& name) noexcept;
			const std::string& getName() const noexcept;

			void setParent(std::int16_t parent) noexcept;
			std::int16_t getParent() const noexcept;

			void setPosition(const ::octoon::math::float3& position) noexcept;
			const ::octoon::math::float3& getPosition() const noexcept;

			void setRotation(const ::octoon::math::Quaternion& rotate) noexcept;
			const ::octoon::math::Quaternion& getRotation() const noexcept;

			void setScaling(const ::octoon::math::float3& scale) noexcept;
			const ::octoon::math::float3& getScaling() const noexcept;

			void setLocalTransform(const ::octoon::math::Matrix4x4& transform) noexcept;
			const ::octoon::math::Matrix4x4& getLocalTransform() const noexcept;

			void setTransform(const ::octoon::math::Matrix4x4& transform) noexcept;
			const ::octoon::math::Matrix4x4& getTransform() const noexcept;

		private:
			std::string _name;

			bool _isLeg;

			std::int16_t _parent;

			::octoon::math::float3 _translate;
			::octoon::math::float3 _scaling;
			::octoon::math::Quaternion _rotation;

			::octoon::math::float4x4 _transform;
			::octoon::math::float4x4 _localTransform;
		};
	}
}

#endif // !OCTOON_BONE_H_
