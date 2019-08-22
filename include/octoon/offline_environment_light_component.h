#ifndef OCTOON_OFFLINE_ENVIRONMENT_LIGHT_COMPONENT_H_
#define OCTOON_OFFLINE_ENVIRONMENT_LIGHT_COMPONENT_H_

#include <octoon/offline_light_component.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineEnvironmentLightComponent final : public OfflineLightComponent
	{
		OctoonDeclareSubInterface(OfflineEnvironmentLightComponent, OfflineLightComponent)
	public:
		OfflineEnvironmentLightComponent() noexcept;
		virtual ~OfflineEnvironmentLightComponent() noexcept;

		void setIntensity(float value) noexcept override;

		void setBgImage(const std::string& path) noexcept;
		const std::string& getBgImage() const noexcept;

		void setUseBgImage(bool enable) noexcept;
		bool getUseBgImage() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onLayerChangeAfter() noexcept override;

	private:
		void* createImage(const std::string& path) noexcept;

	private:
		OfflineEnvironmentLightComponent(const OfflineEnvironmentLightComponent&) = delete;
		OfflineEnvironmentLightComponent& operator=(const OfflineEnvironmentLightComponent&) = delete;

	private:
		bool useBgImage_;

		void* rprImage_;
		void* rprLight_;

		math::float3 color_;
		std::string path_;
	};
}

#endif