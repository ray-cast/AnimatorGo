#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(TransformComponent, GameComponent, "TransformComponent")

	TransformComponent::TransformComponent() noexcept
		: localScaling_(math::float3::One)
		, localTranslate_(math::float3::Zero)
		, localRotation_(math::Quaternion::Zero)
		, worldScaling_(math::float3::One)
		, worldTranslate_(math::float3::Zero)
		, worldRotation_(math::Quaternion::Zero)
		, localNeedUpdates_(true)
		, worldNeedUpdates_(true)
	{
	}

	TransformComponent::~TransformComponent()
	{
	}

	void
	TransformComponent::setTranslate(const math::float3& pos) noexcept
	{
		if (localTranslate_ != pos)
		{
			this->_onMoveBefore();

			localTranslate_ = pos;
			localNeedUpdates_ = true;

			this->_updateLocalChildren();
			this->_onMoveAfter();
		}
	}

	void
	TransformComponent::setTranslateAccum(const math::float3& v) noexcept
	{
		this->setTranslate(localTranslate_ + v);
	}

	const math::float3&
	TransformComponent::getTranslate() const noexcept
	{
		_updateLocalTransform();
		return localTranslate_;
	}

	void
	TransformComponent::setScale(const math::float3& scale) noexcept
	{
		if (localScaling_ != scale)
		{
			this->_onMoveBefore();

			localScaling_ = scale;
			localNeedUpdates_ = true;

			this->_updateLocalChildren();
			this->_onMoveAfter();
		}
	}

	void
	TransformComponent::setScaleAll(const float scale) noexcept
	{
		this->setScale(math::float3(scale, scale, scale));
	}

	void
	TransformComponent::setScaleAccum(const math::float3& scale) noexcept
	{
		this->setScale(localScaling_ + scale);
	}

	const math::float3&
	TransformComponent::getScale() const noexcept
	{
		_updateLocalTransform();
		return localScaling_;
	}

	void
	TransformComponent::setQuaternion(const math::Quaternion& quat) noexcept
	{
		if (localRotation_ != quat)
		{
			this->_onMoveBefore();

			localRotation_ = quat;
			localNeedUpdates_ = true;

			this->_updateLocalChildren();
			this->_onMoveAfter();
		}
	}

	void
	TransformComponent::setQuaternionAccum(const math::Quaternion& quat) noexcept
	{
		this->setQuaternion(math::cross(quat, localRotation_));
	}

	const math::Quaternion&
	TransformComponent::getQuaternion() const noexcept
	{
		_updateLocalTransform();
		return localRotation_;
	}

	const math::float3&
	TransformComponent::getRight() const noexcept
	{
		_updateLocalTransform();
		return math::right(localTransform_);
	}

	const math::float3&
	TransformComponent::getUpVector() const noexcept
	{
		_updateLocalTransform();
		return math::up(localTransform_);
	}

	const math::float3&
	TransformComponent::getForward() const noexcept
	{
		_updateLocalTransform();
		return math::forward(localTransform_);
	}

	void
	TransformComponent::setTransform(const math::float4x4& transform) noexcept
	{
		this->_onMoveBefore();

		localTransform_ = transform.get_transform(localTranslate_, localRotation_, localScaling_);
		localNeedUpdates_ = false;

		this->_updateLocalChildren();
		this->_onMoveAfter();
	}

	void
	TransformComponent::setTransformOnlyRotate(const math::float4x4& transform) noexcept
	{
		this->_onMoveBefore();

		localTransform_ = transform.get_transform_without_scaler(localTranslate_, localRotation_);
		localTransform_.scale(localScaling_);
		localNeedUpdates_ = false;

		this->_updateLocalChildren();
		this->_onMoveAfter();
	}

	const math::float4x4&
	TransformComponent::getTransform() const noexcept
	{
		this->_updateLocalTransform();
		return localTransform_;
	}

	const math::float4x4&
	TransformComponent::getTransformInverse() const noexcept
	{
		this->_updateLocalTransform();
		return localTransformInverse_;
	}

	void
	TransformComponent::setWorldTranslate(const math::float3& pos) noexcept
	{
		if (worldTranslate_ != pos)
		{
			this->_onMoveBefore();

			worldTranslate_ = pos;
			worldNeedUpdates_ = true;

			this->_updateWorldChildren();
			this->_onMoveAfter();
		}
	}

	void
	TransformComponent::setWorldTranslateAccum(const math::float3& v) noexcept
	{
		this->setWorldTranslate(worldTranslate_ + v);
	}

	const math::float3&
	TransformComponent::getWorldTranslate() const noexcept
	{
		_updateWorldTransform();
		return worldTranslate_;
	}

	void
	TransformComponent::setWorldScale(const math::float3& pos) noexcept
	{
		if (worldScaling_ != pos)
		{
			this->_onMoveBefore();

			worldScaling_ = pos;
			worldNeedUpdates_ = true;

			this->_updateWorldChildren();
			this->_onMoveAfter();
		}
	}

	void
	TransformComponent::setWorldScaleAccum(const math::float3& scale) noexcept
	{
		this->setWorldScale(worldScaling_ + scale);
	}

	const math::float3&
	TransformComponent::getWorldScale() const noexcept
	{
		_updateWorldTransform();
		return worldScaling_;
	}

	void
	TransformComponent::setWorldQuaternion(const math::Quaternion& quat) noexcept
	{
		if (worldRotation_ != quat)
		{
			this->_onMoveBefore();

			worldRotation_ = quat;
			worldNeedUpdates_ = true;

			this->_updateWorldChildren();
			this->_onMoveAfter();
		}
	}

	void
	TransformComponent::setWorldQuaternionAccum(const math::Quaternion& quat) noexcept
	{
		this->setQuaternion(math::cross(quat, worldRotation_));
	}

	const math::Quaternion&
	TransformComponent::getWorldQuaternion() const noexcept
	{
		_updateWorldTransform();
		return worldRotation_;
	}

	void
	TransformComponent::setWorldTransform(const math::float4x4& transform) noexcept
	{
		this->_onMoveBefore();

		worldTransform_ = transform.get_transform(worldTranslate_, worldRotation_, worldScaling_);
		worldTransformInverse_ = math::transform_inverse(worldTransform_);
		worldNeedUpdates_ = false;

		this->_updateWorldChildren();
		this->_onMoveAfter();
	}

	void
	TransformComponent::setWorldTransformOnlyRotate(const math::float4x4& transform) noexcept
	{
		this->_onMoveBefore();

		worldTransform_ = transform.get_transform_without_scaler(worldTranslate_, worldRotation_);
		worldTransform_.scale(worldScaling_);
		worldTransformInverse_ = math::transform_inverse(worldTransform_);

		worldNeedUpdates_ = false;

		this->_updateWorldChildren();
		this->_onMoveAfter();
	}

	const math::float4x4&
	TransformComponent::getWorldTransform() const noexcept
	{
		this->_updateWorldTransform();
		return worldTransform_;
	}

	const math::float4x4&
	TransformComponent::getWorldTransformInverse() const noexcept
	{
		this->_updateWorldTransform();
		return worldTransformInverse_;
	}

	GameComponentPtr
	TransformComponent::clone() const noexcept
	{
		return std::make_shared<TransformComponent>();
	}

	void
	TransformComponent::_onMoveBefore() except
	{
		this->getGameObject()->_onMoveBefore();
	}

	void
	TransformComponent::_onMoveAfter() except
	{
		this->getGameObject()->_onMoveAfter();
	}

	void
	TransformComponent::_updateLocalChildren() const noexcept
	{
		worldNeedUpdates_ = true;

		for (auto& it : this->getGameObject()->getChildren())
			it->getComponent<TransformComponent>()->_updateLocalChildren();
	}

	void
	TransformComponent::_updateWorldChildren() const noexcept
	{
		this->_updateParentTransform();
		this->_updateLocalChildren();
	}

	void
	TransformComponent::_updateLocalTransform() const noexcept
	{
		if (localNeedUpdates_)
		{
			localTransform_.make_transform(localTranslate_, localRotation_, localScaling_);
			localTransformInverse_ = math::transform_inverse(localTransform_);

			localNeedUpdates_ = false;
		}
	}

	void
	TransformComponent::_updateWorldTransform() const noexcept
	{
		if (worldNeedUpdates_)
		{
			auto parent = this->getGameObject()->getParent();
			if (parent)
			{
				auto& baseTransform = parent->getComponent<TransformComponent>()->getWorldTransform();
				worldTransform_ = math::transform_multiply(baseTransform, this->getTransform());
				worldTransform_.get_transform(worldTranslate_, worldRotation_, worldScaling_);
				worldTransformInverse_ = math::transform_inverse(worldTransform_);
			}
			else
			{
				worldTranslate_ = localTranslate_;
				worldScaling_ = localScaling_;
				worldRotation_ = localRotation_;
				worldTransform_.make_transform(worldTranslate_, worldRotation_, worldScaling_);
				worldTransformInverse_ = math::transform_inverse(worldTransform_);
			}

			worldNeedUpdates_ = false;
		}
	}

	void
	TransformComponent::_updateParentTransform() const noexcept
	{
		if (worldNeedUpdates_)
		{
			worldTransform_.make_transform(worldTranslate_, worldRotation_, worldScaling_);
			worldNeedUpdates_ = false;
		}

		auto parent = this->getGameObject()->getParent();
		if (parent)
		{
			auto& baseTransformInverse = parent->getComponent<TransformComponent>()->getWorldTransformInverse();
			localTransform_ = math::transform_multiply(baseTransformInverse, worldTransform_);
			localTransform_.get_transform(localTranslate_, localRotation_, localScaling_);
		}
		else
		{
			localScaling_ = worldScaling_;
			localRotation_ = worldRotation_;
			localTranslate_ = worldTranslate_;
		}
	}
}