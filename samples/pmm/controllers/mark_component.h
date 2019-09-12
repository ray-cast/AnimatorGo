#ifndef MYSTIC_MARK_COMPONENT_H_
#define MYSTIC_MARK_COMPONENT_H_

#include "../mysticLit_component.h"
#include "../module/mark_module.h"

namespace MysticLit
{
	class MarkComponent final : public MysticLitComponent<MarkModule>
	{
	public:
		MarkComponent() noexcept;
		virtual ~MarkComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		virtual void onPostProcess() noexcept override;

	private:
		MarkComponent(const MarkComponent&) = delete;
		MarkComponent& operator=(const MarkComponent&) = delete;
	};
}

#endif