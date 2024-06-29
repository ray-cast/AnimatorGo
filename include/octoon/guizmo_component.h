#ifndef OCTOON_GUIZMO_COMPONENT_H_
#define OCTOON_GUIZMO_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	class OCTOON_EXPORT GuizmoComponent final : public GameComponent
	{
		OctoonDeclareSubClass(GuizmoComponent, GameComponent)
	public:
		GuizmoComponent() noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onGui() noexcept override;

	private:
		GuizmoComponent(const GuizmoComponent&) = delete;
		GuizmoComponent& operator=(const GuizmoComponent&) = delete;
	};
}

#endif