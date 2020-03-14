#include <octoon/game_feature.h>
#include <octoon/game_server.h>

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
	GameFeature::sendMessage(std::string_view event, const std::any& data) noexcept
	{
		assert(server_);
		server_->sendMessage(event, data);
	}

	void
	GameFeature::addMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept
	{
		assert(server_);
		server_->addMessageListener(event, listener);
	}

	void
	GameFeature::removeMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept
	{
		assert(server_);
		server_->removeMessageListener(event, listener);
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