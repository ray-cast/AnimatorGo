#include <octoon/joint_component.h>

namespace octoon
{
	OctoonImplementSubInterface(JointComponent, GameComponent, "JointComponent")

	JointComponent::JointComponent() noexcept
	{
	}

	JointComponent::~JointComponent()
	{
	}
}