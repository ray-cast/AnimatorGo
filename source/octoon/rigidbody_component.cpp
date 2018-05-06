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
		: body(nullptr)
		, isKinematic(true)
		, isNeedUpdate(true)
		, mass(1.0f)
		, massOffset()
    {
    }

	Rigidbody::Rigidbody(bool type) noexcept
		: body(nullptr), isKinematic(type), mass(1.0f), massOffset()
	{
	}

	Rigidbody::Rigidbody(bool type, float mass) noexcept
		: body(nullptr), isKinematic(type), mass(mass), massOffset()
	{
	}

	Rigidbody::Rigidbody(bool type, float mass, const math::Vector3& offset) noexcept
		: body(nullptr), isKinematic(type), mass(mass), massOffset(offset)
	{
	}

	Rigidbody::~Rigidbody()
    {
		body = nullptr;
    }

	GameComponentPtr
	Rigidbody::clone() const noexcept
    {
		auto instance = std::make_shared<Rigidbody>();
		return instance;
    }

    void
    Rigidbody::setAngularVelocity(const math::float3& v) noexcept
    {
		if ( !isKinematic && body)
		{
			physx::PxRigidBody* rigid_body = static_cast<physx::PxRigidBody*>(body);
			rigid_body->setAngularVelocity(physx::PxVec3(v.x, v.y, v.z));
		}
    }

	math::float3
	Rigidbody::getAngularVelocity() const noexcept
    {
		math::float3 retval;
		if (!isKinematic && body)
		{
			physx::PxRigidBody* rigid_body = static_cast<physx::PxRigidBody*>(body);
			physx::PxVec3 v = rigid_body->getAngularVelocity();
			retval.x = v.x;
			retval.y = v.y;
			retval.z = v.z;
		}

		return retval;
    }

    void
    Rigidbody::setMass(float m) noexcept
    {
		if (!isKinematic && body)
		{
			physx::PxRigidBody* rigid_body = static_cast<physx::PxRigidBody*>(body);
			rigid_body->setMass(m);
		}
    }

    float
    Rigidbody::getMass() const noexcept
    {
		float retval;
		if (!isKinematic && body)
		{
			physx::PxRigidBody* rigid_body = static_cast<physx::PxRigidBody*>(body);
			retval = rigid_body->getMass();
		}

		return retval;
    }

	void
	Rigidbody::setMassOffset(math::Vector3 offset) noexcept
	{
		massOffset = offset;
	}

	math::Vector3
	Rigidbody::getMassOffset() const noexcept
	{
		return massOffset;
	}

    void
    Rigidbody::setSleepMode(RigidbodySleepMode mode) noexcept
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

    RigidbodySleepMode
    Rigidbody::getSleepMode() const noexcept
    {
        return sleepMode;
    }

	void
	Rigidbody::setIsKinematic(bool type) noexcept
	{
		if (isKinematic != type)
		{
			isKinematic = type;
			releaseRigidBody();
			buildRigidBody();
		}
	}

	bool
	Rigidbody::getIsKinematic() const noexcept
	{
		return isKinematic;
	}

    void
    Rigidbody::onActivate() except
    {
        this->addComponentDispatch(GameDispatchType::MoveAfter);
		this->addComponentDispatch(GameDispatchType::FrameEnd);

		buildRigidBody();
    }

    void
    Rigidbody::onDeactivate() noexcept
    {
		releaseRigidBody();

		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->removeComponentDispatch(GameDispatchType::FrameEnd);
    }

    void
    Rigidbody::onAttachComponent(const GameComponentPtr& component) noexcept
    {
		if (component->isA<Collider>())
		{
			auto collider = component->downcast_pointer<Collider>();
			if (collider)
				body->attachShape(*collider->getShape()); // increments reference count
		}
    }

    void
    Rigidbody::onDetachComponent(const GameComponentPtr& component) noexcept
    {
		if (component->isA<Collider>())
		{
			auto collider = component->downcast_pointer<Collider>();
			if (collider)
				body->detachShape(*collider->getShape()); // decrements reference count
		}
    }

	void
	Rigidbody::onFrameBegin() except
	{
	}

	void
	Rigidbody::onFrame() except
	{
	}

	void
	Rigidbody::onFrameEnd() except
	{
		if (body)
		{
			isNeedUpdate = false;

			auto transform = body->getGlobalPose();

			auto transformComponent = this->getComponent<TransformComponent>();
			transformComponent->setTranslate(math::float3(transform.p.x, transform.p.y, transform.p.z) - massOffset);
			transformComponent->setQuaternion(math::Quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w));

			isNeedUpdate = true;
		}
	}

	void
	Rigidbody::onMoveAfter() noexcept
	{
		if (body && isNeedUpdate)
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

	void
	Rigidbody::buildRigidBody() except
	{
		if (body)return;
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

		physicsFeature->getScene()->addActor(*body);
	}

	void
	Rigidbody::releaseRigidBody() noexcept
	{
		if (body)
		{
			body->release();
			body = nullptr;
		}
	}
}