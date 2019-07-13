#include <octoon/rigidbody_component.h>
#include <octoon/collider_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_app.h>
#include <octoon/runtime/except.h>

namespace octoon
{
    OctoonImplementSubClass(Rigidbody, GameComponent, "Rigidbody")

    Rigidbody::Rigidbody() noexcept
    {
    }

	Rigidbody::Rigidbody(bool type) noexcept
	{
	}

	Rigidbody::Rigidbody(bool type, float mass) noexcept
	{
	}

	Rigidbody::~Rigidbody()
    {
    }

	GameComponentPtr Rigidbody::clone() const noexcept
    {
		auto instance = std::make_shared<Rigidbody>();
		return instance;
    }

    void Rigidbody::setAngularVelocity(float v) noexcept
    {
        //angularVelocity = v;
    }

    float Rigidbody::getAngularVelocity() const noexcept
    {
        //return angularVelocity;
		return 0;
    }

    void Rigidbody::setGravityScale(float scale) noexcept
    {
        //gravityScale = scale;
    }

    float Rigidbody::getGravityScale() const noexcept
    {
        //return gravityScale;
		return 0;
    }

    void Rigidbody::setMass(float m) noexcept
    {
        //mass = m;
    }

    float Rigidbody::getMass() const noexcept
    {
        //return mass;
		return 0;
    }

    void Rigidbody::setSleepMode(RigidbodySleepMode mode) noexcept
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

    RigidbodySleepMode Rigidbody::getSleepMode() const noexcept
    {
        //return sleepMode;
		return RigidbodySleepMode::NeverSleep;
    }

	void Rigidbody::setIsKinematic(bool type) noexcept
	{
		//isKinematic = type;
	}

	bool Rigidbody::getIsKinematic() const noexcept
	{
		//return isKinematic;
		return true;
	}

    void Rigidbody::onActivate() except
    {
        this->addComponentDispatch(GameDispatchType::MoveAfter);
		this->addComponentDispatch(GameDispatchType::FrameEnd);

		buildRigidBody();
    }

    void Rigidbody::onDeactivate() noexcept
    {
		releaseRigidBody();

		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->removeComponentDispatch(GameDispatchType::FrameEnd);
    }

    void Rigidbody::onAttachComponent(const GameComponentPtr& component) noexcept
    {
		//if (component->isA<Collider>())
		//{
		//	auto collider = component->downcast_pointer<Collider>();
		//	if (collider)
		//		body->attachShape(*collider->getShape()); // increments reference count
		//}
    }

    void Rigidbody::onDetachComponent(const GameComponentPtr& component) noexcept
    {
		//if (component->isA<Collider>())
		//{
		//	auto collider = component->downcast_pointer<Collider>();
		//	if (collider)
		//		body->detachShape(*collider->getShape()); // decrements reference count
		//}
    }

	void Rigidbody::onFrameBegin() except
	{
	}

	void Rigidbody::onFrame() except
	{
	}

	void Rigidbody::onFrameEnd() except
	{
		/*if (body)
		{
			auto transform = body->getGlobalPose();

			auto transformComponent = this->getComponent<TransformComponent>();
			transformComponent->setTranslate(math::float3(transform.p.x, transform.p.y, transform.p.z) - massOffset);
			transformComponent->setQuaternion(math::Quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w));
		}*/
	}

	void Rigidbody::onMoveAfter() noexcept
	{
		/*if (body)
		{
			auto transform = this->getComponent<TransformComponent>();
			auto& translate = transform->getTranslate();
			auto& rotation = transform->getQuaternion();

			physx::PxTransform pose;
			pose.p = physx::PxVec3(translate.x + massOffset.x, translate.y + massOffset.y, translate.z + massOffset.z);
			pose.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);

			body->setGlobalPose(pose);
		}*/
	}

	void Rigidbody::buildRigidBody() except
	{
		/*if (body)return;
		auto physicsFeature = GameApp::instance()->getFeature<PhysicsFeature>();

		auto transform = this->getComponent<TransformComponent>();

		if (!transform) return;
		auto& translate = transform->getTranslate();
		auto& rotation = transform->getQuaternion();

		physx::PxTransform pose;
		pose.p = physx::PxVec3(translate.x + massOffset.x, translate.y + massOffset.y, translate.z + massOffset.z);
		pose.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);

		if (isKinematic)
		{
			body = physicsFeature->getSDK()->createRigidStatic(pose);
			if (!body)
				throw  runtime::runtime_error::create("create body failed!");
		}
		else
		{
			body = physicsFeature->getSDK()->createRigidDynamic(pose);
			if (!body)
				throw  runtime::runtime_error::create("create body failed!");

			physx::PxRigidBody* rigid_body = static_cast<physx::PxRigidBody*>(body);
			physx::PxRigidBodyExt::updateMassAndInertia(*rigid_body, mass);
		}

		physicsFeature->getScene()->addActor(*body);*/
	}

	void Rigidbody::releaseRigidBody() noexcept
	{
		/*if (body)
		{
			body->release();
			body = nullptr;
		}*/
	}
}