#include "asset_window.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(AssetWindow, GameComponent, "AssetWindow")

		AssetWindow::AssetWindow() noexcept
			: isShowedAssetsWindow_(true)
		{
		}

		AssetWindow::~AssetWindow() noexcept
		{
		}

		void
		AssetWindow::onActivate() noexcept
		{
			this->addComponentDispatch(GameDispatchType::Gui);
		}

		void
		AssetWindow::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();
		}

		void 
		AssetWindow::onGui() noexcept
		{
			if (imgui::beginDock("Assets", &isShowedAssetsWindow_))
			{
				imgui::text("", "");
				imgui::sameLine();
				if (imgui::button("Import")) {}
				imgui::sameLine();
				if (imgui::button("Export")) {}

				imgui::pushStyleColor(imgui::GuiCol::Button, math::float4::Zero);
				imgui::text("", "");
				imgui::sameLine();

				imgui::popStyleColor();

				imgui::endDock();
			}
		}

		GameComponentPtr 
		AssetWindow::clone() const noexcept
		{
			return std::make_shared<AssetWindow>();
		}
	}
}