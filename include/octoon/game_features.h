#ifndef OCTOON_GAME_FEATURE_H_
#define OCTOON_GAME_FEATURE_H_

#include <octoon/game_types.h>

namespace octoon
{
	class OCTOON_EXPORT GameFeature : public runtime::RttiInterface
	{
		OctoonDeclareSubInterface(GameFeature, MessageListener)
	public:
		GameFeature() noexcept;
		virtual ~GameFeature() noexcept;

		void setActive(bool active)  except;
		bool getActive() noexcept;

		void setGameListener(const GameListenerPtr& listener) noexcept;
		const GameListenerPtr& getGameListener() const noexcept;

		GameServer* getGameServer() noexcept;

	protected:
		virtual void onActivate() except;
		virtual void onDeactivate() noexcept;

		virtual void onListenerChangeBefore() noexcept;
		virtual void onListenerChangeAfter() noexcept;

		virtual void onOpenScene(const GameScenePtr& scene) except;
		virtual void onCloseScene(const GameScenePtr& scene) noexcept;

		virtual void onReset() except;

		virtual void onFrameBegin() except;
		virtual void onFrame() except;
		virtual void onFrameEnd() except;

	private:
		friend GameServer;
		void _setGameServer(GameServer* server) noexcept;

	private:
		GameFeature(const GameFeature&) noexcept = delete;
		GameFeature& operator=(const GameFeature&) noexcept = delete;

	private:

		bool isActive_;

		GameServer* gameServer_;
		GameListenerPtr gameListener_;
	};
}

#endif