#ifndef OCTOON_UI_VIEW_COMPONENT_H_
#define OCTOON_UI_VIEW_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	namespace editor
	{
		class MainMenu final : public GameComponent
		{
			OctoonDeclareSubClass(MainMenu, GameComponent)
		public:
			MainMenu() noexcept;
			~MainMenu() noexcept;

			GameComponentPtr clone() const noexcept override;

		private:
			void onActivate() noexcept override;
			void onDeactivate() noexcept override;

			void onGui() noexcept override;

		private:
			bool isShowedMainMenu_;
		};
	}
}

#endif
