#include <octoon/rigidbody_component.h>
#include <octoon/collider_component.h>
#include <octoon/transform_component.h>
#include <octoon/physics_feature.h>

namespace octoon
{
    OctoonImplementSubClass(RigidbodyComponent, GameComponent, "RigidbodyComponent")

    RigidbodyComponent::RigidbodyComponent() noexcept
		: isKinematic_(false)
		, gravityScale_(1.0)
		, mass_(1.0)
		, dynamicFriction_(0.5f)
		, staticFriction_(0.5f)
		, restitution_(0.0f)
		, linearDamping_(0.5f)
		, angularDamping_(0.5f)
		, group_(0)
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
	RigidbodyComponent::setPositionAndRotation(const math::float3& position, const math::Quaternion& quat) noexcept
	{
		if (position_ != position || rotation_ != quat)
		{
			if (rigidbody_)
				rigidbody_->setPositionAndRotation(position, quat);

			rotation_ = quat;
			position_ = position;
		}
	}

	void
	RigidbodyComponent::setGroup(std::uint8_t group) noexcept
	{
		if (rigidbody_)
			rigidbody_->setGroup(group);
		group_ = group;
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
	RigidbodyComponent::setGravityScale(float gravityScale) noexcept
	{
		gravityScale_ = gravityScale;
	}

	void
	RigidbodyComponent::setLinearVelocity(const math::float3& linearVelocity) noexcept
	{
		if (rigidbody_)
			rigidbody_->setLinearVelocity(linearVelocity);
	}
	
	void
	RigidbodyComponent::setAngularVelocity(const math::float3& angularVelocity) noexcept
	{
		if (rigidbody_)
			rigidbody_->setAngularVelocity(angularVelocity);
	}

	void
	RigidbodyComponent::setInterpolationLinearVelocity(const math::float3& value) noexcept
	{
		if (rigidbody_)
			rigidbody_->setAngularVelocity(value);
	}

	void
	RigidbodyComponent::setInterpolationAngularVelocity(const math::float3& value) noexcept
	{
		if (rigidbody_)
			rigidbody_->setAngularVelocity(value);
	}

	void
	RigidbodyComponent::setMass(float mass) noexcept
	{
		if (mass_ != mass)
		{
			if (rigidbody_)
				rigidbody_->setMass(mass);

			mass_ = mass;
		}
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

	float
	RigidbodyComponent::getLinearDamping() const noexcept
	{
		return linearDamping_;
	}

	float
	RigidbodyComponent::getAngularDamping() const noexcept
	{
		return angularDamping_;
	}

	bool
	RigidbodyComponent::getEnableCCD() const noexcept
	{
		return enableCCD_;
	}

	std::uint32_t
	RigidbodyComponent::getGroup() const noexcept
	{
		return group_;
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

	math::float3
	RigidbodyComponent::getLinearVelocity() const noexcept
	{
		if (rigidbody_)
			rigidbody_->getLinearVelocity();
		return math::float3::Zero;
	}
	
	math::float3
	RigidbodyComponent::getAngularVelocity() const noexcept
	{
		if (rigidbody_)
			rigidbody_->getAngularVelocity();
		return math::float3::Zero;
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
		instance->setMass(this->getMass());
		instance->setGroupMask(this->getGroupMask());
		instance->setDynamicFriction(this->getDynamicFriction());
		instance->setStaticFriction(this->getStaticFriction());
		instance->setRestitution(this->getRestitution());
		instance->setAngularVelocity(this->getAngularVelocity());
		instance->setGravityScale(this->getGravityScale());
		instance->setIsKinematic(this->getIsKinematic());
		instance->setSleepThreshold(this->getSleepThreshold());
		instance->setEnableCCD(this->getEnableCCD());
		return instance;
	}

    void
	RigidbodyComponent::onActivate() except
    {
		this->addComponentDispatch(GameDispatchType::MoveAfter);

		auto collider = this->getComponent<ColliderComponent>();
		if (collider)
			this->initializeRigidbody(*collider);
	}

    void
	RigidbodyComponent::onDeactivate() noexcept
    {
		this->removeComponentDispatch(GameDispatchType::MoveAfter);

		if (rigidbody_)
		{
			auto physicsFeature = this->getFeature<PhysicsFeature>();
			if (physicsFeature)
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
				this->initializeRigidbody(*collider);
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
	RigidbodyComponent::onMoveAfter() noexcept
	{
		if (rigidbody_ && isKinematic_)
		{
			auto transform = this->getComponent<TransformComponent>();

			position_ = transform->getTranslate();
			rotation_ = transform->getRotation();

			rigidbody_->setPositionAndRotation(position_, rotation_);
		}
	}

	void 
	RigidbodyComponent::onFetchResult() noexcept
	{
		if (rigidbody_ && !isKinematic_)
		{
			auto transform = this->getComponent<TransformComponent>();
			if (transform)
			{
				this->position_ = rigidbody_->getPosition();
				this->rotation_ = rigidbody_->getRotation();
				
				transform->setTransform(this->position_, this->rotation_);
			}
		}
	}

	void
	RigidbodyComponent::initializeRigidbody(ColliderComponent& collider) noexcept
	{
		auto physicsFeature = this->tryGetFeature<PhysicsFeature>();
		if (physicsFeature && !rigidbody_)
		{
			auto transform = this->getComponent<TransformComponent>();

			PhysicsRigidbodyDesc desc;
			desc.type = isKinematic_ ? PhysicsRigidbodyType::Static : PhysicsRigidbodyType::Dynamic;
			desc.position = center_ = position_ = transform->getTranslate();
			desc.rotation = quaternion_ = rotation_ = transform->getRotation();
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
				rigidbody_->setGroup(group_);
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