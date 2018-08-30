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
			this->addMessageListener("editor:object:selected", std::bind(&HierarchyWindow::onObjectSelected, this, std::placeholders::_1));
		}

		void
		HierarchyWindow::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();
		}

		void 
		HierarchyWindow::onGui() noexcept
		{
			if (imgui::beginDock("Hierarchy", &isShowedHierarchyWindow_))
			{
				auto& actors = GameObjectManager::instance()->getInstanceList();
				
				if (imgui::treeNodeEx("Object", imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
				{
					for (auto& it : actors)
					{
						auto& name = it->getName();
						char objectName[256];
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

					imgui::treePop();
				}

				imgui::endDock();
			}
		}

		GameComponentPtr 
		HierarchyWindow::clone() const noexcept
		{
			return std::make_shared<HierarchyWindow>();
		}

		void
		HierarchyWindow::onObjectSelected(const runtime::any& data) noexcept
		{
			auto object = runtime::any_cast<GameObject*>(data);
			if (object)
				_selectedObject = object;
		}
	}
}