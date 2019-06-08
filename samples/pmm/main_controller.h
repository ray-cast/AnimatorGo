#ifndef OCTOON_MAIN_CONTROLLER_H_
#define OCTOON_MAIN_CONTROLLER_H_

#include <octoon/game_component.h>

namespace octoon
{
	namespace editor
	{
		class MainController final : public GameComponent
		{
			OctoonDeclareSubClass(MainController, GameComponent)
		public:
			MainController() noexcept;
			~MainController() noexcept;

			GameComponentPtr clone() const noexcept override;

		private:
			void onActivate() noexcept override;
			void onDeactivate() noexcept override;

		private:
			GameObjectPtr main_;
			GameObjectPtr object_;
		};
	}
}

#endif