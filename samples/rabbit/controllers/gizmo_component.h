#ifndef RABBIT_GIZMO_COMPONENT_H_
#define RABBIT_GIZMO_COMPONENT_H_

#include <rabbit_component.h>

#include <octoon/raycaster.h>
#include <octoon/game_object.h>
#include <octoon/material/material.h>

#include <module/drag_module.h>

namespace rabbit
{
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
		GizmoComponent(const GizmoComponent&) = delete;
		GizmoComponent& operator=(const GizmoComponent&) = delete;
	};
}

#endif