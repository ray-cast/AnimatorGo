#include <octoon/rigidbody_component.h>
#include <octoon/collider_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_scene.h>
#include <octoon/physics_feature.h>

namespace octoon
{
	OctoonImplementSubClass(RigidbodyComponent, GameComponent, "RigidbodyComponent")

	RigidbodyComponent::RigidbodyComponent() noexcept
		: isKinematic_(false)
		, angularVelocity_(1.0)
		, gravityScale_(1.0)
		, mass_(1.0)
		, dynamicFriction_(0.5f)
		, staticFriction_(0.5f)
		, restitution_(0.0f)
		, linearDamping_(0.5f)
		, angularDamping_(0.5f)
		, position_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, groupMask_(0)
	{
	}

	RigidbodyComponent::RigidbodyComponent(bool type) noexcept
		: RigidbodyComponent()
	{
	}

	RigidbodyComponent::RigidbodyComponent(bool type, float mass) noexcept
		: RigidbodyComponent()
	{
	}

	RigidbodyComponent::~RigidbodyComponent()
	{
	}

	void
	RigidbodyComponent::setPosition(const math::float3& position) noexcept
	{
		position_ = position;
		transform_.makeTransform(position_, rotation_);
	}

	void
	RigidbodyComponent::setRotation(const math::Quaternion& quat) noexcept
	{
		rotation_ = quat;
		transform_.makeTransform(position_, rotation_);
	}

	void
	RigidbodyComponent::setGroupMask(std::uint16_t groupMask) noexcept
	{
		if (rigidbody_)
			rigidbody_->setGroupMask(groupMask);
		groupMask_ = groupMask;
	}

	void
	RigidbodyComponent::setAngularVelocity(float angularVelocity) noexcept
	{
		angularVelocity_ = angularVelocity;
	}

	void
	RigidbodyComponent::setGravityScale(float gravityScale) noexcept
	{
		gravityScale_ = gravityScale;
	}

	void
	RigidbodyComponent::setMass(float mass) noexcept
	{
		if (rigidbody_)
			rigidbody_->setMass(mass);
		mass_ = mass;
	}

	void
	RigidbodyComponent::setSleepMode(RigidbodySleepMode sleepMode) noexcept
	{
		sleepMode_ = sleepMode;
	}

	void
	RigidbodyComponent::setIsKinematic(bool isKinematic) noexcept
	{
		if (isKinematic_ != isKinematic)
		{
			if (rigidbody_)
				rigidbody_->setKinematic(isKinematic);
			isKinematic_ = isKinematic;
		}
	}

	void
	RigidbodyComponent::setDynamicFriction(float dynamicFriction)
	{
		if (rigidbody_)
			rigidbody_->setDynamicFriction(dynamicFriction);
		dynamicFriction_ = dynamicFriction;
	}

	void
	RigidbodyComponent::setStaticFriction(float staticFriction)
	{
		if (rigidbody_)
			rigidbody_->setStaticFriction(staticFriction);
		staticFriction_ = staticFriction;
	}

	void
	RigidbodyComponent::setRestitution(float restitution)
	{
		if (rigidbody_)
			rigidbody_->setRestitution(restitution);
		restitution_ = restitution;
	}

	void
	RigidbodyComponent::setLinearDamping(float value) noexcept
	{
		if (rigidbody_ && !isKinematic_)
			rigidbody_->setLinearDamping(value);
		linearDamping_ = value;
	}

	void
	RigidbodyComponent::setAngularDamping(float value) noexcept
	{
		if (rigidbody_ && !isKinematic_)
			rigidbody_->setAngularDamping(value);
		angularDamping_ = value;
	}

	float
	RigidbodyComponent::getDynamicFriction() const
	{
		return dynamicFriction_;
	}

	float
	RigidbodyComponent::getStaticFriction() const
	{
		return staticFriction_;
	}

	float
	RigidbodyComponent::getRestitution() const
	{
		return restitution_;
	}

	float
	RigidbodyComponent::getAngularVelocity() const noexcept
	{
		return angularVelocity_;
	}

	float
	RigidbodyComponent::getGravityScale() const noexcept
	{
		return gravityScale_;
	}

	float
	RigidbodyComponent::getMass() const noexcept
	{
		return mass_;
	}

	RigidbodySleepMode
	RigidbodyComponent::getSleepMode() const noexcept
	{
		return sleepMode_;
	}

	bool 
	RigidbodyComponent::getIsKinematic() const noexcept
	{
		return isKinematic_;
	}

	std::shared_ptr<physics::PhysicsRigidbody>
	RigidbodyComponent::getRigidbody()
	{
		return rigidbody_;
	}

	GameComponentPtr
	RigidbodyComponent::clone() const noexcept
	{
		auto instance = std::make_shared<RigidbodyComponent>();
		instance->setMass(instance->getMass());
		instance->setDynamicFriction(instance->getDynamicFriction());
		instance->setStaticFriction(instance->getStaticFriction());
		instance->setRestitution(instance->getRestitution());
		instance->setAngularVelocity(instance->getAngularVelocity());
		instance->setGravityScale(instance->getGravityScale());
		instance->setIsKinematic(instance->getIsKinematic());
		return instance;
	}

	void
	RigidbodyComponent::onActivate() except
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);
		auto collider = this->getComponent<ColliderComponent>();
		if (collider)
			this->setupRigidbody(*collider);
	}

	void
	RigidbodyComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);

		rigidbody_.reset();
	}

	void 
	RigidbodyComponent::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<ColliderComponent>())
		{
			auto collider = component->downcast_pointer<ColliderComponent>();
			if (collider)
				this->setupRigidbody(*collider);
		}
	}

	void 
	RigidbodyComponent::onDetachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<ColliderComponent>())
		{
			auto collider = component->downcast_pointer<ColliderComponent>();
			if (collider)
				rigidbody_->detachShape(collider->getShape());
		}
	}

	void 
	RigidbodyComponent::onFetchResult() noexcept
	{
		if (rigidbody_)
		{
			this->position_ = rigidbody_->getPosition();
			this->rotation_ = rigidbody_->getRotation();
			transform_.makeTransform(position_, rotation_);

			auto transform = this->getComponent<TransformComponent>();
			transform->getGameObject()->getParent()->getComponent<TransformComponent>()->setTransform(transform_ * transform->getLocalTransformInverse());
		}
	}

	void
	RigidbodyComponent::onLayerChangeAfter() noexcept
	{
		if (rigidbody_)
			rigidbody_->setGroup(this->getGameObject()->getLayer());
	}

	void 
	RigidbodyComponent::onMoveAfter() noexcept
	{
		if (rigidbody_ && isKinematic_)
		{
			auto transform = this->getComponent<TransformComponent>();
			rigidbody_->setPositionAndRotation(transform->getTranslate(), transform->getQuaternion());
		}
	}

	void
	RigidbodyComponent::updateParentTransform() noexcept
	{
		/*auto parent = this->getGameObject()->getParent()->getComponent<TransformComponent>();
		auto transform = math::transformMultiply(parent->getTransform(), localTransform_);
		transform.getTransform(position_, rotation_, scaling_);
		rigidbody_->setPositionAndRotation(position_, rotation_);*/

		auto parent = this->getGameObject()->getParent()->getComponent<TransformComponent>();
		localTransform_ = math::transformMultiply(parent->getTransformInverse(), transform_);
		localTransform_.getTransform(localPosition_, localRotation_, localScaling_);
	}

	void
	RigidbodyComponent::setupRigidbody(ColliderComponent& collider) noexcept
	{
		auto physicsFeature = this->getGameScene()->getFeature<PhysicsFeature>();
		if (physicsFeature && !rigidbody_)
		{
			physics::PhysicsRigidbodyDesc desc;
			desc.type = isKinematic_ ? physics::PhysicsRigidbodyType::Static : physics::PhysicsRigidbodyType::Dynamic;
			desc.translate = position_;
			desc.rotation = rotation_;
			desc.mass = mass_;

			rigidbody_ = physicsFeature->getContext()->createRigidbody(desc);
			if (rigidbody_)
			{
				rigidbody_->attachShape(collider.getShape());

				rigidbody_->setMass(mass_);
				rigidbody_->setOwnerListener(this);
				rigidbody_->setGroup(this->getGameObject()->getLayer());
				rigidbody_->setRestitution(restitution_);
				rigidbody_->setDynamicFriction(dynamicFriction_);
				rigidbody_->setStaticFriction(staticFriction_);
				rigidbody_->setPositionAndRotation(position_, rotation_);
				rigidbody_->setGroup(this->getGameObject()->getLayer());
				rigidbody_->setGroupMask(groupMask_);
				rigidbody_->setKinematic(isKinematic_);

				if (!isKinematic_)
				{
					rigidbody_->setLinearDamping(linearDamping_);
					rigidbody_->setAngularDamping(angularDamping_);
				}

				physicsFeature->getScene()->addRigidbody(rigidbody_);
			}
		}
	}
}