#ifndef UNREAL_SELECTOR_COMPONENT_H_
#define UNREAL_SELECTOR_COMPONENT_H_

#include <unreal_component.h>

#include <octoon/raycaster.h>
#include <octoon/game_object.h>
#include <octoon/material/material.h>

#include <module/selector_module.h>

namespace unreal
{
	class SelectorComponent final : public UnrealComponent<SelectorModule>
	{
	public:
		SelectorComponent() noexcept;
		~SelectorComponent() noexcept;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(SelectorComponent);
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
		octoon::MaterialPtr gizmoHoverMtl_;
		octoon::MaterialPtr gizmoSelectedMtl_;

		octoon::GameObjectPtr gizmoHover_;
		octoon::GameObjectPtr gizmoSelected_;
	};
}

#endif