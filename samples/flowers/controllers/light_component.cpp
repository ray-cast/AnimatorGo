#include "light_component.h"
#include "../libs/nativefiledialog/nfd.h"
#include "../flower_profile.h"

#include <octoon/point_light_component.h>
#include <octoon/spot_light_component.h>
#include <octoon/directional_light_component.h>

namespace flower
{
	LightComponent::LightComponent() noexcept
	{
	}

	LightComponent::~LightComponent() noexcept
	{
	}

	void
	LightComponent::setActive(bool active) noexcept
	{
	}

	bool
	LightComponent::getActive() const noexcept
	{
		return true;
	}

	bool
	LightComponent::createLight(LightType type) noexcept
	{
		if (type == LightType::Point)
		{
			auto light = octoon::GameObject::create("PointLight");
			light->addComponent<octoon::PointLightComponent>();
			this->getModel()->objects.push_back(light);
			return true;
		}
		else if (type == LightType::Spot)
		{
			auto light = octoon::GameObject::create("SpotLight");
			light->addComponent<octoon::SpotLightComponent>();
			this->getModel()->objects.push_back(light);
			return true;
		}
		else if (type == LightType::Directional)
		{
			auto light = octoon::GameObject::create("DirectionalLight");
			light->addComponent<octoon::DirectionalLightComponent>();
			this->getModel()->objects.push_back(light);
			return true;
		}

		return false;
	}
}