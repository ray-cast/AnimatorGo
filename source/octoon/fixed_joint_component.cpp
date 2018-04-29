#include <octoon/fixed_joint_component.h>
#include <octoon/rigidbody_component.h>


namespace octoon
{
    OctoonImplementSubClass(FixedJoint, AnchoredJoint, "FixedJoint")

    FixedJoint::FixedJoint() noexcept
    {
    }

    FixedJoint::~FixedJoint()
    {

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