#include <octoon/collider_component.h>

namespace octoon
{
	OctoonImplementSubInterface(ColliderComponent, GameComponent, "ColliderComponent")

	ColliderComponent::ColliderComponent() noexcept
	{
	}

	ColliderComponent::~ColliderComponent() noexcept
	{
	}
}