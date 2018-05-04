#include <octoon/fixed_joint_component.h>
#include <octoon/rigidbody_component.h>

#include <PxPhysicsAPI.h>


namespace octoon
{
	OctoonImplementSubClass(FixedJoint, Joint, "FixedJoint")

	FixedJoint::FixedJoint() noexcept
    {
    }

	FixedJoint::FixedJoint(const std::shared_ptr<Rigidbody>& connected_body) noexcept
		: Joint(connected_body)
	{

	}

    FixedJoint::~FixedJoint()
    {
    }

	GameComponentPtr FixedJoint::clone() const noexcept
	{
		return std::make_shared<FixedJoint>();
	}

	void FixedJoint::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<Rigidbody>())
		{
			buildJoint(component);
		}
	}

	void FixedJoint::onDetachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<Rigidbody>())
		{
			releaseJoint(component);
		}
	}

    void FixedJoint::onJointChange()
    {

    }

    void FixedJoint::onJointEnter()
    {

    }

    void FixedJoint::onJointExit()
    {

    }

	void FixedJoint::buildJoint(const GameComponentPtr& component) except
	{
		if (joint) return;
		auto physics_feature = GameApp::instance()->getFeature<PhysicsFeature>();
		auto actor0 = component->downcast_pointer<Rigidbody>()->getRigidbody();
		auto actor1 = connectedBody->getRigidbody();
		if (!actor0 || !actor1) return;
		joint = physx::PxFixedJointCreate(*physics_feature->getSDK(),
			actor0, physx::PxTransform(anchor.x, anchor.y, anchor.z),
			actor1, physx::PxTransform(connectedAnchor.x, connectedAnchor.y, connectedAnchor.z));
	}

	void FixedJoint::releaseJoint(const GameComponentPtr& component) except
	{
		if (joint)
		{
			joint->release();
			joint = nullptr;
		}
	}
}