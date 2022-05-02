#ifndef FLOWER_GIZMO_COMPONENT_H_
#define FLOWER_GIZMO_COMPONENT_H_

#include <flower_component.h>

#include <octoon/raycaster.h>
#include <octoon/game_object.h>
#include <octoon/material/material.h>

#include <module/drag_module.h>

#include <optional>

namespace flower
{
	class TransformGizmo
	{
	public:
		TransformGizmo();
		virtual ~TransformGizmo() = default;

		void highlight(std::string_view axis) noexcept;

		virtual void setActivePlane(const std::string& axis, const octoon::math::float3& eye) = 0;

		octoon::GameObjectPtr activePlane;
		octoon::GameObjectPtr planes;
		octoon::GameObjectPtr handleGizmos;
		octoon::GameObjectPtr pickerGizmos;

	private:
		TransformGizmo(const TransformGizmo&) = delete;
		TransformGizmo& operator=(const TransformGizmo&) = delete;
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

		void handleMouseDown(const octoon::input::InputEvent& event) noexcept;
		void handleMouseUp(const octoon::input::InputEvent& event) noexcept;
		void handleMouseMove(const octoon::input::InputEvent& event) noexcept;
		void handleMouseHover(const octoon::input::InputEvent& event) noexcept;

	private:
		void onEnable() noexcept;
		void onDisable() noexcept;

		void onMouseDown(const octoon::input::InputEvent& event) noexcept override;
		void onMouseUp(const octoon::input::InputEvent& event) noexcept override;
		void onMouseMotion(const octoon::input::InputEvent& event) noexcept override;

		void onLateUpdate() noexcept override;

	private:
		std::optional<octoon::RaycastHit> intersectObjects(float x, float y, octoon::GameObjects& pickerGizmos) noexcept;
		std::optional<octoon::RaycastHit> intersectObjects(float x, float y, octoon::GameObjectPtr& pickerGizmos) noexcept;

	private:
		GizmoComponent(const GizmoComponent&) = delete;
		GizmoComponent& operator=(const GizmoComponent&) = delete;

	private:
		std::string axis_;
		std::string gizmoMode_;
		octoon::math::float3 offset_;

		octoon::math::float3 oldScale_;
		octoon::math::float3 oldPosition_;
		octoon::math::Quaternion oldRotation_;

		std::map<std::string, std::unique_ptr<TransformGizmo>> gizmo_;
	};
}

#endif