#ifndef OCTOON_DIRECTIONAL_LIGHT_COMPONENT_H_
#define OCTOON_DIRECTIONAL_LIGHT_COMPONENT_H_

#include <octoon/light_component.h>
#include <octoon/light/directional_light.h>

namespace octoon
{
	class OCTOON_EXPORT DirectionalLightComponent final : public LightComponent
	{
		OctoonDeclareSubInterface(DirectionalLightComponent, LightComponent)
	public:
		DirectionalLightComponent() noexcept;
		virtual ~DirectionalLightComponent() noexcept;

		void setIntensity(float value) noexcept override;
		void setColor(const math::float3& value) noexcept override;

		void setShadowEnable(bool enable) noexcept;
		bool getShadowEnable() const noexcept;

		void setShadowBias(float bias) noexcept;
		float getShadowBias() const noexcept;

		void setShadowRadius(float radius) noexcept;
		float getShadowRadius() const noexcept;

		void setShadowMapSize(const math::float2& size) noexcept;
		const math::float2& getShadowMapSize() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveAfter() noexcept override;

		void onLayerChangeAfter() noexcept override;

	private:
		DirectionalLightComponent(const DirectionalLightComponent&) = delete;
		DirectionalLightComponent& operator=(const DirectionalLightComponent&) = delete;

	private:
		bool shadowEnable_;

		float shadowBias_;
		float shadowRadius_;
		math::float2 shadowMapSize_;
		std::shared_ptr<light::DirectionalLight> directionalLight_;
	};
}

#endif