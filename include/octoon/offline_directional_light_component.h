#ifndef OCTOON_OFFLINE_DIRECTIONAL_LIGHT_COMPONENT_H_
#define OCTOON_OFFLINE_DIRECTIONAL_LIGHT_COMPONENT_H_

#include <octoon/offline_light_component.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineDirectionalLightComponent final : public OfflineLightComponent
	{
		OctoonDeclareSubInterface(OfflineLightComponent, OfflineLightComponent)
	public:
		OfflineDirectionalLightComponent() noexcept;
		virtual ~OfflineDirectionalLightComponent() noexcept;

		void setIntensity(float value) noexcept override;
		void setColor(const math::float3& value) noexcept override;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveAfter() noexcept override;

		void onLayerChangeAfter() noexcept override;

	private:
		OfflineDirectionalLightComponent(const OfflineDirectionalLightComponent&) = delete;
		OfflineDirectionalLightComponent& operator=(const OfflineDirectionalLightComponent&) = delete;

	private:
		void* rprLight_;
	};
}

#endif