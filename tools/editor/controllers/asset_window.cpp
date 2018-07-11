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
			if (imgui::begin_dock("Assets", &isShowedAssetsWindow_))
			{
				imgui::end_dock();
			}
		}

		GameComponentPtr 
		AssetWindow::clone() const noexcept
		{
			return std::make_shared<AssetWindow>();
		}
	}
}