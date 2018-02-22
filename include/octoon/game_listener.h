#ifndef OCTOON_GAME_LISTENER_H_
#define OCTOON_GAME_LISTENER_H_

#include <octoon/game_types.h>

namespace octoon
{
	class GameListener : public runtime::RttiInterface
	{
		OctoonDeclareSubInterface(GameListener, runtime::RttiInterface)
	public:
		GameListener() noexcept = default;
		virtual ~GameListener() = default;

		virtual void on_message(const std::string& message) noexcept = 0;
	};
}

#endif