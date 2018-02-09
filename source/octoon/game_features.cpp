#include <octoon/game_features.h>
#include <octoon/game_server.h>
#include <octoon/game_scene.h>

namespace octoon
{
	OctoonImplementSubInterface(GameFeature, runtime::RttiInterface, "GameFeature")

	GameFeature::GameFeature() noexcept
		: _isActive(false)
		, _gameServer(nullptr)
	{
	}

	GameFeature::~GameFeature() noexcept
	{
	}

	void
	GameFeature::setActive(bool active) except
	{
		if (_isActive != active)
		{
			if (active)
				this->onActivate();
			else
				this->onDeactivate();

			_isActive = active;
		}
	}
	bool
	GameFeature::getActive() noexcept
	{
		return _isActive;
	}

	void
	GameFeature::setGameListener(const GameListenerPtr& listener) noexcept
	{
		if (_gameListener != listener)
		{
			this->onListenerChangeBefore();

			_gameListener = listener;

			this->onListenerChangeAfter();
		}
	}

	const GameListenerPtr&
	GameFeature::getGameListener() const noexcept
	{
		return _gameListener;
	}

	void
	GameFeature::_setGameServer(GameServer* server) noexcept
	{
		_gameServer = server;
	}

	GameServer*
	GameFeature::getGameServer() noexcept
	{
		return _gameServer;
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