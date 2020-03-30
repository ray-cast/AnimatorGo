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
		std::shared_ptr<light::PointLight> pointLight_;
	};
}

#endif