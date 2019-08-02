#ifndef OCTOON_OFFLINE_POINT_LIGHT_COMPONENT_H_
#define OCTOON_OFFLINE_POINT_LIGHT_COMPONENT_H_

#include <octoon/offline_light_component.h>

namespace octoon
{
	class OCTOON_EXPORT OfflinePointLightComponent final : public OfflineLightComponent
	{
		OctoonDeclareSubInterface(OfflineLightComponent, OfflineLightComponent)
	public:
		OfflinePointLightComponent() noexcept;
		virtual ~OfflinePointLightComponent() noexcept;

		void setIntensity(float value) noexcept override;
		void setColor(const math::float3& value) noexcept override;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveAfter() noexcept override;

		void onLayerChangeAfter() noexcept override;

	private:
		OfflinePointLightComponent(const OfflinePointLightComponent&) = delete;
		OfflinePointLightComponent& operator=(const OfflinePointLightComponent&) = delete;

	private:
		void* rprLight_;

		math::float3 color_;
	};
}

#endif