#ifndef UNREAL_MATERIAL_COMPONENT_H_
#define UNREAL_MATERIAL_COMPONENT_H_

#include "../unreal_component.h"
#include "../module/resource_module.h"

namespace unreal
{
	class MaterialComponent final : public UnrealComponent<ResourceModule>
	{
	public:
		MaterialComponent() noexcept;
		virtual ~MaterialComponent() noexcept;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(MaterialComponent);
		}

	private:
		void onEnable() noexcept(false);
		void onDisable() noexcept;

	private:
		MaterialComponent(const MaterialComponent&) = delete;
		MaterialComponent& operator=(const MaterialComponent&) = delete;		
	};
}

#endif