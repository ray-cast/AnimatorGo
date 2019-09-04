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

			if (imgui::beginMenu(u8"文件"))
			{
				if (imgui::menuItem(u8"打开", "CTRL+O")) { this->sendMessage("editor:menu:file:open"); }
				if (imgui::menuItem(u8"保存", "CTRL+S")) { this->sendMessage("editor:menu:file:save"); }
				if (imgui::menuItem(u8"另存为...", "CTRL+SHIFT+S")) { this->sendMessage("editor:menu:file:saveAs"); }

				imgui::separator();

				if (imgui::menuItem(u8"输出图像", "CTRL+B")) { this->sendMessage("editor:menu:file:picture"); }
				if (imgui::menuItem(u8"输出视频", "CTRL+B")) { this->sendMessage("editor:menu:file:video"); }

				imgui::separator();

				if (imgui::menuItem(u8"导入模型")) { this->sendMessage("editor:menu:file:import"); }
				if (imgui::menuItem(u8"导出模型")) { this->sendMessage("editor:menu:file:export"); }

				imgui::separator();

				if (imgui::menuItem(u8"退出")) { this->sendMessage("editor:menu:file:exit"); }

				imgui::endMenu();
			}

			if (imgui::beginMenu(u8"环境"))
			{
				if (imgui::menuItem(u8"加载天空球", "CTRL+I")) { this->sendMessage("editor:menu:enviroment:hdri"); };
				imgui::endMenu();
			}

			if (imgui::beginMenu(u8"设置"))
			{
				if (imgui::menuItem(u8"开始渲染")) { this->sendMessage("editor:menu:setting:render", true); }
				if (imgui::menuItem(u8"停止渲染")) { this->sendMessage("editor:menu:setting:render", false); }

				imgui::separator();

				if (imgui::menuItem(u8"离线模式", "CTRL+P", isOfflineMode_))
				{
					this->sendMessage("editor:menu:setting:mode", !isOfflineMode_);
					isOfflineMode_ = !isOfflineMode_;
				}

				imgui::endMenu();
			}

			if (imgui::beginMenu(u8"帮助"))
			{
				if (imgui::menuItem(u8"关于")) { this->sendMessage("editor:menu:help:about"); }
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