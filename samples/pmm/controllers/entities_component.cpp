#include "entities_component.h"
#include <octoon/animation_component.h>
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
			auto animation = it->getComponent<octoon::AnimationComponent>();
			if (animation)
				animation->play();

			auto animator = it->getComponent<octoon::AnimatorComponent>();
			if (animator)
				animator->play();
		}
	}

	void
	EntitiesComponent::stop() noexcept
	{
		for (auto& it : this->getModel()->objects)
		{
			auto animation = it->getComponent<octoon::AnimationComponent>();
			if (animation)
				animation->stop();

			auto animator = it->getComponent<octoon::AnimatorComponent>();
			if (animator)
				animator->stop();
		}
	}
}