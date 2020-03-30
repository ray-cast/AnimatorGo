#ifndef OCTOON_RECTANGLE_LIGHT_COMPONENT_H_
#define OCTOON_RECTANGLE_LIGHT_COMPONENT_H_

#include <octoon/light_component.h>
#include <octoon/light/rectangle_light.h>

namespace octoon
{
	class OCTOON_EXPORT RectangleLightComponent final : public LightComponent
	{
		OctoonDeclareSubInterface(RectangleLightComponent, LightComponent)
	public:
		RectangleLightComponent() noexcept;
		virtual ~RectangleLightComponent() noexcept;

		void setIntensity(float value) noexcept override;
		void setColor(const math::float3& value) noexcept override;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveAfter() noexcept override;

		void onLayerChangeAfter() noexcept override;

	private:
		RectangleLightComponent(const RectangleLightComponent&) = delete;
		RectangleLightComponent& operator=(const RectangleLightComponent&) = delete;

	private:
		std::shared_ptr<light::RectangleLight> rectangleLight_;
	};
}

#endif