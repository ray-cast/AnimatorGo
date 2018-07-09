#ifndef OCTOON_MODEL_BONE_H_
#define OCTOON_MODEL_BONE_H_

#include <octoon/math/math.h>
#include <octoon/model/modtypes.h>

namespace octoon
{
	namespace model
	{
		class OCTOON_EXPORT Bone final
		{
		public:
			Bone() noexcept;
			Bone(const std::string& name) noexcept;
			~Bone() noexcept;

			void setName(const std::string& name) noexcept;
			void setParent(std::int16_t parent) noexcept;
			void setPosition(const math::float3& position) noexcept;
			void setRotation(const math::Quaternion& rotate) noexcept;
			void setScaling(const math::float3& scale) noexcept;
			void setLocalTransform(const math::float4x4& transform) noexcept;
			void setTransform(const math::float4x4& transform) noexcept;

			const std::string& getName() const noexcept;
			std::int16_t getParent() const noexcept;
			const math::float3& getPosition() const noexcept;
			const math::Quaternion& getRotation() const noexcept;
			const math::float3& getScaling() const noexcept;
			const math::float4x4& getLocalTransform() const noexcept;
			const math::float4x4& getTransform() const noexcept;

		private:
			std::string _name;
			std::int16_t _parent;

			math::float3 _translate;
			math::float3 _scaling;
			math::Quaternion _rotation;

			math::float4x4 _transform;
			math::float4x4 _localTransform;
		};
	}
}

#endif