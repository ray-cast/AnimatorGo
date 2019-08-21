#ifndef MYSTICLIT_UI_THEME_MANAGER_H_
#define MYSTICLIT_UI_THEME_MANAGER_H_

#include <octoon/game_component.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	namespace editor
	{
		class ThemeManager final : public GameComponent
		{
			OctoonDeclareSubClass(ThemeManager, GameComponent)
		public:
			ThemeManager() noexcept;
			~ThemeManager() noexcept;

			GameComponentPtr clone() const noexcept override;

		private:
			void onActivate() noexcept override;
			void onDeactivate() noexcept override;

			void onGui() noexcept override;

		private:
			imgui::GuiStyle _style;
			imgui::GuiStyle _styleDefault;
		};
	}
}

#endif
