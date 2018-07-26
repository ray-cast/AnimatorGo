#include "about_window.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(AboutWindow, GameComponent, "UIViewComponent")

		AboutWindow::AboutWindow() noexcept
		{
		}

		AboutWindow::~AboutWindow() noexcept
		{
		}

		void
		AboutWindow::onActivate() noexcept
		{
			this->addComponentDispatch(GameDispatchType::Gui);
			this->addMessageListener("editor:menu:help:about", std::bind(&AboutWindow::showAboutWindow, this, std::placeholders::_1));
		}

		void
		AboutWindow::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();
			this->removeMessageListener("editor:menu:help:about", std::bind(&AboutWindow::showAboutWindow, this, std::placeholders::_1));
		}

		void
		AboutWindow::showAboutWindow(const runtime::any& data) noexcept
		{
			auto str = "Octoon Studio Ver.0.1 beta\n\n Licensed under the MIT.\n\n Copyright (c) 2018-2019. All rights reserved.";
			this->sendMessage("editor:message", std::make_pair("About", str));
		}

		void 
		AboutWindow::onGui() noexcept
		{
		}

		GameComponentPtr 
		AboutWindow::clone() const noexcept
		{
			return std::make_shared<AboutWindow>();
		}
	}
}