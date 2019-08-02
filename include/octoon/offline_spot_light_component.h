#ifndef OCTOON_OFFLINE_SPOT_LIGHT_COMPONENT_H_
#define OCTOON_OFFLINE_SPOT_LIGHT_COMPONENT_H_

#include <octoon/offline_light_component.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineSpotLightComponent final : public OfflineLightComponent
	{
		OctoonDeclareSubInterface(OfflineLightComponent, OfflineLightComponent)
	public:
		OfflineSpotLightComponent() noexcept;
		virtual ~OfflineSpotLightComponent() noexcept;

		void setIntensity(float value) noexcept override;
		void setColor(const math::float3& value) noexcept override;

		void setInnerAngle(float value) noexcept;
		void setOuterAngle(float value) noexcept;

		float getInnerAngle() const noexcept;
		float getOuterAngle() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveAfter() noexcept override;

		void onLayerChangeAfter() noexcept override;

	private:
		OfflineSpotLightComponent(const OfflineSpotLightComponent&) = delete;
		OfflineSpotLightComponent& operator=(const OfflineSpotLightComponent&) = delete;

	private:
		void* rprLight_;

		float innerAngle_;
		float outerAngle_;

		math::float3 color_;
	};
}

#endif
