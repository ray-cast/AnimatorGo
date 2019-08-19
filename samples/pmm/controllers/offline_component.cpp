#include "offline_component.h"
#include "../libs/nativefiledialog/nfd.h"
#include "../mysticLit_profile.h"

#include <octoon/offline_camera_component.h>
#include <octoon/perspective_camera_component.h>
#include <octoon/offline_skinned_mesh_renderer_component.h>

namespace MysticLit
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
			this->getContext()->profile->entitiesModule_->camera->getComponent<octoon::OfflineCameraComponent>()->setActive(active);
			this->getContext()->profile->entitiesModule_->camera->getComponent<octoon::PerspectiveCameraComponent>()->setActive(!active);

			for (auto& it : this->getContext()->profile->entitiesModule_->objects)
			{
				for (auto& child : it->getChildren())
				{
					auto meshRenderer = child->getComponent<octoon::OfflineSkinnedMeshRendererComponent>();
					if (meshRenderer)
						meshRenderer->setActive(active);
				}
			}
		}
	}

	bool
	OfflineComponent::getActive() const noexcept
	{
		return this->getModel()->offlineEnable;
	}
}