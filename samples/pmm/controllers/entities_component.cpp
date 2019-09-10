#include "entities_component.h"
#include <octoon/animator_component.h>
#include <octoon/rigidbody_component.h>
#include <octoon/transform_component.h>

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
			for (auto& component : it->getComponents())
			{
				if (component->isA<octoon::AnimatorComponent>())
					component->downcast<octoon::AnimatorComponent>()->play();
			}
		}
	}

	void
	EntitiesComponent::pause() noexcept
	{
		for (auto& it : this->getModel()->objects)
		{
			for (auto& component : it->getComponents())
			{
				if (component->isA<octoon::AnimatorComponent>())
					component->downcast<octoon::AnimatorComponent>()->pause();
			}
		}
	}

	void
	EntitiesComponent::reset() noexcept
	{
		for (auto& it : this->getModel()->objects)
		{
			for (auto& component : it->getComponents())
			{
				if (component->isA<octoon::AnimatorComponent>())
				{
					auto animator = component->downcast<octoon::AnimatorComponent>();
					animator->reset();
					animator->sample();

					auto& avatar = animator->getAvatar();
					for (auto& bone : avatar)
					{
						for (auto& child : bone->getChildren())
						{
							auto rigidbody = child->getComponent<octoon::RigidbodyComponent>();
							if (rigidbody)
							{
								auto transform = rigidbody->getComponent<octoon::TransformComponent>();
								rigidbody->movePosition(transform->getTranslate());
								rigidbody->rotation(transform->getQuaternion());
							}
						}
					}
				}
			}
		}
	}

	void
	EntitiesComponent::sample(float delta) noexcept
	{
		for (auto& it : this->getModel()->objects)
		{
			for (auto& component : it->getComponents())
			{
				if (component->isA<octoon::AnimatorComponent>())
				{
					auto animator = component->downcast<octoon::AnimatorComponent>();
					animator->sample(delta);

					auto& avatar = animator->getAvatar();
					for (auto& bone : avatar)
					{
						for (auto& child : bone->getChildren())
						{
							auto rigidbody = child->getComponent<octoon::RigidbodyComponent>();
							if (rigidbody)
							{
								auto transform = rigidbody->getComponent<octoon::TransformComponent>();
								rigidbody->movePosition(transform->getTranslate());
								rigidbody->rotation(transform->getQuaternion());
							}
						}
					}
				}
			}
		}
	}
}