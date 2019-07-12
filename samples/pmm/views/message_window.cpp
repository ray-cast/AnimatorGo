#include "message_window.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(MessageWindow, GameComponent, "MessageWindow")

		MessageWindow::MessageWindow() noexcept
		{
			_isShowMessage = false;
			_isShowMessageFirst = false;
		}

		MessageWindow::~MessageWindow() noexcept
		{
		}

		void
		MessageWindow::onActivate() noexcept
		{
			this->addComponentDispatch(GameDispatchType::Gui);
			this->addMessageListener("editor:message", std::bind(&MessageWindow::showPopupMessage, this, std::placeholders::_1));
			this->addMessageListener("editor:message:error", std::bind(&MessageWindow::showErrorPopupMessage, this, std::placeholders::_1));
		}

		void
		MessageWindow::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();
			this->removeMessageListener("editor:message", std::bind(&MessageWindow::showPopupMessage, this, std::placeholders::_1));
			this->removeMessageListener("editor:message:error", std::bind(&MessageWindow::showErrorPopupMessage, this, std::placeholders::_1));
		}

		void
		MessageWindow::showPopupMessage(const runtime::any& data) noexcept
		{
			if (!_isShowMessageFirst)
			{
				auto pair = runtime::any_cast<std::pair<const char*, const char*>>(data);

				_messageText = pair.second;
				_messageTitle = pair.first;

				_isShowMessageFirst = true;
			}
		}

		void
		MessageWindow::showErrorPopupMessage(const runtime::any& data) noexcept
		{
			if (!_isShowMessageFirst)
			{
				_messageText = runtime::any_cast<const char*>(data);
				_messageTitle = "Error";

				_isShowMessageFirst = true;
			}
		}

		void 
		MessageWindow::onGui() noexcept
		{
			if (_isShowMessageFirst)
			{
				_isShowMessageFirst = false;

				if (_ignoreMessage[_messageTitle])
					return;

				imgui::openPopup(_messageTitle.c_str());

				_isShowMessage = true;
			}

			if (!_isShowMessage)
				return;

			if (imgui::beginPopupModal(_messageTitle.c_str(), &_isShowMessage, imgui::GuiWindowFlagBits::AlwaysAutoResizeBit | imgui::GuiWindowFlagBits::NoSavedSettingsBit))
			{
				imgui::textUnformatted(_messageText.c_str());
				imgui::separator();

				imgui::pushStyleVar(imgui::GuiStyleVar::FramePadding, math::float2::Zero);
				imgui::checkbox("Don't ask me next time?", &_ignoreMessage[_messageText]);
				imgui::popStyleVar();

				if (imgui::button("Ok", math::float2(120, 0))) { imgui::closeCurrentPopup(); _isShowMessage = false; }
				imgui::sameLine();

				if (imgui::button("Cancel", math::float2(120, 0))) { imgui::closeCurrentPopup(); _isShowMessage = false; }

				if (imgui::isKeyDown(input::InputKey::Enter) || imgui::isKeyDown(input::InputKey::KP_Enter))
				{
					_isShowMessage = false;
					imgui::closeCurrentPopup();
				}

				imgui::endPopup();
			}
		}

		GameComponentPtr 
		MessageWindow::clone() const noexcept
		{
			return std::make_shared<MessageWindow>();
		}
	}
}