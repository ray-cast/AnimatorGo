#include <octoon/spring_joint_component.h>
#include <octoon/rigidbody_component.h>

#include <PxPhysicsAPI.h>


namespace octoon
{
	OctoonImplementSubClass(SpringJoint, Joint, "SpringJoint")

	SpringJoint::SpringJoint() noexcept
		:damper(0.f), minDistance(0.5f), maxDistance(1.0f)
		, spring(20.f), tolerance(0.01f)
    {
    }

	SpringJoint::SpringJoint(const std::shared_ptr<Rigidbody>& connected_body) noexcept
		: Joint(connected_body), damper(0.f), minDistance(0.f)
		, maxDistance(1.f), spring(0.5f), tolerance(0.1f)
	{

	}

    SpringJoint::~SpringJoint()
    {
    }

	GameComponentPtr SpringJoint::clone() const noexcept
	{
		return std::make_shared<SpringJoint>();
	}

	void SpringJoint::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<Rigidbody>())
		{
			buildJoint(component);
		}
	}

	void SpringJoint::onDetachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<Rigidbody>())
		{
			releaseJoint(component);
		}
	}

    void SpringJoint::onJointChange()
    {

    }

    void SpringJoint::onJointEnter()
    {

    }

    void SpringJoint::onJointExit()
    {

    }

	void SpringJoint::buildJoint(const GameComponentPtr& component) except
	{
		if (joint) return;
		auto physics_feature = GameApp::instance()->getFeature<PhysicsFeature>();
		auto actor0 = component->downcast_pointer<Rigidbody>()->getRigidbody();
		auto actor1 = connectedBody->getRigidbody();
		if (!actor0 || !actor1) return;
		physx::PxPrismaticJoint* prismatic_joint = physx::PxPrismaticJointCreate(*physics_feature->getSDK(),
			actor0, physx::PxTransform(anchor.x, anchor.y, anchor.z),
			actor1, physx::PxTransform(connectedAnchor.x, connectedAnchor.y, connectedAnchor.z));

		physx::PxJointLinearLimitPair limitPair(minDistance, maxDistance, physx::PxSpring(spring, damper));
		prismatic_joint->setLimit(limitPair);
		prismatic_joint->setPrismaticJointFlag(physx::PxPrismaticJointFlag::eLIMIT_ENABLED, true);
		joint = prismatic_joint;
	}

	void SpringJoint::releaseJoint(const GameComponentPtr& component) except
	{
		if (joint)
		{
			joint->release();
			joint = nullptr;
		}
	}
}