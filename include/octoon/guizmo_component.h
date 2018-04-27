#ifndef OCTOON_GUIZMO_COMPONENT_H_
#define OCTOON_GUIZMO_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT GuizmoComponent : public GameComponent
	{
		OctoonDeclareSubClass(GuizmoComponent, GameComponent)
	public:
		GuizmoComponent() noexcept;
		GuizmoComponent(GameObjectPtr& camera) noexcept;

		octoon::GameComponentPtr clone() const noexcept;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onGui() noexcept override;

	private:
		GameObjectPtr camera_;
	};
}

#endif