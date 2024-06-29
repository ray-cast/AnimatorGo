#ifndef OCTOON_BONE_H_
#define OCTOON_BONE_H_

#include <octoon/math/math.h>
#include <octoon/runtime/platform.h>

namespace octoon
{
	class OCTOON_EXPORT Bone final
	{
	public:
		Bone() noexcept;
		Bone(std::string_view name) noexcept;
		~Bone() noexcept;

		void setName(std::string_view name) noexcept;
		void setVisable(bool visable) noexcept;
		void setParent(std::int16_t parent) noexcept;
		void setAdditiveParent(std::int16_t parent) noexcept;
		void setAdditiveUseLocal(bool enable) noexcept;
		void setAdditiveMoveRatio(float ratio) noexcept;
		void setAdditiveRotationRatio(float ratio) noexcept;
		void setPosition(const math::float3& position) noexcept;
		void setRotation(const math::Quaternion& rotate) noexcept;

		const std::string& getName() const noexcept;
		std::int16_t getParent() const noexcept;
		std::int16_t getAdditiveParent() const noexcept;
		bool getVisable() const noexcept;
		bool getAdditiveUseLocal() const noexcept;
		float getAdditiveMoveRatio() const noexcept;
		float getAdditiveRotationRatio() const noexcept;
		const math::float3& getPosition() const noexcept;
		const math::Quaternion& getRotation() const noexcept;

	private:
		float _moveRatio;
		float _rotationRatio;

		bool _visable;
		bool _additiveUseLocal;

		std::string _name;
		std::int16_t _parent;
		std::int16_t _additivePrent;

		math::float3 _translate;
		math::float3 _scaling;
		math::Quaternion _rotation;
	};
}

#endif