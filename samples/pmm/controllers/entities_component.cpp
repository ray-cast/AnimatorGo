#include "entities_component.h"
#include <octoon/animator_component.h>

namespace MysticLit
{
	EntitiesComponent::EntitiesComponent() noexcept
	{
	}

	EntitiesComponent::~EntitiesComponent() noexcept
	{
	}

	void
	EntitiesComponent::setActive(bool active) noexcept
	{
	}

	bool
	EntitiesComponent::getActive() const noexcept
	{
		return true;
	}

	void
	EntitiesComponent::play() noexcept
	{
		for (auto& it : this->getModel()->objects)
		{
			for (auto& it : it->getComponents())
			{
				if (it->isA<octoon::AnimatorComponent>())
					it->downcast<octoon::AnimatorComponent>()->play();
			}
		}
	}

	void
	EntitiesComponent::stop() noexcept
	{
		for (auto& it : this->getModel()->objects)
		{
			for (auto& it : it->getComponents())
			{
				if (it->isA<octoon::AnimatorComponent>())
					it->downcast<octoon::AnimatorComponent>()->stop();
			}
		}
	}
}