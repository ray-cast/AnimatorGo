#include "main_controller.h"

#include "views/main_menu.h"
#include "views/asset_window.h"
#include "views/camera_window.h"
#include "views/hierarchy_window.h"
#include "views/inspector_window.h"
#include "views/message_window.h"
#include "views/theme_manager.h"
#include "views/about_window.h"

#include "controllers/file_controller.h"
#include "controllers/edit_controller.h"

#include <octoon/octoon.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(MainController, GameComponent, "MainController")

		MainController::MainController() noexcept
		{
		}

		MainController::~MainController() noexcept
		{
		}

		void
		MainController::onActivate() noexcept
		{
			main_ = GameObject::create("MainView");
			main_->addComponent<MainMenu>();
			main_->addComponent<CameraWindow>();
			main_->addComponent<AssetWindow>();
			main_->addComponent<HierarchyWindow>();
			main_->addComponent<InspectorWindow>();
			main_->addComponent<ThemeManager>();
			main_->addComponent<MessageWindow>();
			main_->addComponent<AboutWindow>();

			main_->addComponent<FileController>();
			main_->addComponent<EditController>();
		}

		void
		MainController::onDeactivate() noexcept
		{
			main_.reset();
			object_.reset();
		}

		GameComponentPtr 
		MainController::clone() const noexcept
		{
			return std::make_shared<MainController>();
		}
	}
}