#ifndef OCTOON_ENVIRONMENT_LIGHT_COMPONENT_H_
#define OCTOON_ENVIRONMENT_LIGHT_COMPONENT_H_

#include <octoon/light_component.h>
#include <octoon/light/environment_light.h>

namespace octoon
{
	class OCTOON_EXPORT EnvironmentLightComponent final : public LightComponent
	{
		OctoonDeclareSubInterface(EnvironmentLightComponent, LightComponent)
	public:
		EnvironmentLightComponent() noexcept;
		virtual ~EnvironmentLightComponent() noexcept;

		void setIntensity(float value) noexcept override;
		void setColor(const math::float3& value) noexcept override;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveAfter() noexcept override;

		void onLayerChangeAfter() noexcept override;

	private:
		EnvironmentLightComponent(const EnvironmentLightComponent&) = delete;
		EnvironmentLightComponent& operator=(const EnvironmentLightComponent&) = delete;

	private:
		std::shared_ptr<light::EnvironmentLight> environmentLight_;
	};
}

#endif