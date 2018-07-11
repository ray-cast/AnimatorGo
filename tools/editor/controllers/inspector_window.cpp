#include "inspector_window.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(InspectorWindow, GameComponent, "InspectorWindow")

		InspectorWindow::InspectorWindow() noexcept
			: isShowedInspectorWindow_(true)
		{
		}

		InspectorWindow::~InspectorWindow() noexcept
		{
		}

		void
		InspectorWindow::onActivate() noexcept
		{
			this->addComponentDispatch(GameDispatchType::Gui);
			this->addMessageListener("editor:object:selected", std::bind(&InspectorWindow::onObjectSelected, this, std::placeholders::_1));
		}

		void
		InspectorWindow::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();
		}

		void 
		InspectorWindow::onGui() noexcept
		{
			if (!isShowedInspectorWindow_)
				return;

			if (imgui::begin_dock("Inspector", &isShowedInspectorWindow_))
			{
				imgui::end_dock();
			}
		}

		GameComponentPtr 
		InspectorWindow::clone() const noexcept
		{
			return std::make_shared<InspectorWindow>();
		}

		void
		InspectorWindow::onObjectSelected(const runtime::any& data) noexcept
		{
		}
	}
}