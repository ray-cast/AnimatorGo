#ifndef OCTOON_UI_MESSAGE_WINDOW_H_
#define OCTOON_UI_MESSAGE_WINDOW_H_

#include <octoon/game_component.h>

namespace octoon
{
	namespace editor
	{
		class MessageWindow final : public GameComponent
		{
			OctoonDeclareSubClass(MessageWindow, GameComponent)
		public:
			MessageWindow() noexcept;
			~MessageWindow() noexcept;

			GameComponentPtr clone() const noexcept override;

		private:
			void onActivate() noexcept override;
			void onDeactivate() noexcept override;

			void onGui() noexcept override;

			void showPopupMessage(const std::any& data) noexcept;

		private:
			bool _isShowMessage;
			bool _isShowMessageFirst;

			std::string _messageTitle;
			std::string _messageText;
			std::map<std::string, bool> _ignoreMessage;
		};
	}
}

#endif
