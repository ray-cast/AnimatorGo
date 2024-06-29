#include "light_component.h"
#include "../unreal_profile.h"

#include <octoon/point_light_component.h>
#include <octoon/spot_light_component.h>
#include <octoon/directional_light_component.h>

namespace unreal
{
	LightComponent::LightComponent() noexcept
	{
	}

	LightComponent::~LightComponent() noexcept
	{
	}

	bool
	LightComponent::createLight(LightType type) noexcept
	{
		if (type == LightType::Point)
		{
			auto light = std::make_shared<octoon::GameObject>(std::string_view("PointLight"));
			light->addComponent<octoon::PointLightComponent>();
			this->getModel()->objects.getValue().push_back(light);
			return true;
		}
		else if (type == LightType::Spot)
		{
			auto light = std::make_shared<octoon::GameObject>(std::string_view("SpotLight"));
			light->addComponent<octoon::SpotLightComponent>();
			this->getModel()->objects.getValue().push_back(light);
			return true;
		}
		else if (type == LightType::Directional)
		{
			auto light = std::make_shared<octoon::GameObject>(std::string_view("DirectionalLight"));
			light->addComponent<octoon::DirectionalLightComponent>();
			this->getModel()->objects.getValue().push_back(light);
			return true;
		}

		return false;
	}
}