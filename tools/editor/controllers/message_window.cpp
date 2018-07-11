#include "message_window.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(MessageWindow, GameComponent, "UIViewComponent")

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
		}

		void
		MessageWindow::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();
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
		MessageWindow::onGui() noexcept
		{
			if (_isShowMessageFirst)
			{
				_isShowMessageFirst = false;

				if (_ignoreMessage[_messageTitle])
					return;

				imgui::open_popup(_messageTitle.c_str());

				_isShowMessage = true;
			}

			if (!_isShowMessage)
				return;

			if (imgui::begin_popup_modal(_messageTitle.c_str(), &_isShowMessage, imgui::GuiWindowFlagBits::AlwaysAutoResizeBit | imgui::GuiWindowFlagBits::NoSavedSettingsBit))
			{
				imgui::text_unformatted(_messageText.c_str());
				imgui::separator();

				imgui::push_style_var(imgui::GuiStyleVar::FramePadding, math::float2::Zero);
				imgui::checkbox("Don't ask me next time?", &_ignoreMessage[_messageText]);
				imgui::pop_style_var();

				if (imgui::button("Ok", math::float2(120, 0))) { imgui::close_current_popup(); _isShowMessage = false; }
				imgui::same_line();

				if (imgui::button("Cancel", math::float2(120, 0))) { imgui::close_current_popup(); _isShowMessage = false; }

				if (imgui::is_key_down(input::InputKey::Enter) || imgui::is_key_down(input::InputKey::KP_Enter))
				{
					_isShowMessage = false;
					imgui::close_current_popup();
				}

				imgui::end_popup();
			}
		}

		GameComponentPtr 
		MessageWindow::clone() const noexcept
		{
			return std::make_shared<MessageWindow>();
		}
	}
}