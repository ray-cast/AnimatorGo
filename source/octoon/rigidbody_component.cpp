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
    {
    }

	RigidbodyComponent::RigidbodyComponent(bool type) noexcept
	{
	}

	RigidbodyComponent::RigidbodyComponent(bool type, float mass) noexcept
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

    void RigidbodyComponent::setAngularVelocity(float v) noexcept
    {
        //angularVelocity = v;
    }

    float RigidbodyComponent::getAngularVelocity() const noexcept
    {
        //return angularVelocity;
		return 0;
    }

    void RigidbodyComponent::setGravityScale(float scale) noexcept
    {
        //gravityScale = scale;
    }

    float RigidbodyComponent::getGravityScale() const noexcept
    {
        //return gravityScale;
		return 0;
    }

    void RigidbodyComponent::setMass(float m) noexcept
    {
        //mass = m;
    }

    float RigidbodyComponent::getMass() const noexcept
    {
        //return mass;
		return 0;
    }

    void RigidbodyComponent::setSleepMode(RigidbodySleepMode mode) noexcept
    {
        /*sleepMode = mode;
        if(sleepMode == RigidbodySleepMode::NeverSleep)
        {
        }
        else if(sleepMode == RigidbodySleepMode::StartAsleep)
        {
        }
        else if(sleepMode == RigidbodySleepMode::StartAwake)
        {
        }*/
    }

    RigidbodySleepMode RigidbodyComponent::getSleepMode() const noexcept
    {
        //return sleepMode;
		return RigidbodySleepMode::NeverSleep;
    }

	void RigidbodyComponent::setIsKinematic(bool type) noexcept
	{
		//isKinematic = type;
	}

	bool RigidbodyComponent::getIsKinematic() const noexcept
	{
		//return isKinematic;
		return true;
	}

	std::shared_ptr<physics::PhysicsRigidbody> RigidbodyComponent::getRigidbody()
	{
		return rigidbody;
	}

    void RigidbodyComponent::onActivate() except
    {
        this->addComponentDispatch(GameDispatchType::MoveAfter);
		this->addComponentDispatch(GameDispatchType::FrameEnd);

		auto physicsFeature = this->getGameObject()->getGameScene()->getFeature<PhysicsFeature>();
		auto physicsContext = physicsFeature->getContext();
		rigidbody = physicsContext->createRigidbody(physics::PhysicsRigidbodyDesc());
    }

    void RigidbodyComponent::onDeactivate() noexcept
    {
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->removeComponentDispatch(GameDispatchType::FrameEnd);
    }

    void RigidbodyComponent::onAttachComponent(const GameComponentPtr& component) noexcept
    {
		//if (component->isA<Collider>())
		//{
		//	auto collider = component->downcast_pointer<Collider>();
		//	if (collider)
		//		body->attachShape(*collider->getShape()); // increments reference count
		//}
    }

    void RigidbodyComponent::onDetachComponent(const GameComponentPtr& component) noexcept
    {
		//if (component->isA<Collider>())
		//{
		//	auto collider = component->downcast_pointer<Collider>();
		//	if (collider)
		//		body->detachShape(*collider->getShape()); // decrements reference count
		//}
    }

	void RigidbodyComponent::onFrameEnd() except
	{
		if (rigidbody)
		{
			auto position = rigidbody->getPosition();
			auto rotation = rigidbody->getRotation();

			auto transformComponent = this->getComponent<TransformComponent>();
			transformComponent->setTranslate(position);
			transformComponent->setQuaternion(rotation);
		}
	}

	void RigidbodyComponent::onMoveAfter() noexcept
	{
		if (rigidbody)
		{
			auto transform = this->getComponent<TransformComponent>();
			auto& translate = transform->getTranslate();
			auto& rotation = transform->getQuaternion();

			rigidbody->setPositionAndRotation(translate, rotation);
		}
	}
}