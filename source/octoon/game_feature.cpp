#include <octoon/game_feature.h>
#include <octoon/game_server.h>
#include <octoon/game_scene.h>

namespace octoon
{
	OctoonImplementSubInterface(GameFeature, runtime::RttiInterface, "GameFeature")

	GameFeature::GameFeature() noexcept
		: isActived_(false)
		, server_(nullptr)
	{
	}

	GameFeature::~GameFeature() noexcept
	{
		assert(!isActived_);
	}

	void
	GameFeature::setActive(bool active) except
	{
		if (isActived_ != active)
		{
			if (active)
				this->onActivate();
			else
				this->onDeactivate();

			isActived_ = active;
		}
	}
	bool
	GameFeature::getActive() noexcept
	{
		return isActived_;
	}

	const GameListenerPtr&
	GameFeature::getGameListener() const noexcept
	{
		assert(server_);
		return server_->getGameListener();
	}

	GameFeature*
	GameFeature::getFeature(const runtime::Rtti* type) const noexcept
	{
		assert(server_);
		assert(this->rtti() != type);
		return server_->getFeature(type);
	}

	GameFeature*
	GameFeature::getFeature(const runtime::Rtti& type) const noexcept
	{
		assert(server_);
		assert(this->rtti() != &type);
		return server_->getFeature(type);
	}

	const GameFeatures&
	GameFeature::getFeaturs() const noexcept
	{
		assert(server_);
		return server_->getFeatures();
	}

	void
	GameFeature::_setGameServer(GameServer* server) noexcept
	{
		assert(!server_);
		server_ = server;
	}

	GameServer*
	GameFeature::getGameServer() noexcept
	{
		return server_;
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