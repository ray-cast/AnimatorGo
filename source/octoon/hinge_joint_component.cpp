#include <octoon/hinge_joint_component.h>
#include <octoon/rigidbody_component.h>

#include <PxPhysicsAPI.h>


namespace octoon
{
	OctoonImplementSubClass(HingeJoint, Joint, "HingeJoint")

	HingeJoint::HingeJoint() noexcept
		: Joint()
    {
    }

	HingeJoint::HingeJoint(const std::shared_ptr<Rigidbody>& connected_body) noexcept
		: Joint(connected_body)
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

		physx::PxJointAngularLimitPair limitPair(minDistance, maxDistance, physx::PxSpring(spring, damper));
		revolute_joint->setLimit(limitPair);
		revolute_joint->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eLIMIT_ENABLED, true);
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