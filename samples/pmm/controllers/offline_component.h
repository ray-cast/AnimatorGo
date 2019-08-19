#ifndef MYSTIC_OFFLINE_COMPONENT_H_
#define MYSTIC_OFFLINE_COMPONENT_H_

#include "../mysticLit_component.h"
#include "../utils/pmm.h"
#include "../module/offline_module.h"

namespace MysticLit
{
	class OfflineComponent final : public MysticLitComponent<OfflineModule>
	{
	public:
		OfflineComponent() noexcept;
		virtual ~OfflineComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;
	};
}

#endif