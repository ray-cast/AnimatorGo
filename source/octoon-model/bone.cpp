#include <octoon/model/bone.h>

using namespace octoon::math;

namespace octoon
{
	namespace model
	{
		Bone::Bone() noexcept
			: _parent(-1)
			, _rotation(0.0f, 0.0f, 0.0f, 1.0f)
			, _scaling(float3::One)
			, _isLeg(false)
		{
			_localTransform = float4x4::One;
			_transform = float4x4::One;
		}

		Bone::Bone(const std::string& name) noexcept
		{
			_name = name;
		}

		Bone::~Bone() noexcept
		{
		}

		void Bone::setName(const std::string& name) noexcept
		{
			_name = name;
		}

		const std::string& Bone::getName() const noexcept
		{
			return _name;
		}

		void Bone::setParent(std::int16_t parent) noexcept
		{
			_parent = parent;
		}

		std::int16_t Bone::getParent() const noexcept
		{
			return _parent;
		}

		void Bone::setPosition(const float3& position) noexcept
		{
			_translate = position;
		}

		const float3& Bone::getPosition() const noexcept
		{
			return _translate;
		}

		void Bone::setScaling(const float3& scale) noexcept
		{
			_scaling = scale;
		}

		const float3& Bone::getScaling() const noexcept
		{
			return _scaling;
		}

		void Bone::setRotation(const Quaternion& rotate) noexcept
		{
			_rotation = rotate;
		}

		const Quaternion& Bone::getRotation() const noexcept
		{
			return _rotation;
		}

		void Bone::setLocalTransform(const Matrix4x4& transform) noexcept
		{
			_localTransform = transform;
		}

		const Matrix4x4& Bone::getLocalTransform() const noexcept
		{
			return _localTransform;
		}

		void Bone::setTransform(const Matrix4x4& transform) noexcept
		{
			_transform = transform;
		}

		const Matrix4x4& Bone::getTransform() const noexcept
		{
			return _transform;
		}
	}
}