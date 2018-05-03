#include <octoon/rigidbody_component.h>
#include <octoon/collider_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_app.h>
#include <octoon/runtime/except.h>

#include <PxPhysicsAPI.h>

namespace octoon
{
    OctoonImplementSubClass(Rigidbody, GameComponent, "Rigidbody")

    Rigidbody::Rigidbody() noexcept
		: bodyType(RigidbodyType::Dynamic)
		, mass(1.0f)
		, massOffset()
		, body(nullptr)
    {
    }

	Rigidbody::Rigidbody(RigidbodyType type) noexcept
		: bodyType(type), mass(1.0f), massOffset()
	{
	}

	Rigidbody::Rigidbody(RigidbodyType type, float mass) noexcept
		: bodyType(type), mass(mass), massOffset()
	{
	}

	Rigidbody::Rigidbody(RigidbodyType type, float mass, const math::Vector3& offset) noexcept
		: bodyType(type), mass(mass), massOffset(offset)
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
        angularVelocity = v;
    }

    float Rigidbody::getAngularVelocity() const noexcept
    {
        return angularVelocity;
    }

    void Rigidbody::setGravityScale(float scale) noexcept
    {
        gravityScale = scale;
    }

    float Rigidbody::getGravityScale() const noexcept
    {
        return gravityScale;
    }

    void Rigidbody::setMass(float m) noexcept
    {
        mass = m;
    }

    float Rigidbody::getMass() const noexcept
    {
        return mass;
    }

	void Rigidbody::setMassOffset(math::Vector3 offset) noexcept
	{
		massOffset = offset;
	}

	math::Vector3 Rigidbody::getMassOffset() const noexcept
	{
		return massOffset;
	}

    void Rigidbody::setSleepMode(RigidbodySleepMode mode) noexcept
    {
        sleepMode = mode;
        if(sleepMode == RigidbodySleepMode::NeverSleep)
        {
        }
        else if(sleepMode == RigidbodySleepMode::StartAsleep)
        {
        }
        else if(sleepMode == RigidbodySleepMode::StartAwake)
        {
        }
    }

    RigidbodySleepMode Rigidbody::getSleepMode() const noexcept
    {
        return sleepMode;
    }

    void Rigidbody::setBodyType(RigidbodyType type) noexcept
    {
        bodyType = type;
    }

    RigidbodyType Rigidbody::getBodyType() const noexcept
    {
        return bodyType;
    }

    void Rigidbody::onActivate() except
    {
        this->addComponentDispatch(GameDispatchType::MoveAfter);
		this->addComponentDispatch(GameDispatchType::FrameEnd);

		auto physicsFeature = GameApp::instance()->getFeature<PhysicsFeature>();

		auto transform = this->getComponent<TransformComponent>();
		auto& translate = transform->getTranslate();
		auto& rotation = transform->getQuaternion();

		physx::PxTransform pose;
		pose.p = physx::PxVec3(translate.x - massOffset.x, translate.y - massOffset.y, translate.z - massOffset.z);
		pose.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);

		switch (bodyType)
		{
		case RigidbodyType::Dynamic:
		{
			body = physicsFeature->getSDK()->createRigidDynamic(pose);
			if (!body)
				throw  runtime::runtime_error::create("create body failed!");

			physx::PxRigidBody* rigid_body = static_cast<physx::PxRigidBody*>(body);
			physx::PxRigidBodyExt::updateMassAndInertia(*rigid_body, mass);
		}
		break;
		case RigidbodyType::Static:
		case RigidbodyType::Kinematic:
		{
			body = physicsFeature->getSDK()->createRigidStatic(pose);
			if (!body)
				throw  runtime::runtime_error::create("create body failed!");
		}
		break;
		default:
			throw runtime::type_error::create("Invalid enum type of rigidbody");
		}

		physicsFeature->getScene()->addActor(*body);
    }

    void Rigidbody::onDeactivate() noexcept
    {
		if (body)
		{
			body->release();
			body = nullptr;
		}

		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->removeComponentDispatch(GameDispatchType::FrameEnd);
    }

    void Rigidbody::onAttachComponent(const GameComponentPtr& component) noexcept
    {
        //if (component->isA<Collider>())
		//    component->downcast<Collider>()->addShapeChangeListener(&OnCollisionChange);
    }

    void Rigidbody::onDetachComponent(const GameComponentPtr& component) noexcept
    {
        //if (component->isA<Collider>())
		//    component->downcast<Collider>()->removeShapeChangeListener(&OnCollisionChange);
    }

	void Rigidbody::onFrameBegin() except
	{
	}

	void Rigidbody::onFrame() except
	{
	}

	void Rigidbody::onFrameEnd() except
	{
		if (body)
		{
			auto transform = body->getGlobalPose();

			auto transformComponent = this->getComponent<TransformComponent>();
			transformComponent->setTranslate(math::float3(transform.p.x, transform.p.y, transform.p.z) - massOffset);
			transformComponent->setQuaternion(math::Quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w));
		}
	}

	void Rigidbody::onMoveAfter() noexcept
	{
		if (body)
		{
			auto transform = this->getComponent<TransformComponent>();
			auto& translate = transform->getTranslate();
			auto& rotation = transform->getQuaternion();

			physx::PxTransform pose;
			pose.p = physx::PxVec3(translate.x + massOffset.x, translate.y + massOffset.y, translate.z + massOffset.z);
			pose.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);

			body->setGlobalPose(pose);
		}
	}

    void Rigidbody::rigidbodyEnter() noexcept
    {
    }

    void Rigidbody::rigidbodyChange() noexcept
    {
    }

    void Rigidbody::rigidbodyExit() noexcept
    {
    }
}