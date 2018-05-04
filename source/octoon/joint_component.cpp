#include <octoon/joint_component.h>
#include <octoon/rigidbody_component.h>


namespace octoon
{
    OctoonImplementSubInterface(Joint, GameComponent, "Joint")

    Joint::Joint() noexcept
		: joint(nullptr), anchor(0.f, 0.f, 0.f), connectedAnchor(0.f, 0.f, 0.f)
    {
    }

	Joint::Joint(const std::shared_ptr<Rigidbody>& connected_body) noexcept
		: joint(nullptr), anchor(0.f, 0.f, 0.f),
		connectedAnchor(0.f, 0.f, 0.f), connectedBody(connected_body)
	{

	}

	Joint::~Joint()
    {

    }
}