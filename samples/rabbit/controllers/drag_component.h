#ifndef RABBIT_DRAG_COMPONENT_H_
#define RABBIT_DRAG_COMPONENT_H_

#include <rabbit_component.h>

#include <octoon/raycaster.h>
#include <octoon/game_object.h>
#include <octoon/material/material.h>

#include <module/drag_module.h>

#include <optional>

namespace rabbit
{
	class DragComponent final : public RabbitComponent<DragModule>
	{
	public:
		DragComponent() noexcept;
		~DragComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		const std::optional<octoon::RaycastHit>& getHoverItem() const noexcept;
		const std::optional<octoon::RaycastHit>& getSelectedItem() const noexcept;

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

		octoon::material::MaterialPtr gizmoHoverMtl_;
		octoon::material::MaterialPtr gizmoSelectedMtl_;

		std::optional<octoon::RaycastHit> selectedItem_;
		std::optional<octoon::RaycastHit> selectedItemHover_;
	};
}

#endif