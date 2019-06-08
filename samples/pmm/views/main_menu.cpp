#include "main_menu.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(MainMenu, GameComponent, "MainMenu")

		MainMenu::MainMenu() noexcept
			: isShowedMainMenu_(true)
		{
		}

		MainMenu::~MainMenu() noexcept
		{
		}

		void
		MainMenu::onActivate() noexcept
		{
			this->addComponentDispatch(GameDispatchType::Gui);
		}

		void
		MainMenu::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();
		}

		void
		MainMenu::onGui() noexcept
		{
			if (!isShowedMainMenu_)
				return;

			auto& style = imgui::getStyleDefault();
			imgui::pushStyleVar(imgui::GuiStyleVar::WindowPadding, math::float2(style.WindowPadding.x * 2, style.WindowPadding.y));

			if (imgui::beginMainMenuBar())
			{
				math::float2 size = imgui::getDisplaySize();
				size.y -= imgui::getWindowHeight();

				imgui::rootDock(math::float2(0, imgui::getWindowHeight()), size);
				imgui::pushStyleColor(imgui::GuiCol::Border, math::float4::Zero);

				if (imgui::beginMenu("File"))
				{
					if (imgui::menuItem("Open", "CTRL+O")) { this->sendMessage("editor:menu:file:open"); }
					if (imgui::menuItem("Save", "CTRL+S")) { this->sendMessage("editor:menu:file:save"); }
					if (imgui::menuItem("Save As...", "CTRL+SHIFT+S")) { this->sendMessage("editor:menu:file:saveAs"); }

					imgui::separator();

					if (imgui::menuItem("Import Model")) { this->sendMessage("editor:menu:file:import"); }
					if (imgui::menuItem("Export Model")) { this->sendMessage("editor:menu:file:export"); }

					imgui::separator();

					if (imgui::menuItem("Exit")) { this->sendMessage("editor:menu:file:exit"); }

					imgui::endMenu();
				}

				if (imgui::beginMenu("Edit"))
				{
					imgui::menuItem("Undo", "CTRL+Z", false, false);
					imgui::menuItem("Rndo", "CTRL+Y", false, false);

					imgui::separator();

					if (imgui::beginMenu("Language"))
					{
						if (imgui::menuItem("English")) { /*on click() */ }
						if (imgui::menuItem("Chinese")) { /*on click() */ }

						imgui::endMenu();
					}

					imgui::separator();
					imgui::menuItem("Style Editor", 0);

					imgui::endMenu();
				}

				if (imgui::beginMenu("GameObject"))
				{
					if (imgui::menuItem("Create Game Object")) { this->sendMessage("editor:menu:create:object"); }
					if (imgui::menuItem("Create Cube Object")) { this->sendMessage("editor:menu:create:cube"); }
					if (imgui::menuItem("Create Sphere Object")) { this->sendMessage("editor:menu:create:sphere"); }
					if (imgui::menuItem("Create Plane Object")) { this->sendMessage("editor:menu:create:plane"); }

					imgui::endMenu();
				}

				if (imgui::beginMenu("Help"))
				{
					if (imgui::menuItem("About")) { this->sendMessage("editor:menu:help:about"); }
					imgui::endMenu();
				}

				imgui::popStyleColor();

				imgui::endMainMenuBar();
			}

			imgui::popStyleVar();
		}

		GameComponentPtr
		MainMenu::clone() const noexcept
		{
			return std::make_shared<MainMenu>();
		}
	}
}