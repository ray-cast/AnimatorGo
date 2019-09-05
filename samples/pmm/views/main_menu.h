#ifndef MYSTICLIT_UI_MAIN_MENU_H_
#define MYSTICLIT_UI_MAIN_MENU_H_

#include <octoon/game_component.h>

namespace MysticLit
{
	class MainMenu final : public octoon::GameComponent
	{
		OctoonDeclareSubClass(MainMenu, octoon::GameComponent)
	public:
		MainMenu() noexcept;
		~MainMenu() noexcept;

		octoon::GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onGui() noexcept override;

	private:
		bool isOfflineMode_;
		bool isShowedMainMenu_;
	};
}

#endif