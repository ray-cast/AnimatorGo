#include "offline_component.h"
#include "../libs/nativefiledialog/nfd.h"
#include "../rabbit_profile.h"
#include "../rabbit_behaviour.h"

#include <octoon/offline_camera_component.h>
#include <octoon/perspective_camera_component.h>
#include <octoon/offline_skinned_mesh_renderer_component.h>

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
			if (this->getContext()->profile->entitiesModule->camera)
			{
				this->getContext()->profile->entitiesModule->camera->getComponent<octoon::OfflineCameraComponent>()->setActive(active);
				this->getContext()->profile->entitiesModule->camera->getComponent<octoon::PerspectiveCameraComponent>()->setActive(!active);
			}

			for (auto& it : this->getContext()->profile->entitiesModule->objects)
			{
				for (auto& child : it->getChildren())
				{
					auto meshRenderer = child->getComponent<octoon::OfflineSkinnedMeshRendererComponent>();
					if (meshRenderer)
						meshRenderer->setActive(active);
				}
			}

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