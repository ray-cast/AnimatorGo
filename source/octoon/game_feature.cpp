#include <octoon/game_feature.h>
#include <octoon/game_server.h>
#include <octoon/game_scene.h>

namespace octoon
{
	OctoonImplementSubInterface(GameFeature, runtime::RttiInterface, "GameFeature")

	GameFeature::GameFeature() noexcept
		: is_actived_(false)
		, game_server_(nullptr)
	{
	}

	GameFeature::~GameFeature() noexcept
	{
		assert(!is_actived_);
	}

	void
	GameFeature::setActive(bool active) except
	{
		if (is_actived_ != active)
		{
			if (active)
				this->onActivate();
			else
				this->onDeactivate();

			is_actived_ = active;
		}
	}
	bool
	GameFeature::getActive() noexcept
	{
		return is_actived_;
	}

	const GameListenerPtr&
	GameFeature::getGameListener() const noexcept
	{
		assert(game_server_);
		return game_server_->getGameListener();
	}

	GameFeaturePtr
	GameFeature::getFeature(const runtime::Rtti* type) const noexcept
	{
		assert(game_server_);
		assert(this->rtti() != type);
		return game_server_->getFeature(type);
	}

	GameFeaturePtr
	GameFeature::getFeature(const runtime::Rtti& type) const noexcept
	{
		assert(game_server_);
		assert(this->rtti() != &type);
		return game_server_->getFeature(type);
	}

	const GameFeatures&
	GameFeature::getFeatures() const noexcept
	{
		assert(game_server_);
		return game_server_->getFeatures();
	}

	void
	GameFeature::_setGameServer(GameServer* server) noexcept
	{
		assert(!game_server_);
		game_server_ = server;
	}

	GameServer*
	GameFeature::getGameServer() noexcept
	{
		return game_server_;
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
	GameFeature::onOpenScene(const GameScenePtr& scene) except
	{
	}

	void
	GameFeature::onCloseScene(const GameScenePtr& scene) noexcept
	{
	}

	void
	GameFeature::onInputEvent(const input::InputEvent& event) noexcept
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