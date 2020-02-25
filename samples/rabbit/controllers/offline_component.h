#ifndef RABBIT_OFFLINE_COMPONENT_H_
#define RABBIT_OFFLINE_COMPONENT_H_

#include "../rabbit_component.h"
#include "../module/offline_module.h"

namespace rabbit
{
	class OfflineComponent final : public RabbitComponent<OfflineModule>
	{
	public:
		OfflineComponent() noexcept;
		virtual ~OfflineComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(OfflineComponent);
		}

	private:
		OfflineComponent(const OfflineComponent&) = delete;
		OfflineComponent& operator=(const OfflineComponent&) = delete;
	};
}

#endif