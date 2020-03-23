#ifndef RABBIT_GRID_COMPONENT_H_
#define RABBIT_GRID_COMPONENT_H_

#include <rabbit_component.h>

#include <octoon/raycaster.h>
#include <octoon/game_object.h>
#include <octoon/material/material.h>

#include <module/grid_module.h>

#include <optional>

namespace rabbit
{
	class GridComponent final : public RabbitComponent<GridModule>
	{
	public:
		GridComponent() noexcept;
		~GridComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

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