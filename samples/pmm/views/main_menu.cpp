#include "main_menu.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>

using namespace octoon;

namespace MysticLit
{
	OctoonImplementSubClass(MainMenu, octoon::GameComponent, "MainMenu")

	MainMenu::MainMenu() noexcept
		: isShowedMainMenu_(true)
		, isOfflineMode_(false)
	{
	}

	MainMenu::~MainMenu() noexcept
	{
	}

	void
	MainMenu::onActivate() noexcept
	{
		this->addComponentDispatch(octoon::GameDispatchType::Gui);
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

				if (imgui::menuItem("render to picture file", "CTRL+B")) { this->sendMessage("editor:menu:file:picture"); }
				if (imgui::menuItem("render to video file", "CTRL+B")) { this->sendMessage("editor:menu:file:video"); }

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

			if (imgui::beginMenu("Environment"))
			{
				if (imgui::menuItem("Load hdri", "CTRL+I")) { this->sendMessage("editor:menu:enviroment:hdri"); };
				imgui::endMenu();
			}

			if (imgui::beginMenu("Setting"))
			{
				if (imgui::menuItem("Render")) { this->sendMessage("editor:menu:setting:render", true); }
				if (imgui::menuItem("Stop Render")) { this->sendMessage("editor:menu:setting:render", false); }

				imgui::separator();

				if (imgui::menuItem("Offline Mode", "CTRL+P", isOfflineMode_))
				{
					this->sendMessage("editor:menu:setting:mode", !isOfflineMode_);
					isOfflineMode_ = !isOfflineMode_;
				}

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

	octoon::GameComponentPtr
	MainMenu::clone() const noexcept
	{
		return std::make_shared<MainMenu>();
	}
}