#include <octoon/game_features.h>
#include <octoon/game_server.h>
#include <octoon/game_scene.h>

namespace octoon
{
	OctoonImplementSubInterface(GameFeature, runtime::RttiInterface, "GameFeature")

	GameFeature::GameFeature() noexcept
		: isActive_(false)
		, gameServer_(nullptr)
	{
	}

	GameFeature::~GameFeature() noexcept
	{
	}

	void
	GameFeature::setActive(bool active) except
	{
		if (isActive_ != active)
		{
			if (active)
				this->onActivate();
			else
				this->onDeactivate();

			isActive_ = active;
		}
	}
	bool
	GameFeature::getActive() noexcept
	{
		return isActive_;
	}

	void
	GameFeature::setGameListener(const GameListenerPtr& listener) noexcept
	{
		if (gameListener_ != listener)
		{
			this->onListenerChangeBefore();

			gameListener_ = listener;

			this->onListenerChangeAfter();
		}
	}

	const GameListenerPtr&
	GameFeature::getGameListener() const noexcept
	{
		return gameListener_;
	}

	void
	GameFeature::_setGameServer(GameServer* server) noexcept
	{
		gameServer_ = server;
	}

	GameServer*
	GameFeature::getGameServer() noexcept
	{
		return gameServer_;
	}

	void
	GameFeature::onActivate() except
	{
	}

	void
	GameFeature::onDeactivate() noexcept
	{
	}

	void
	GameFeature::onListenerChangeBefore() noexcept
	{
	}

	void
	GameFeature::onListenerChangeAfter() noexcept
	{
	}

	void
	GameFeature::onOpenScene(const GameScenePtr& scene) except
	{
	}

	void
	GameFeature::onCloseScene(const GameScenePtr& scene) noexcept
	{
	}

	void
	GameFeature::onReset() except
	{
	}

	void
	GameFeature::onFrameBegin() except
	{
	}

	void
	GameFeature::onFrame() except
	{
	}

	void
	GameFeature::onFrameEnd() except
	{
	}
}