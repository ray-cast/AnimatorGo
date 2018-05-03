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

    void Rigidbody::setPosition(math::Vector3 pos) noexcept
    {
		auto transform_component = this->getComponent<TransformComponent>();
		transform_component->setTranslate(pos);
    }

    math::Vector3 Rigidbody::getPosition() const noexcept
    {
		auto transform_component = this->getComponent<TransformComponent>();
		return transform_component->getTranslate();
    }

    void Rigidbody::setRotation(math::Quaternion delta) noexcept
    {
		auto transform_component = this->getComponent<TransformComponent>();
		transform_component->setQuaternion(delta);
    }

	math::Quaternion Rigidbody::getRotation() const noexcept
    {
		auto transform_component = this->getComponent<TransformComponent>();
		return transform_component->getQuaternion();
    }

    void Rigidbody::onAttach() except
    {
        addComponentDispatch(GameDispatchType::MoveAfter);
		addComponentDispatch(GameDispatchType::FrameEnd);

		auto physics_feature = GameApp::instance()->getFeature<PhysicsFeature>();

		// get from transform
		auto transform_component = this->getComponent<TransformComponent>();
		math::Vector3 position = transform_component->getTranslate();
		math::Quaternion rotation = transform_component->getQuaternion();

		physx::PxTransform pose;

		pose = physx::PxTransform(physx::PxVec3(position.x, position.y, position.z), physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));

		body = physics_feature->getSDK()->createRigidDynamic(pose);
		if (!body)
			runtime::runtime_error::create("create body failed!");

		physx::PxRigidBodyExt::updateMassAndInertia(*body, 1);

		physics_feature->getScene()->addActor(*body);
    }

    void Rigidbody::onDetach() noexcept
    {
        removeComponentDispatch(GameDispatchType::MoveAfter);
		removeComponentDispatch(GameDispatchType::FrameEnd);

		body->release();
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
		auto transform = body->getGlobalPose();

		auto transform_component = this->getComponent<TransformComponent>();
		transform_component->setTranslate(math::float3(transform.p.x, transform.p.y, transform.p.z));
		transform_component->setQuaternion(math::Quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w));
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