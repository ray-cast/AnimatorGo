#ifndef OCTOON_UI_ABOUT_WINDOW_H_
#define OCTOON_UI_ABOUT_WINDOW_H_

#include <octoon/game_component.h>

namespace octoon
{
	namespace editor
	{
		class AboutWindow final : public GameComponent
		{
			OctoonDeclareSubClass(AboutWindow, GameComponent)
		public:
			AboutWindow() noexcept;
			~AboutWindow() noexcept;

			GameComponentPtr clone() const noexcept override;

		private:
			void onActivate() noexcept override;
			void onDeactivate() noexcept override;

			void onGui() noexcept override;

			void showAboutWindow(const runtime::any& data) noexcept;
		};
	}
}

#endif