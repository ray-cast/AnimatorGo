#include "main_light_component.h"
#include "../unreal_profile.h"
#include "../unreal_behaviour.h"

namespace unreal
{
	MainLightComponent::MainLightComponent() noexcept
	{
	}

	MainLightComponent::~MainLightComponent() noexcept
	{
	}

	void
	MainLightComponent::onInit() noexcept
	{
		this->getModel()->size += [this](float value)
		{
			auto& profile = this->getContext()->profile;

			auto& mainLight = profile->mainLightModule->mainLight.getValue();
			if (mainLight)
			{
				auto lightComponment = mainLight->getComponent<octoon::DirectionalLightComponent>();
				if (lightComponment)
					lightComponment->setSize(value);
			}
		};

		this->getModel()->intensity += [this](float value)
		{
			auto& profile = this->getContext()->profile;

			auto& mainLight = profile->mainLightModule->mainLight.getValue();
			if (mainLight)
			{
				auto lightComponment = mainLight->getComponent<octoon::DirectionalLightComponent>();
				if (lightComponment)
					lightComponment->setIntensity(value);
			}
		};

		this->getModel()->color += [this](const octoon::math::float3& value)
		{
			auto& profile = this->getContext()->profile;

			auto& mainLight = profile->mainLightModule->mainLight.getValue();
			if (mainLight)
			{
				auto lightComponment = mainLight->getComponent<octoon::DirectionalLightComponent>();
				if (lightComponment)
					lightComponment->setColor(value);
			}
		};

		this->getModel()->rotation += [this](const octoon::math::float3& value)
		{
			auto& profile = this->getContext()->profile;

			auto& mainLight = profile->mainLightModule->mainLight.getValue();
			if (mainLight)
			{
				auto transform = mainLight->getComponent<octoon::TransformComponent>();
				if (transform)
				{
					transform->setRotation(octoon::math::Quaternion(octoon::math::radians(value)));
					transform->setTranslate(-octoon::math::rotate(transform->getRotation(), octoon::math::float3::UnitZ) * 50);
				}
			}
		};
	}

	void
	MainLightComponent::onEnable() noexcept
	{
		auto mainLight = std::make_shared<octoon::GameObject>(std::string_view("DirectionalLight"));
		mainLight->addComponent<octoon::DirectionalLightComponent>();
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setShadowEnable(true);
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setShadowMapSize(octoon::math::uint2(2048, 2048));
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setSize(this->getContext()->profile->mainLightModule->size);
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setIntensity(this->getContext()->profile->mainLightModule->intensity);
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setColor(this->getContext()->profile->mainLightModule->color);
		mainLight->getComponent<octoon::TransformComponent>()->setRotation(octoon::math::Quaternion(octoon::math::radians(this->getContext()->profile->mainLightModule->rotation)));

		this->getContext()->profile->mainLightModule->mainLight = mainLight;
	}

	void
	MainLightComponent::onDisable() noexcept
	{
	}
}