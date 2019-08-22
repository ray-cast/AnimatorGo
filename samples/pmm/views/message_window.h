#ifndef MYSTICLIT_UI_MESSAGE_WINDOW_H_
#define MYSTICLIT_UI_MESSAGE_WINDOW_H_

#include <octoon/game_component.h>

namespace MysticLit
{
	class MessageWindow final : public octoon::GameComponent
	{
		OctoonDeclareSubClass(MessageWindow, octoon::GameComponent)
	public:
		MessageWindow() noexcept;
		~MessageWindow() noexcept;

		octoon::GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onGui() noexcept override;

		void showPopupMessage(const octoon::runtime::any& data) noexcept;
		void showErrorPopupMessage(const octoon::runtime::any& data) noexcept;

	private:
		bool _isShowMessage;
		bool _isShowMessageFirst;

		std::string _messageTitle;
		std::string _messageText;
		std::map<std::string, bool> _ignoreMessage;
	};
}

#endif
