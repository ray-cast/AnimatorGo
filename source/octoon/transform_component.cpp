#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(TransformComponent, GameComponent, "TransformComponent")

	TransformComponent::TransformComponent() noexcept
		: scaling_(math::float3::One)
		, translate_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, local_scaling_(math::float3::One)
		, local_translate_(math::float3::Zero)
		, local_rotation_(math::Quaternion::Zero)
		, local_need_updates_(true)
		, world_need_updates_(true)
	{
	}

	TransformComponent::~TransformComponent()
	{
	}

	void
	TransformComponent::setTranslate(const math::float3& pos) noexcept
	{
		if (translate_ != pos)
		{
			this->onMoveBefore();

			translate_ = pos;
			world_need_updates_ = true;

			this->updateWorldChildren();
			this->onMoveAfter();
		}
	}

	void
	TransformComponent::setTranslateAccum(const math::float3& v) noexcept
	{
		this->setTranslate(translate_ + v);
	}

	const math::float3&
	TransformComponent::getTranslate() const noexcept
	{
		updateWorldTransform();
		return translate_;
	}

	void
	TransformComponent::setScale(const math::float3& pos) noexcept
	{
		if (scaling_ != pos)
		{
			this->onMoveBefore();

			scaling_ = pos;
			world_need_updates_ = true;

			this->updateWorldChildren();
			this->onMoveAfter();
		}
	}

	void
	TransformComponent::setScaleAccum(const math::float3& scale) noexcept
	{
		this->setScale(scaling_ + scale);
	}

	const math::float3&
	TransformComponent::getScale() const noexcept
	{
		updateWorldTransform();
		return scaling_;
	}

	void
	TransformComponent::setQuaternion(const math::Quaternion& quat) noexcept
	{
		if (rotation_ != quat)
		{
			this->onMoveBefore();

			rotation_ = quat;
			world_need_updates_ = true;

			this->updateWorldChildren();
			this->onMoveAfter();
		}
	}

	void
	TransformComponent::setQuaternionAccum(const math::Quaternion& quat) noexcept
	{
		this->setLocalQuaternion(math::cross(quat, rotation_));
	}

	const math::Quaternion&
	TransformComponent::getQuaternion() const noexcept
	{
		updateWorldTransform();
		return rotation_;
	}

	void
	TransformComponent::setTransform(const math::float4x4& transform) noexcept
	{
		this->onMoveBefore();

		transform_ = transform.get_transform(translate_, rotation_, scaling_);
		transform_inverse_ = math::transform_inverse(transform_);
		world_need_updates_ = false;

		this->updateWorldChildren();
		this->onMoveAfter();
	}

	void
	TransformComponent::setTransformOnlyRotate(const math::float4x4& transform) noexcept
	{
		this->onMoveBefore();

		transform_ = transform.get_transform_without_scaler(translate_, rotation_);
		transform_.scale(scaling_);
		transform_inverse_ = math::transform_inverse(transform_);

		world_need_updates_ = false;

		this->updateWorldChildren();
		this->onMoveAfter();
	}

	const math::float4x4&
	TransformComponent::getTransform() const noexcept
	{
		this->updateWorldTransform();
		return transform_;
	}

	const math::float4x4&
	TransformComponent::getTransformInverse() const noexcept
	{
		this->updateWorldTransform();
		return transform_inverse_;
	}

	const math::float3&
	TransformComponent::getRight() const noexcept
	{
		updateWorldTransform();
		return math::right(transform_);
	}

	const math::float3&
	TransformComponent::getUp() const noexcept
	{
		updateWorldTransform();
		return math::up(transform_);
	}

	const math::float3&
	TransformComponent::getForward() const noexcept
	{
		updateWorldTransform();
		return math::forward(transform_);
	}

	void
	TransformComponent::setLocalTranslate(const math::float3& pos) noexcept
	{
		if (local_translate_ != pos)
		{
			this->onMoveBefore();

			local_translate_ = pos;
			local_need_updates_ = true;

			this->updateLocalChildren();
			this->onMoveAfter();
		}
	}

	void
	TransformComponent::setLocalTranslateAccum(const math::float3& v) noexcept
	{
		this->setLocalTranslate(local_translate_ + v);
	}

	const math::float3&
	TransformComponent::getLocalTranslate() const noexcept
	{
		updateLocalTransform();
		return local_translate_;
	}

	void
	TransformComponent::setLocalScale(const math::float3& scale) noexcept
	{
		if (local_scaling_ != scale)
		{
			this->onMoveBefore();

			local_scaling_ = scale;
			local_need_updates_ = true;

			this->updateLocalChildren();
			this->onMoveAfter();
		}
	}

	void
	TransformComponent::setLocalScaleAll(const float scale) noexcept
	{
		this->setLocalScale(math::float3(scale, scale, scale));
	}

	void
	TransformComponent::setLocalScaleAccum(const math::float3& scale) noexcept
	{
		this->setLocalScale(local_scaling_ + scale);
	}

	const math::float3&
	TransformComponent::getLocalScale() const noexcept
	{
		updateLocalTransform();
		return local_scaling_;
	}

	void
	TransformComponent::setLocalQuaternion(const math::Quaternion& quat) noexcept
	{
		if (local_rotation_ != quat)
		{
			this->onMoveBefore();

			local_rotation_ = quat;
			local_need_updates_ = true;

			this->updateLocalChildren();
			this->onMoveAfter();
		}
	}

	void
	TransformComponent::setLocalQuaternionAccum(const math::Quaternion& quat) noexcept
	{
		this->setLocalQuaternion(math::cross(quat, local_rotation_));
	}

	const math::Quaternion&
	TransformComponent::getLocalQuaternion() const noexcept
	{
		updateLocalTransform();
		return local_rotation_;
	}

	void
	TransformComponent::setLocalTransform(const math::float4x4& transform) noexcept
	{
		this->onMoveBefore();

		local_transform_ = transform.get_transform(local_translate_, local_rotation_, local_scaling_);
		local_need_updates_ = false;

		this->updateLocalChildren();
		this->onMoveAfter();
	}

	void
	TransformComponent::setLocalTransformOnlyRotate(const math::float4x4& transform) noexcept
	{
		this->onMoveBefore();

		local_transform_ = transform.get_transform_without_scaler(local_translate_, local_rotation_);
		local_transform_.scale(local_scaling_);
		local_need_updates_ = false;

		this->updateLocalChildren();
		this->onMoveAfter();
	}

	const math::float4x4&
	TransformComponent::getLocalTransform() const noexcept
	{
		this->updateLocalTransform();
		return local_transform_;
	}

	const math::float4x4&
	TransformComponent::getLocalTransformInverse() const noexcept
	{
		this->updateLocalTransform();
		return local_transform_inverse_;
	}

	const math::float3&
	TransformComponent::getLocalRight() const noexcept
	{
		updateLocalTransform();
		return math::right(local_transform_);
	}

	const math::float3&
	TransformComponent::getLocalUp() const noexcept
	{
		updateLocalTransform();
		return math::up(local_transform_);
	}

	const math::float3&
	TransformComponent::getLocalForward() const noexcept
	{
		updateLocalTransform();
		return math::forward(local_transform_);
	}

	GameComponentPtr
	TransformComponent::clone() const noexcept
	{
		auto component = std::make_shared<TransformComponent>();
		component->setTransform(this->getTransform());
		component->setLocalTransform(this->getLocalTransform());
		return component;
	}

	void
	TransformComponent::onMoveBefore() except
	{
		this->getGameObject()->onMoveBefore();
	}

	void
	TransformComponent::onMoveAfter() except
	{
		this->getGameObject()->onMoveAfter();
	}

	void
	TransformComponent::updateLocalChildren() const noexcept
	{
		world_need_updates_ = true;

		for (auto& it : this->getGameObject()->getChildren())
			it->getComponent<TransformComponent>()->updateLocalChildren();
	}

	void
	TransformComponent::updateWorldChildren() const noexcept
	{
		this->updateParentTransform();
		this->updateLocalChildren();
	}

	void
	TransformComponent::updateLocalTransform() const noexcept
	{
		if (local_need_updates_)
		{
			local_transform_.make_transform(local_translate_, local_rotation_, local_scaling_);
			local_transform_inverse_ = math::transform_inverse(local_transform_);

			local_need_updates_ = false;
		}
	}

	void
	TransformComponent::updateWorldTransform() const noexcept
	{
		if (world_need_updates_)
		{
			auto parent = this->getGameObject()->getParent();
			if (parent)
			{
				auto& baseTransform = parent->getComponent<TransformComponent>()->getTransform();
				transform_ = math::transform_multiply(baseTransform, this->getLocalTransform());
				transform_.get_transform(translate_, rotation_, scaling_);
				transform_inverse_ = math::transform_inverse(transform_);
			}
			else
			{
				transform_.make_transform(translate_, rotation_, scaling_);
				transform_ = math::transform_multiply(transform_, this->getLocalTransform());
				transform_inverse_ = math::transform_inverse(transform_);
			}

			world_need_updates_ = false;
		}
	}

	void
	TransformComponent::updateParentTransform() const noexcept
	{
		if (world_need_updates_)
		{
			transform_.make_transform(translate_, rotation_, scaling_);
			world_need_updates_ = false;
		}

		auto parent = this->getGameObject()->getParent();
		if (parent)
		{
			auto& baseTransformInverse = parent->getComponent<TransformComponent>()->getTransformInverse();
			local_transform_ = math::transform_multiply(baseTransformInverse, transform_);
			local_transform_.get_transform(local_translate_, local_rotation_, local_scaling_);
		}
		else
		{
			local_scaling_ = scaling_;
			local_rotation_ = rotation_;
			local_translate_ = translate_;
		}
	}
}