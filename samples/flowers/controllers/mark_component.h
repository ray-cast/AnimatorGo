#ifndef FLOWER_MARK_COMPONENT_H_
#define FLOWER_MARK_COMPONENT_H_

#include "../flower_component.h"
#include "../module/mark_module.h"

namespace flower
{
	class MarkComponent final : public RabbitComponent<MarkModule>
	{
	public:
		MarkComponent() noexcept;
		virtual ~MarkComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(MarkComponent);
		}

	private:
		MarkComponent(const MarkComponent&) = delete;
		MarkComponent& operator=(const MarkComponent&) = delete;
	};
}

#endif