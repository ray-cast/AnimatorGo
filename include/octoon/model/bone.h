#ifndef OCTOON_MODEL_BONE_H_
#define OCTOON_MODEL_BONE_H_

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

			const std::string& getName() const noexcept;
			std::int16_t getParent() const noexcept;
			const math::float3& getPosition() const noexcept;
			const math::Quaternion& getRotation() const noexcept;

		private:
			std::string _name;
			std::int16_t _parent;

			math::float3 _translate;
			math::float3 _scaling;
			math::Quaternion _rotation;
		};
	}
}

#endif