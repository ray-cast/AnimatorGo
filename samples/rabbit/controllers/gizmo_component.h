#ifndef RABBIT_GIZMO_COMPONENT_H_
#define RABBIT_GIZMO_COMPONENT_H_

#include <rabbit_component.h>

#include <octoon/raycaster.h>
#include <octoon/game_object.h>
#include <octoon/material/material.h>

#include <module/drag_module.h>

namespace rabbit
{
	class TransformGizmo
	{
	public:
		void init();

		octoon::GameObjectPtr activePlane;
		std::map<std::string, octoon::GameObjectPtr> planes;
		std::map<std::string, octoon::GameObjects> handleGizmos;
		std::map<std::string, octoon::GameObjects> pickerGizmos;
	};

	class GizmoComponent final : public RabbitComponent<DragModule>
	{
	public:
		GizmoComponent() noexcept;
		virtual ~GizmoComponent() noexcept;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(GizmoComponent);
		}

	private:
		void onEnable() noexcept;
		void onDisable() noexcept;

	private:
		GizmoComponent(const GizmoComponent&) = delete;
		GizmoComponent& operator=(const GizmoComponent&) = delete;

	private:
		std::unique_ptr<TransformGizmo> translateGizmo_;
		octoon::GameObjectPtr transformGizmo_;
	};
}

#endif