#include "hierarchy_window.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(HierarchyWindow, GameComponent, "HierarchyWindow")

		HierarchyWindow::HierarchyWindow() noexcept
			: isShowedHierarchyWindow_(true)
		{
		}

		HierarchyWindow::~HierarchyWindow() noexcept
		{
		}

		void
		HierarchyWindow::onActivate() noexcept
		{
			this->addComponentDispatch(GameDispatchType::Gui);
		}

		void
		HierarchyWindow::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();
		}

		void 
		HierarchyWindow::onGui() noexcept
		{
			if (imgui::begin_dock("Hierarchy", &isShowedHierarchyWindow_))
			{
				if (imgui::tree_node_ex("Camera", imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
				{

					imgui::tree_pop();
				}

				if (imgui::tree_node_ex("Object", imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
				{
					imgui::tree_pop();
				}

				imgui::end_dock();
			}
		}

		GameComponentPtr 
		HierarchyWindow::clone() const noexcept
		{
			return std::make_shared<HierarchyWindow>();
		}
	}
}