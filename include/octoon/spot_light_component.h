#ifndef OCTOON_SPOT_LIGHT_COMPONENT_H_
#define OCTOON_SPOT_LIGHT_COMPONENT_H_

#include <octoon/light_component.h>
#include <octoon/light/spot_light.h>

namespace octoon
{
	class OCTOON_EXPORT SpotLightComponent final : public LightComponent
	{
		OctoonDeclareSubInterface(SpotLightComponent, LightComponent)
	public:
		SpotLightComponent() noexcept;
		virtual ~SpotLightComponent() noexcept;

		void setIntensity(float value) noexcept override;
		void setColor(const math::float3& value) noexcept override;

		void setShadowEnable(bool enable) noexcept;
		bool getShadowEnable() const noexcept;

		void setShadowBias(float bias) noexcept;
		float getShadowBias() const noexcept;

		void setShadowMapSize(const math::uint2& size) noexcept;
		const math::uint2& getShadowMapSize() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveAfter() noexcept override;

		void onLayerChangeAfter() noexcept override;

	private:
		SpotLightComponent(const SpotLightComponent&) = delete;
		SpotLightComponent& operator=(const SpotLightComponent&) = delete;

	private:
		bool shadowEnable_;

		float shadowBias_;
		float shadowRadius_;
		math::uint2 shadowMapSize_;
		std::shared_ptr<light::SpotLight> spotLight_;
	};
}

#endif