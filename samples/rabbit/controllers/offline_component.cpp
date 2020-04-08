#include "offline_component.h"
#include "../libs/nativefiledialog/nfd.h"
#include "../rabbit_profile.h"
#include "../rabbit_behaviour.h"

namespace rabbit
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
			this->getFeature<octoon::VideoFeature>()->setGlobalIllumination(active);
			this->getModel()->offlineEnable = active;
			this->sendMessage("rabbit:offline", active);
		}
	}

	bool
	OfflineComponent::getActive() const noexcept
	{
		return this->getModel()->offlineEnable;
	}
}