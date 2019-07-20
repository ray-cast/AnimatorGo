#ifndef OCTOON_OFFLINE_ENVIRONMENT_LIGHT_COMPONENT_H_
#define OCTOON_OFFLINE_ENVIRONMENT_LIGHT_COMPONENT_H_

#include <octoon/offline_light_component.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineEnvironmentLightComponent : public OfflineLightComponent
	{
		OctoonDeclareSubInterface(OfflineLightComponent, OfflineLightComponent)
	public:
		OfflineEnvironmentLightComponent() noexcept;
		virtual ~OfflineEnvironmentLightComponent() noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onLayerChangeAfter() noexcept override;

	private:
		OfflineEnvironmentLightComponent(const OfflineEnvironmentLightComponent&) = delete;
		OfflineEnvironmentLightComponent& operator=(const OfflineEnvironmentLightComponent&) = delete;

	private:
		void* rprImage_;
		void* rprLight_;
		math::float3 color_;
	};
}

#endif