#ifndef FLOWER_DRAG_COMPONENT_H_
#define FLOWER_DRAG_COMPONENT_H_

#include <flower_component.h>

#include <octoon/raycaster.h>
#include <octoon/game_object.h>
#include <octoon/material/material.h>

#include <module/drag_module.h>

#include <optional>

namespace flower
{
	class DragComponent final : public RabbitComponent<DragModule>
	{
	public:
		DragComponent() noexcept;
		~DragComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(DragComponent);
		}

	private:
		void handleMouseDown(const octoon::input::InputEvent& event) noexcept;
		void handleMouseMove(const octoon::input::InputEvent& event) noexcept;
		void handleMouseHover(const octoon::input::InputEvent& event) noexcept;

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

		void onUpdate() noexcept override;

		void onMouseDown(const octoon::input::InputEvent& event) noexcept;
		void onMouseMotion(const octoon::input::InputEvent& event) noexcept;
		void onMouseUp(const octoon::input::InputEvent& event) noexcept;

	private:
		std::optional<octoon::RaycastHit> intersectObjects(float x, float y) noexcept;

	private:
		octoon::GameObjectPtr gizmoHover_;
		octoon::GameObjectPtr gizmoSelected_;

		octoon::MaterialPtr gizmoHoverMtl_;
		octoon::MaterialPtr gizmoSelectedMtl_;
	};
}

#endif