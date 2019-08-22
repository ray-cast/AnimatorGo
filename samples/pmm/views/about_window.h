#ifndef MYSTICLIT_UI_ABOUT_WINDOW_H_
#define MYSTICLIT_UI_ABOUT_WINDOW_H_

#include <octoon/game_component.h>

namespace MysticLit
{
	class AboutWindow final : public octoon::GameComponent
	{
		OctoonDeclareSubClass(AboutWindow, octoon::GameComponent)
	public:
		AboutWindow() noexcept;
		~AboutWindow() noexcept;

		octoon::GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onGui() noexcept override;

		void showAboutWindow(const octoon::runtime::any& data) noexcept;
	};
}

#endif