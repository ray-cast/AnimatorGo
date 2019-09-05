#include "ui_component.h"

#include "views/main_menu.h"
#include "views/camera_window.h"
#include "views/message_window.h"
#include "views/theme_manager.h"
#include "views/about_window.h"

#include "mysticlit_behaviour.h"

namespace MysticLit
{
	UIComponent::UIComponent() noexcept
	{
	}

	UIComponent::~UIComponent() noexcept
	{
	}

	void
	UIComponent::setActive(bool active) noexcept
	{
	}

	bool
	UIComponent::getActive() const noexcept
	{
		return true;
	}

	void
	UIComponent::addMessageListener(const std::string& event, std::function<void(const octoon::runtime::any&)> listener) noexcept
	{
		main_->addMessageListener(event, listener);
	}
	
	void
	UIComponent::removeMessageListener(const std::string& event, std::function<void(const octoon::runtime::any&)> listener) noexcept
	{
		main_->removeMessageListener(event, listener);
	}

	void
	UIComponent::init(const std::shared_ptr<MysticLitContext>& context, const std::shared_ptr<CanvasModule>& model) noexcept
	{
		this->setContext(context);
		this->setModel(model);
		this->onEnable();
	}

	void
	UIComponent::onEnable() noexcept
	{
		if (!main_)
		{
			main_ = octoon::GameObject::create("MainView");
			//main_->addComponent<MainMenu>();
			main_->addComponent<CameraWindow>();
			main_->addComponent<ThemeManager>();
			main_->addComponent<MessageWindow>();
			main_->addComponent<AboutWindow>();

			this->getContext()->behaviour->addMessageListener("editor:camera:set", 
			std::bind([&](const octoon::runtime::any& data) 
			{
				main_->sendMessage("editor:camera:set", data); 
			}, std::placeholders::_1));
		}
	}

	void
	UIComponent::onDisable() noexcept
	{
		main_.reset();
	}
}