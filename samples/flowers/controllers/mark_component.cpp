#include "mark_component.h"

namespace flower
{
	MarkComponent::MarkComponent() noexcept
	{
	}

	MarkComponent::~MarkComponent() noexcept
	{
	}

	void
	MarkComponent::setActive(bool active) noexcept
	{
		if (this->getModel()->markEnable != active)
			this->getModel()->markEnable = active;
	}

	bool
	MarkComponent::getActive() const noexcept
	{
		return this->getModel()->markEnable;
	}
}