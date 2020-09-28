#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(TransformComponent, GameComponent, "TransformComponent")

	TransformComponent::TransformComponent() noexcept
		: scaling_(math::float3::One)
		, translate_(math::float3::Zero)
		, euler_angles_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, local_scaling_(math::float3::One)
		, local_translate_(math::float3::Zero)
		, local_euler_angles_(math::float3::Zero)
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
		assert(math::abs(math::length(quat) - 1) < 1e-2f);

		if (rotation_ != quat)
		{
			this->onMoveBefore();

			rotation_ = quat;
			euler_angles_ = math::eulerAngles(quat);
			world_need_updates_ = true;

			this->updateWorldChildren();
			this->onMoveAfter();
		}
	}

	void
	TransformComponent::setQuaternionAccum(const math::Quaternion& quat) noexcept
	{
		assert(math::abs(math::length(quat) - 1) < 1e-2f);
		this->setQuaternion(math::normalize(this->getQuaternion() * quat));
	}

	const math::Quaternion&
	TransformComponent::getQuaternion() const noexcept
	{
		updateWorldTransform();
		return rotation_;
	}

	void
	TransformComponent::setEulerAngles(const math::float3& euler) noexcept
	{
		if (euler_angles_ != euler)
		{
			this->onMoveBefore();

			euler_angles_ = euler;
			rotation_ = math::normalize(math::Quaternion(euler));
			world_need_updates_ = true;

			this->updateWorldChildren();
			this->onMoveAfter();
		}
	}
	
	const math::float3&
	TransformComponent::getEulerAngles() const noexcept
	{
		updateWorldTransform();
		return euler_angles_;
	}

	void
	TransformComponent::setTransform(const math::float4x4& transform) noexcept
	{
		this->onMoveBefore();

		transform_ = transform.getTransform(translate_, rotation_, scaling_);
		transform_inverse_ = math::transformInverse(transform_);
		world_need_updates_ = false;

		this->updateWorldChildren();
		this->onMoveAfter();
	}

	void
	TransformComponent::setTransform(const math::float3& translate, const math::Quaternion& quat, const math::float3& scale) noexcept
	{
		assert(math::abs(math::length(quat) - 1) < 1e-2f);

		if (translate != translate_ || rotation_ != quat || scale != scaling_)
		{
			this->onMoveBefore();

			scaling_ = scale;
			rotation_ = quat;
			translate_ = translate;
			world_need_updates_ = true;

			this->updateWorldChildren();
			this->onMoveAfter();
		}
	}

	void
	TransformComponent::setTransformOnlyRotate(const math::float4x4& transform) noexcept
	{
		this->onMoveBefore();

		transform_ = transform.getTransformWithoutScaler(translate_, rotation_);
		transform_.scale(scaling_);
		transform_inverse_ = math::transformInverse(transform_);

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
		assert(math::abs(math::length(quat) - 1) < 1e-2f);

		if (local_rotation_ != quat)
		{
			this->onMoveBefore();

			local_rotation_ = quat;
			local_euler_angles_ = math::eulerAngles(quat);
			local_need_updates_ = true;

			this->updateLocalChildren();
			this->onMoveAfter();
		}
	}

	void
	TransformComponent::setLocalQuaternionAccum(const math::Quaternion& quat) noexcept
	{
		assert(math::abs(math::length(quat) - 1) < 1e-2f);

		this->setLocalQuaternion(math::normalize(this->getLocalQuaternion() * quat));
	}

	const math::Quaternion&
	TransformComponent::getLocalQuaternion() const noexcept
	{
		updateLocalTransform();
		return local_rotation_;
	}

	void
	TransformComponent::setLocalEulerAngles(const math::float3& euler) noexcept
	{
		if (this->local_euler_angles_ != euler)
		{
			this->onMoveBefore();

			local_euler_angles_ = euler;
			local_rotation_ = math::normalize(math::Quaternion(euler));
			local_need_updates_ = true;

			this->updateLocalChildren();
			this->onMoveAfter();
		}		
	}
	
	const math::float3&
	TransformComponent::getLocalEulerAngles() const noexcept
	{
		updateLocalTransform();
		return this->local_euler_angles_;
	}

	void
	TransformComponent::setLocalTransform(const math::float4x4& transform) noexcept
	{
		this->onMoveBefore();

		local_transform_ = transform.getTransform(local_translate_, local_rotation_, local_scaling_);
		local_need_updates_ = false;

		this->updateLocalChildren();
		this->onMoveAfter();
	}

	void
	TransformComponent::setLocalTransformOnlyRotate(const math::float4x4& transform) noexcept
	{
		this->onMoveBefore();

		local_transform_ = transform.getTransformWithoutScaler(local_translate_, local_rotation_);
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

	void
	TransformComponent::up(float speed) noexcept
	{
		this->setLocalTranslateAccum(this->getLocalUp() * speed);
	}

	void
	TransformComponent::up(const math::float3& speed) noexcept
	{
		this->setLocalTranslateAccum(this->getLocalUp() * speed);
	}

	void
	TransformComponent::yaw(float speed) noexcept
	{
		this->setLocalTranslateAccum(this->getLocalRight() * speed);
	}

	void
	TransformComponent::yaw(const math::float3& speed) noexcept
	{
		this->setLocalTranslateAccum(this->getLocalRight() * speed);
	}

	void
	TransformComponent::move(float speed) noexcept
	{
		this->setLocalTranslateAccum(this->getLocalForward() * speed);
	}

	void
	TransformComponent::move(const math::float3& speed) noexcept
	{
		this->setLocalTranslateAccum(this->getLocalForward() * speed);
	}

	GameComponentPtr
	TransformComponent::clone() const noexcept
	{
		auto component = std::make_shared<TransformComponent>();
		component->setName(this->getName());
		component->setLocalScale(this->getLocalScale());
		component->setLocalTranslate(this->getLocalTranslate());
		component->setLocalQuaternion(this->getLocalQuaternion());
		return component;
	}

	void
	TransformComponent::onMoveBefore() except
	{
		if (this->getGameObject())
			this->getGameObject()->onMoveBefore();
	}

	void
	TransformComponent::onMoveAfter() except
	{
		if (this->getGameObject())
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
		world_need_updates_ = false;
	}

	void
	TransformComponent::updateLocalTransform() const noexcept
	{
		if (local_need_updates_)
		{
			local_transform_.makeTransform(local_translate_, local_rotation_, local_scaling_);
			local_transform_inverse_ = math::transformInverse(local_transform_);

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
				transform_ = math::transformMultiply(baseTransform, this->getLocalTransform());
				transform_.getTransform(translate_, rotation_, scaling_);
				transform_inverse_ = math::transformInverse(transform_);
				euler_angles_ = math::eulerAngles(rotation_);
			}
			else
			{
				translate_ = local_translate_;
				scaling_ = local_scaling_;
				euler_angles_ = local_euler_angles_;
				rotation_ = local_rotation_;

				transform_.makeTransform(translate_, rotation_, scaling_);
				transform_inverse_ = math::transformInverse(transform_);
			}

			world_need_updates_ = false;
		}
	}

	void
	TransformComponent::updateParentTransform() const noexcept
	{
		if (world_need_updates_)
		{
			transform_.makeTransform(translate_, rotation_, scaling_);
			transform_inverse_ = math::transformInverse(transform_);
			world_need_updates_ = false;
		}

		auto parent = this->getGameObject()->getParent();
		if (parent)
		{
			auto& baseTransformInverse = parent->getComponent<TransformComponent>()->getTransformInverse();
			local_transform_ = math::transformMultiply(baseTransformInverse, transform_);
			local_transform_.getTransform(local_translate_, local_rotation_, local_scaling_);
			local_euler_angles_ = math::eulerAngles(local_rotation_);
		}
		else
		{
			local_scaling_ = scaling_;
			local_rotation_ = rotation_;
			local_translate_ = translate_;
			local_euler_angles_ = euler_angles_;
		}
	}
}