#include "main_controller.h"

#include "views/main_menu.h"
#include "views/camera_window.h"
#include "views/message_window.h"
#include "views/theme_manager.h"
#include "views/about_window.h"

#include "mysticlit_behaviour.h"

namespace MysticLit
{
	OctoonImplementSubClass(MainController, octoon::GameComponent, "MainController")

	MainController::MainController() noexcept
	{
	}

	MainController::~MainController() noexcept
	{
	}

	void
	MainController::onActivate() noexcept
	{
		main_ = octoon::GameObject::create("MainView");
		main_->addComponent<octoon::editor::MainMenu>();
		main_->addComponent<octoon::editor::CameraWindow>();
		main_->addComponent<octoon::editor::ThemeManager>();
		main_->addComponent<octoon::editor::MessageWindow>();
		main_->addComponent<octoon::editor::AboutWindow>();

		main_->addComponent<MysticlitBehaviour>();
	}

	void
	MainController::onDeactivate() noexcept
	{
		main_.reset();
	}

	octoon::GameComponentPtr
	MainController::clone() const noexcept
	{
		return std::make_shared<MainController>();
	}
}