#ifndef MYSTICLIT_MAIN_CONTROLLER_H_
#define MYSTICLIT_MAIN_CONTROLLER_H_

#include <octoon/game_component.h>

namespace MysticLit
{
	class MainController final : public octoon::GameComponent
	{
		OctoonDeclareSubClass(MainController, octoon::GameComponent)
	public:
		MainController() noexcept;
		~MainController() noexcept;

		octoon::GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

	private:
		octoon::GameObjectPtr main_;
	};
}

#endif