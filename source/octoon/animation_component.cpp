#include <octoon/animation_component.h>

namespace octoon
{
	OctoonImplementSubInterface(AnimationComponent, GameComponent, "Animation")

	AnimationComponent::AnimationComponent() noexcept
	{
	}

	AnimationComponent::~AnimationComponent() noexcept
	{
	}
}