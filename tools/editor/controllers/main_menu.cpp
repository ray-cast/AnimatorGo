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

			auto& style = imgui::get_style_default();
			imgui::push_style_var(imgui::GuiStyleVar::WindowPadding, math::float2(style.WindowPadding.x * 2, style.WindowPadding.y));

			if (imgui::begin_main_menu_bar())
			{
				math::float2 size = imgui::get_display_size();
				size.y -= imgui::get_window_height();

				imgui::root_dock(math::float2(0, imgui::get_window_height()), size);
				imgui::push_style_color(imgui::GuiCol::Border, math::float4::Zero);

				if (imgui::begin_menu("File"))
				{
					if (imgui::menu_item("Open", "CTRL+O", false, false)) { this->sendMessage("editor:menu:file:open"); }
					if (imgui::menu_item("Save", "CTRL+S", false, false)) { this->sendMessage("editor:menu:file:save"); }
					if (imgui::menu_item("Save As...", "CTRL+SHIFT+S", false, false)) { this->sendMessage("editor:menu:file:saveAs"); }

					imgui::separator();

					if (imgui::menu_item("Import Model")) { this->sendMessage("editor:menu:file:import"); }
					if (imgui::menu_item("Export Model")) { this->sendMessage("editor:menu:file:export"); }

					imgui::separator();

					if (imgui::menu_item("Exit")) { this->sendMessage("editor:menu:file:exit"); }

					imgui::end_menu();
				}

				if (imgui::begin_menu("Edit"))
				{
					if (imgui::begin_menu("Language"))
					{
						if (imgui::menu_item("English")) { /*on click() */ }
						if (imgui::menu_item("Chinese")) { /*on click() */ }

						imgui::end_menu();
					}

					imgui::separator();
					imgui::menu_item("Style Editor", 0);

					imgui::end_menu();
				}

				if (imgui::begin_menu("GameObject"))
				{
					if (imgui::menu_item("Create Game Object")) { this->sendMessage("editor:menu:help:about"); }
					imgui::end_menu();
				}

				if (imgui::begin_menu("Window"))
				{
					imgui::menu_item("Assert", 0);
					imgui::menu_item("Camera", 0);
					imgui::menu_item("Inspector", 0);
					imgui::menu_item("Light Mass", 0);
					imgui::end_menu();
				}

				if (imgui::begin_menu("Help"))
				{
					if (imgui::menu_item("About")) { this->sendMessage("editor:menu:help:about"); }
					imgui::end_menu();
				}

				imgui::pop_style_color();

				imgui::end_main_menu_bar();
			}

			imgui::pop_style_var();
		}

		GameComponentPtr
		MainMenu::clone() const noexcept
		{
			return std::make_shared<MainMenu>();
		}
	}
}