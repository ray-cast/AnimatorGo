#include "hierarchy_window.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>
#include <octoon/game_object.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(HierarchyWindow, GameComponent, "HierarchyWindow")

		HierarchyWindow::HierarchyWindow() noexcept
			: isShowedHierarchyWindow_(true)
			, selectedObject_(nullptr)
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
			constexpr int PATHLIMITE = 4096;

			if (imgui::beginDock("Hierarchy", &isShowedHierarchyWindow_))
			{			
				if (imgui::treeNodeEx("Object", imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
				{
					for (auto& it : GameObject::instances())
					{
						if (it->getParent())
							continue;

						char objectName[PATHLIMITE];

						auto& name = it->getName();
						if (name.empty())
							std::sprintf(objectName, "|-null");
						else
							std::sprintf(objectName, "|-%s", name.c_str());

						if (imgui::selectable(objectName, selectedObject_ == it ? true : false))
							this->sendMessage("editor:object:selected", it);
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
				selectedObject_ = object;
		}
	}
}