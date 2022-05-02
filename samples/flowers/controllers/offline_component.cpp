#include "offline_component.h"
#include "../libs/nativefiledialog/nfd.h"
#include "../flower_profile.h"
#include "../flower_behaviour.h"

namespace flower
{
	OfflineComponent::OfflineComponent() noexcept
	{
	}

	OfflineComponent::~OfflineComponent() noexcept
	{
	}

	void
	OfflineComponent::setActive(bool active) noexcept
	{
		if (this->getModel()->offlineEnable != active)
		{
			for (auto& object : this->getContext()->profile->entitiesModule->objects)
			{
				auto smr = object->getComponent<octoon::SkinnedMeshRendererComponent>();
				if (smr)
					smr->setAutomaticUpdate(!active);
			}

			this->getFeature<octoon::VideoFeature>()->setGlobalIllumination(active);
			this->getModel()->offlineEnable = active;

			this->sendMessage("flower:offline", active);
		}
	}

	bool
	OfflineComponent::getActive() const noexcept
	{
		return this->getModel()->offlineEnable;
	}

	void
	OfflineComponent::onEnable() noexcept
	{
	}

	void
	OfflineComponent::onDisable() noexcept
	{
	}


	void
	OfflineComponent::setMaxBounces(std::uint32_t num_bounces)
	{
		if (this->getModel()->bounces != num_bounces)
		{
			this->getFeature<octoon::VideoFeature>()->setMaxBounces(num_bounces);
			this->getModel()->bounces = num_bounces;
		}		
	}

	std::uint32_t
	OfflineComponent::getMaxBounces() const
	{
		return this->getModel()->bounces;
	}
}