#include <octoon/fixed_joint_component.h>
#include <octoon/rigidbody_component.h>


namespace octoon
{
    OctoonImplementSubClass(FixedJoint, Joint, "FixedJoint")

    FixedJoint::FixedJoint() noexcept
    {
    }

    FixedJoint::~FixedJoint()
    {

    }

	GameComponentPtr FixedJoint::clone() const noexcept
	{
		return std::make_shared<FixedJoint>();
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
}