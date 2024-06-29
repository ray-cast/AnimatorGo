#ifndef UNREAL_ENTITIES_COMPONENT_H_
#define UNREAL_ENTITIES_COMPONENT_H_

#include <filesystem>

#include "../unreal_component.h"
#include "../module/entities_module.h"

namespace unreal
{
	class EntitiesComponent final : public UnrealComponent<EntitiesModule>
	{
	public:
		EntitiesComponent() noexcept;
		virtual ~EntitiesComponent() noexcept;

		octoon::GameObjectPtr importModel(const std::filesystem::path& path) noexcept(false);

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(EntitiesComponent);
		}
	};
}

#endif