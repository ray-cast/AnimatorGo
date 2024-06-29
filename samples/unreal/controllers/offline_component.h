#ifndef UNREAL_OFFLINE_COMPONENT_H_
#define UNREAL_OFFLINE_COMPONENT_H_

#include "../unreal_component.h"
#include "../module/offline_module.h"

namespace unreal
{
	class OfflineComponent final : public UnrealComponent<OfflineModule>
	{
	public:
		OfflineComponent() noexcept;
		virtual ~OfflineComponent() noexcept;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(OfflineComponent);
		}

	private:
		void onInit() noexcept override;

		void onEnable() noexcept override;
		void onDisable() noexcept override;

		void onLateUpdate() noexcept override;

	private:
		OfflineComponent(const OfflineComponent&) = delete;
		OfflineComponent& operator=(const OfflineComponent&) = delete;
	};
}

#endif