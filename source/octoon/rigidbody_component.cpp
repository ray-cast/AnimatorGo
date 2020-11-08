#include <octoon/rigidbody_component.h>
#include <octoon/collider_component.h>
#include <octoon/transform_component.h>
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
		, groupMask_(0)
		, sleepThreshold_(0.005)
		, minPositionIters_(4)
		, minVelocityIters_(1)
		, enableCCD_(false)
		, position_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
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
	RigidbodyComponent::movePosition(const math::float3& position) noexcept
	{
		if (position_ != position)
		{
			if (rigidbody_)
				rigidbody_->setPosition(position);
			position_ = position;
		}
	}

	void
	RigidbodyComponent::rotation(const math::Quaternion& quat) noexcept
	{
		if (rotation_ != quat)
		{
			if (rigidbody_)
				rigidbody_->setRotation(quat);
			rotation_ = quat;
		}
	}

	void
	RigidbodyComponent::setGroupMask(std::uint16_t groupMask) noexcept
	{
		if (rigidbody_)
			rigidbody_->setGroupMask(groupMask);
		groupMask_ = groupMask;
	}

	void
	RigidbodyComponent::setSleepThreshold(float threshold) noexcept
	{
		if (rigidbody_)
			rigidbody_->setSleepThreshold(threshold);
		sleepThreshold_ = threshold;
	}

	void
	RigidbodyComponent::setEnableCCD(bool enable) noexcept
	{
		assert(!enable || enable && !isKinematic_);

		if (rigidbody_)
			rigidbody_->setEnableCCD(enable);
		enableCCD_ = enable;
	}

	void
	RigidbodyComponent::setSolverIterationCounts(std::uint32_t minPositionIters, std::uint32_t minVelocityIters) noexcept
	{
		if (rigidbody_)
			rigidbody_->setSolverIterationCounts(minPositionIters, minVelocityIters);
		minPositionIters_ = minPositionIters;
		minVelocityIters_ = minVelocityIters;
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

	float
	RigidbodyComponent::getSleepThreshold() const noexcept
	{
		return sleepThreshold_;
	}

	bool
	RigidbodyComponent::getEnableCCD() const noexcept
	{
		return enableCCD_;
	}

	std::uint32_t
	RigidbodyComponent::getGroup() const noexcept
	{
		return this->getGameObject()->getLayer();
	}

	std::uint16_t
	RigidbodyComponent::getGroupMask() const noexcept
	{
		return groupMask_;
	}

	RigidbodySleepMode
	RigidbodyComponent::getSleepMode() const noexcept
	{
		return sleepMode_;
	}

	void
	RigidbodyComponent::wakeUp() noexcept
	{
		if (rigidbody_ && !isKinematic_)
			rigidbody_->wakeUp();
	}

	bool 
	RigidbodyComponent::getIsKinematic() const noexcept
	{
		return isKinematic_;
	}

	std::shared_ptr<PhysicsRigidbody>
	RigidbodyComponent::getRigidbody()
	{
		return rigidbody_;
	}

	void
	RigidbodyComponent::clearForce() noexcept
	{
		if (rigidbody_ && !isKinematic_)
			rigidbody_->clearForce();
	}
	
	void
	RigidbodyComponent::clearTorque() noexcept
	{
		if (rigidbody_ && !isKinematic_)
			rigidbody_->clearTorque();
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
		instance->setSleepThreshold(instance->getSleepThreshold());
		instance->setEnableCCD(instance->getEnableCCD());
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

		if (rigidbody_)
		{
			auto physicsFeature = this->getFeature<PhysicsFeature>();
			if (physicsFeature && !rigidbody_)
				physicsFeature->getScene()->removeRigidbody(rigidbody_);

			rigidbody_->setOwnerListener(nullptr);
			rigidbody_ = nullptr;
		}
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
			auto transform = this->getComponent<TransformComponent>();
			if (transform)
			{
				this->position_ = rigidbody_->getPosition();
				this->rotation_ = rigidbody_->getRotation();

				auto parent = this->getGameObject()->getParent();
				if (parent)
					parent->getComponent<TransformComponent>()->setTransform(math::float4x4(this->rotation_, this->position_) * transform->getLocalTransformInverse());
				else
				{
					transform->setTranslate(this->position_);
					transform->setQuaternion(this->rotation_);
				}
			}
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
			rotation_ = transform->getQuaternion();
			position_ = transform->getTranslate();
			rigidbody_->setPositionAndRotation(transform->getTranslate(), transform->getQuaternion());
		}
	}

	void
	RigidbodyComponent::setupRigidbody(ColliderComponent& collider) noexcept
	{
		auto physicsFeature = this->tryGetFeature<PhysicsFeature>();
		if (physicsFeature && !rigidbody_)
		{
			auto transform = this->getComponent<TransformComponent>();

			PhysicsRigidbodyDesc desc;
			desc.type = isKinematic_ ? PhysicsRigidbodyType::Static : PhysicsRigidbodyType::Dynamic;
			desc.translate = position_ = transform->getTranslate();
			desc.rotation = rotation_ = transform->getQuaternion();
			desc.mass = mass_;

			rigidbody_ = physicsFeature->getContext()->createRigidbody(desc);
			if (rigidbody_)
			{
				rigidbody_->attachShape(collider.getShape());

				rigidbody_->setMass(mass_);
				rigidbody_->setOwnerListener(this);
				rigidbody_->setRestitution(restitution_);
				rigidbody_->setDynamicFriction(dynamicFriction_);
				rigidbody_->setStaticFriction(staticFriction_);
				rigidbody_->setGroup(this->getGameObject()->getLayer());
				rigidbody_->setGroupMask(groupMask_);
				rigidbody_->setKinematic(isKinematic_);
				rigidbody_->setSleepThreshold(sleepThreshold_);
				rigidbody_->setSolverIterationCounts(minPositionIters_, minVelocityIters_);
				rigidbody_->setEnableCCD(enableCCD_);

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