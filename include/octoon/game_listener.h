#ifndef OCTOON_GAME_LISTENER_H_
#define OCTOON_GAME_LISTENER_H_

#include <octoon/game_types.h>

namespace octoon
{
	class OCTOON_EXPORT GameListener : public Object
	{
		OctoonDeclareSubInterface(GameListener, Object)
	public:
		GameListener() noexcept = default;
		virtual ~GameListener() = default;

		virtual void onMessage(std::string_view message) noexcept = 0;
	};
}

#endif