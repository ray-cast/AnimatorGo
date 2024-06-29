#ifndef UNREAL_MARK_COMPONENT_H_
#define UNREAL_MARK_COMPONENT_H_

#include "../unreal_component.h"
#include "../module/mark_module.h"

namespace unreal
{
	class MarkComponent final : public UnrealComponent<MarkModule>
	{
	public:
		MarkComponent() noexcept;
		virtual ~MarkComponent() noexcept;

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