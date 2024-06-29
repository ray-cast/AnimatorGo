#include <octoon/collider_component.h>

namespace octoon
{
	OctoonImplementSubInterface(ColliderComponent, GameComponent, "Collider")

	ColliderComponent::ColliderComponent() noexcept
	{
	}

	ColliderComponent::~ColliderComponent() noexcept
	{
	}
}