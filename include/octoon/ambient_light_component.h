#ifndef OCTOON_AMBIENT_LIGHT_COMPONENT_H_
#define OCTOON_AMBIENT_LIGHT_COMPONENT_H_

#include <octoon/light_component.h>
#include <octoon/light/ambient_light.h>

namespace octoon
{
	class OCTOON_EXPORT AmbientLightComponent final : public LightComponent
	{
		OctoonDeclareSubInterface(AmbientLightComponent, LightComponent)
	public:
		AmbientLightComponent() noexcept;
		virtual ~AmbientLightComponent() noexcept;

		void setIntensity(float value) noexcept override;
		void setColor(const math::float3& value) noexcept override;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveAfter() noexcept override;

		void onLayerChangeAfter() noexcept override;

	private:
		AmbientLightComponent(const AmbientLightComponent&) = delete;
		AmbientLightComponent& operator=(const AmbientLightComponent&) = delete;

	private:
		std::shared_ptr<AmbientLight> ambientLight_;
	};
}

#endif