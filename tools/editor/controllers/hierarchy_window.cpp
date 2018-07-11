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

				if (imgui::tree_node_ex("Camera", imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
				{
					for (auto& it : actors)
					{
						auto camera = it->getComponent<CameraComponent>();
						if (!camera)
							continue;

						if (imgui::selectable(it->getName().c_str(), _selectedObject == it ? true : false))
							_selectedObject = it;
					}

					imgui::tree_pop();
				}
				
				if (imgui::tree_node_ex("Object", imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
				{
					for (auto& it : actors)
					{
						auto meshRender = it->getComponent<MeshRendererComponent>();
						if (!meshRender)
							continue;

						auto& name = it->getName();
						char objectName[MAX_PATH];
						if (name.empty())
							std::sprintf(objectName, "|-mesh");
						else
							std::sprintf(objectName, "|-%s", name.c_str());

						if (imgui::selectable(objectName, _selectedObject == it ? true : false))
							_selectedObject = it;
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