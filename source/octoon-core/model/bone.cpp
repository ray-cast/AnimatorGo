#include <octoon/model/bone.h>

using namespace octoon::math;

namespace octoon
{
	namespace model
	{
		Bone::Bone() noexcept
			: _parent(-1)
			, _visable(true)
			, _additivePrent(-1)
			, _additiveUseLocal(false)
			, _rotation(0.0f, 0.0f, 0.0f, 1.0f)
			, _scaling(float3::One)
			, _moveRatio(0.0f)
			, _rotationRatio(0.0f)
		{
		}

		Bone::Bone(std::string_view name) noexcept
		{
			_name = name;
		}

		Bone::~Bone() noexcept
		{
		}

		void
		Bone::setName(std::string_view name) noexcept
		{
			_name = name;
		}

		const std::string&
		Bone::getName() const noexcept
		{
			return _name;
		}

		void
		Bone::setVisable(bool visable) noexcept
		{
			_visable = visable;
		}

		void
		Bone::setParent(std::int16_t parent) noexcept
		{
			_parent = parent;
		}

		void 
		Bone::setAdditiveParent(std::int16_t parent) noexcept
		{
			_additivePrent = parent;
		}

		void
		Bone::setAdditiveUseLocal(bool enable) noexcept
		{
			_additiveUseLocal = enable;
		}

		void
		Bone::setAdditiveMoveRatio(float ratio) noexcept
		{
			_moveRatio = ratio;
		}

		void
		Bone::setAdditiveRotationRatio(float ratio) noexcept
		{
			_rotationRatio = ratio;
		}

		bool
		Bone::getVisable() const noexcept
		{
			return _visable;
		}

		std::int16_t
		Bone::getParent() const noexcept
		{
			return _parent;
		}

		std::int16_t
		Bone::getAdditiveParent() const noexcept
		{
			return _additivePrent;
		}

		bool
		Bone::getAdditiveUseLocal() const noexcept
		{
			return _additiveUseLocal;
		}

		float
		Bone::getAdditiveMoveRatio() const noexcept
		{
			return _moveRatio;
		}

		float
		Bone::getAdditiveRotationRatio() const noexcept
		{
			return _rotationRatio;
		}

		void
		Bone::setPosition(const float3& position) noexcept
		{
			_translate = position;
		}

		const float3&
		Bone::getPosition() const noexcept
		{
			return _translate;
		}

		void
		Bone::setRotation(const Quaternion& rotate) noexcept
		{
			_rotation = rotate;
		}

		const Quaternion&
		Bone::getRotation() const noexcept
		{
			return _rotation;
		}
	}
}