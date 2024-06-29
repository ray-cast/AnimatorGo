#ifndef UNREAL_GRID_COMPONENT_H_
#define UNREAL_GRID_COMPONENT_H_

#include <unreal_component.h>
#include <octoon/game_object.h>
#include <module/grid_module.h>

namespace unreal
{
	class GridComponent final : public UnrealComponent<GridModule>
	{
	public:
		GridComponent() noexcept;
		~GridComponent() noexcept;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(GridComponent);
		}

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

	private:
		octoon::GameObjectPtr gizmo_;
	};
}

#endif