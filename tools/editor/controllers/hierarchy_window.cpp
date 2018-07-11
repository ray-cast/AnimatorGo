#include "hierarchy_window.h"

#include <octoon/octoon.h>
#include <octoon/game_object_manager.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(HierarchyWindow, GameComponent, "HierarchyWindow")

		HierarchyWindow::HierarchyWindow() noexcept
			: isShowedHierarchyWindow_(true)
			, _selectedObject(nullptr)
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
				auto& actors = GameObjectManager::instance()->getInstanceList();
				
				if (imgui::tree_node_ex("Object", imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
				{
					for (auto& it : actors)
					{
						auto& name = it->getName();
						char objectName[MAX_PATH];
						if (name.empty())
							std::sprintf(objectName, "|-null");
						else
							std::sprintf(objectName, "|-%s", name.c_str());

						if (imgui::selectable(objectName, _selectedObject == it ? true : false))
						{
							_selectedObject = it;
							this->sendMessage("editor:object:selected", it);
						}
					}

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