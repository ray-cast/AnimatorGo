#ifndef RABBIT_MARK_COMPONENT_H_
#define RABBIT_MARK_COMPONENT_H_

#include "../rabbit_component.h"
#include "../module/mark_module.h"

namespace rabbit
{
	class MarkComponent final : public RabbitComponent<MarkModule>
	{
	public:
		MarkComponent() noexcept;
		virtual ~MarkComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		virtual void onPostProcess() noexcept override;

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