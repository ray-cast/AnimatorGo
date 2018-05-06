#include <octoon/hinge_joint_component.h>
#include <octoon/rigidbody_component.h>

#include <PxPhysicsAPI.h>


namespace octoon
{
	OctoonImplementSubClass(HingeJoint, Joint, "HingeJoint")

	HingeJoint::HingeJoint() noexcept
		: Joint(), limits(), spring(), motor()
		, useLimits(false), useMotor(false), useSpring(false)
    {
    }

	HingeJoint::HingeJoint(const std::shared_ptr<Rigidbody>& connected_body) noexcept
		: Joint(connected_body), limits(), spring(), motor()
		, useLimits(false), useMotor(false), useSpring(false)
	{

	}

    HingeJoint::~HingeJoint()
    {
    }

	GameComponentPtr HingeJoint::clone() const noexcept
	{
		return std::make_shared<HingeJoint>();
	}

	void HingeJoint::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<Rigidbody>())
		{
			buildJoint(component);
		}
	}

	void HingeJoint::onDetachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<Rigidbody>())
		{
			releaseJoint(component);
		}
	}

    void HingeJoint::onJointChange()
    {

    }

    void HingeJoint::onJointEnter()
    {

    }

    void HingeJoint::onJointExit()
    {

    }

	void HingeJoint::buildJoint(const GameComponentPtr& component) except
	{
		if (joint) return;
		auto physics_feature = GameApp::instance()->getFeature<PhysicsFeature>();
		auto actor0 = component->downcast_pointer<Rigidbody>()->getRigidbody();
		auto actor1 = connectedBody->getRigidbody();
		if (!actor0 || !actor1) return;
		physx::PxRevoluteJoint* revolute_joint = physx::PxRevoluteJointCreate(*physics_feature->getSDK(),
			actor0, physx::PxTransform(anchor.x, anchor.y, anchor.z),
			actor1, physx::PxTransform(connectedAnchor.x, connectedAnchor.y, connectedAnchor.z));

		// spring and limit
		physx::PxJointAngularLimitPair limitPair(limits.min, limits.max, physx::PxSpring(spring.spring, spring.damper));
		revolute_joint->setLimit(limitPair);
		revolute_joint->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eLIMIT_ENABLED, true);

		// motor
		revolute_joint->setDriveVelocity(motor.targetVelocity);
		revolute_joint->setDriveForceLimit(motor.force);
		revolute_joint->setConstraintFlag(physx::PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES, true);
		revolute_joint->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eDRIVE_ENABLED, true);
		if (motor.freeSpin)
			revolute_joint->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eDRIVE_FREESPIN, true);
		else
			revolute_joint->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eDRIVE_FREESPIN, false);

		joint = revolute_joint;
	}

	void HingeJoint::releaseJoint(const GameComponentPtr& component) except
	{
		if (joint)
		{
			joint->release();
			joint = nullptr;
		}
	}
}