#include <octoon/rigidbody_component.h>
#include <octoon/collider_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_app.h>
#include <octoon/game_scene.h>
#include <octoon/runtime/except.h>

namespace octoon
{
    OctoonImplementSubClass(RigidbodyComponent, GameComponent, "RigidbodyComponent")

    RigidbodyComponent::RigidbodyComponent() noexcept
		: isKinematic_(false)
		, angularVelocity_(1.0)
		, gravityScale_(1.0)
		, mass_(1.0)
		, dynamicFriction_(1.0)
		, staticFriction_(1.0)
		, restitution_(1.0)
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

	GameComponentPtr RigidbodyComponent::clone() const noexcept
    {
		auto instance = std::make_shared<RigidbodyComponent>();
		return instance;
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
		isKinematic_ = isKinematic;
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

    void
	RigidbodyComponent::onActivate() except
    {
        this->addComponentDispatch(GameDispatchType::MoveAfter);
		this->addComponentDispatch(GameDispatchType::FrameEnd);

		auto physicsFeature = this->getGameScene()->getFeature<PhysicsFeature>();
		auto physicsContext = physicsFeature->getContext();
		rigidbody_ = physicsContext->createRigidbody(physics::PhysicsRigidbodyDesc());

		if (rigidbody_)
		{
			rigidbody_->setRestitution(restitution_);
			rigidbody_->setDynamicFriction(dynamicFriction_);
			rigidbody_->setStaticFriction(staticFriction_);

			physicsFeature->getScene()->addRigidbody(rigidbody_);
		}
    }

    void
	RigidbodyComponent::onDeactivate() noexcept
    {
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->removeComponentDispatch(GameDispatchType::FrameEnd);
    }

	void 
	RigidbodyComponent::onAttach() except
	{
	}

	void 
	RigidbodyComponent::onDetach() noexcept
	{
	}

	void 
	RigidbodyComponent::onAttachComponent(const GameComponentPtr& component) noexcept
    {
		//if (component->isA<Collider>())
		//{
		//	auto collider = component->downcast_pointer<Collider>();
		//	if (collider)
		//		body->attachShape(*collider->getShape()); // increments reference count
		//}
    }

    void 
	RigidbodyComponent::onDetachComponent(const GameComponentPtr& component) noexcept
    {
		//if (component->isA<Collider>())
		//{
		//	auto collider = component->downcast_pointer<Collider>();
		//	if (collider)
		//		body->detachShape(*collider->getShape()); // decrements reference count
		//}
    }

	void 
	RigidbodyComponent::onFrameEnd() except
	{
		if (rigidbody_)
		{
			auto position = rigidbody_->getPosition();
			auto rotation = rigidbody_->getRotation();

			auto transformComponent = this->getComponent<TransformComponent>();
			transformComponent->setTranslate(position);
			transformComponent->setQuaternion(rotation);
		}
	}

	void 
	RigidbodyComponent::onMoveAfter() noexcept
	{
		if (rigidbody_)
		{
			auto transform = this->getComponent<TransformComponent>();
			auto& translate = transform->getTranslate();
			auto& rotation = transform->getQuaternion();

			rigidbody_->setPositionAndRotation(translate, rotation);
		}
	}
}