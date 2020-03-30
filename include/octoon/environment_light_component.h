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

		void setIrradiance(const hal::GraphicsTexturePtr& texture) noexcept;
		const hal::GraphicsTexturePtr& getIrradiance() const noexcept;

		void setRadiance(const hal::GraphicsTexturePtr& texture) noexcept;
		const hal::GraphicsTexturePtr& getRadiance() const noexcept;

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
		hal::GraphicsTexturePtr radiance_;
		hal::GraphicsTexturePtr irradiance_;
		std::shared_ptr<light::EnvironmentLight> environmentLight_;
	};
}

#endif