#include <octoon/joint_component.h>
#include <octoon/rigidbody_component.h>


namespace octoon
{
    OctoonImplementSubInterface(Joint, GameComponent, "Joint")

    Joint::Joint() noexcept
    {
    }

	Joint::~Joint()
    {

    }
}