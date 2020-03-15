#ifndef OCTOON_EDITOR_EDIT_CONTROLLER_H_
#define OCTOON_EDITOR_EDIT_CONTROLLER_H_

#include <octoon/game_component.h>

namespace octoon
{
	namespace editor
	{
		class EditController : public GameComponent
		{
			OctoonDeclareSubClass(EditController, GameComponent)
		public:
			EditController() noexcept;
			~EditController() noexcept;

			virtual void createGameObject(const std::any& data) noexcept;
			virtual void createCubeObject(const std::any& data) noexcept;
			virtual void createSphereObject(const std::any& data) noexcept;
			virtual void createPlaneObject(const std::any& data) noexcept;

			virtual GameComponentPtr clone() const noexcept override;

		private:
			void onActivate() noexcept override;
			void onDeactivate() noexcept override;

		private:
			GameObjects objects_;
		};
	}
}

#endif