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
		: body(nullptr), bodyType(RigidbodyType::Kinematic), mass(1.0f), massOffset()
    {
    }

	Rigidbody::Rigidbody(RigidbodyType type) noexcept
		: body(nullptr), bodyType(type), mass(1.0f), massOffset()
	{
	}

	Rigidbody::Rigidbody(RigidbodyType type, float mass) noexcept
		: body(nullptr), bodyType(type), mass(mass), massOffset()
	{
	}

	Rigidbody::Rigidbody(RigidbodyType type, float mass, const math::Vector3& offset) noexcept
		: body(nullptr), bodyType(type), mass(mass), massOffset(offset)
	{
	}

	Rigidbody::~Rigidbody()
    {
		body = nullptr;
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

		pose = physx::PxTransform(physx::PxVec3(position.x + massOffset.x, position.y + massOffset.y, position.z + massOffset.z),
			physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));

		if (bodyType == RigidbodyType::Dynamic)
		{
			body = physics_feature->getSDK()->createRigidDynamic(pose);
			if (!body)
				runtime::runtime_error::create("create body failed!");
			physx::PxRigidBody* rigid_body = static_cast<physx::PxRigidBody*>(body);
			physx::PxRigidBodyExt::updateMassAndInertia(*rigid_body, mass);
		}
		else if (bodyType == RigidbodyType::Static)
		{
			body = physics_feature->getSDK()->createRigidStatic(pose);
			if (!body)
				runtime::runtime_error::create("create body failed!");
		}
		else if (bodyType == RigidbodyType::Kinematic)
		{
			body = physics_feature->getSDK()->createRigidDynamic(pose);
			if (!body)
				runtime::runtime_error::create("create body failed!");
			physx::PxRigidBody* rigid_body = static_cast<physx::PxRigidBody*>(body);
			physx::PxRigidBodyExt::updateMassAndInertia(*rigid_body, mass);
		}

		physics_feature->getScene()->addActor(*body);
    }

    void Rigidbody::onDetach() noexcept
    {
        removeComponentDispatch(GameDispatchType::MoveAfter);
		removeComponentDispatch(GameDispatchType::FrameEnd);

		if (body)
		{
			body->release();
			body = nullptr;
		}
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

			auto transform_component = this->getComponent<TransformComponent>();
			transform_component->setTranslate(math::float3(transform.p.x - massOffset.x, transform.p.y - massOffset.y, transform.p.z - massOffset.z));
			transform_component->setQuaternion(math::Quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w));
		}
	}

	void Rigidbody::onMoveBefore() except
	{
		
	}

	void Rigidbody::onMoveAfter() except
	{
		if (body)
		{
			auto transform_component = this->getComponent<TransformComponent>();
			auto translate = transform_component->getTranslate();
			auto rotation = transform_component->getQuaternion();
			body->setGlobalPose(physx::PxTransform(
				translate.x - massOffset.x,
				translate.y - massOffset.y,
				translate.z - massOffset.z,
				physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)));
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