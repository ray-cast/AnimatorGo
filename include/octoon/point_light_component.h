#ifndef OCTOON_POINT_LIGHT_COMPONENT_H_
#define OCTOON_POINT_LIGHT_COMPONENT_H_

#include <octoon/light_component.h>
#include <octoon/light/point_light.h>

namespace octoon
{
	class OCTOON_EXPORT PointLightComponent final : public LightComponent
	{
		OctoonDeclareSubInterface(PointLightComponent, LightComponent)
	public:
		PointLightComponent() noexcept;
		virtual ~PointLightComponent() noexcept;

		void setIntensity(float value) noexcept override;
		void setColor(const math::float3& value) noexcept override;

		void setShadowEnable(bool enable) noexcept;
		bool getShadowEnable() const noexcept;

		void setShadowBias(float bias) noexcept;
		float getShadowBias() const noexcept;

		void setShadowRadius(float radius) noexcept;
		float getShadowRadius() const noexcept;

		void setShadowMapSize(const math::uint2& size) noexcept;
		const math::uint2& getShadowMapSize() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveAfter() noexcept override;

		void onLayerChangeAfter() noexcept override;

	private:
		PointLightComponent(const PointLightComponent&) = delete;
		PointLightComponent& operator=(const PointLightComponent&) = delete;

	private:
		bool shadowEnable_;

		float shadowBias_;
		float shadowRadius_;
		math::uint2 shadowMapSize_;
		std::shared_ptr<light::PointLight> pointLight_;
	};
}

#endif