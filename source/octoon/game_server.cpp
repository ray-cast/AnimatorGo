#include <octoon/game_server.h>
#include <octoon/game_scene.h>
#include <octoon/game_feature.h>
#include <octoon/game_listener.h>

#include <fstream>

namespace octoon
{
	OctoonImplementSubClass(GameServer, Object, "GameServer")

	GameServer::GameServer() noexcept
		: isActived_(false)
		, isQuitRequest_(false)
		, gameApp_(nullptr)
	{
		this->addScene(std::make_shared<GameScene>());
	}

	GameServer::~GameServer() noexcept
	{
		this->cleanupAll();
	}

	void
	GameServer::setActive(bool active) except
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
	GameServer::getActive() const noexcept
	{
		return isActived_;
	}

	void
	GameServer::setGameListener(GameListenerPtr&& listener) noexcept
	{
		if (listener_ != listener)
		{
			for (auto& it : scenes_)
				it->setGameListener(listener);

			listener_ = std::move(listener);
		}
	}

	void
	GameServer::setGameListener(const GameListenerPtr& listener) noexcept
	{
		if (listener_ != listener)
		{
			for (auto& it : scenes_)
				it->setGameListener(listener);

			listener_ = listener;
		}
	}

	const GameListenerPtr&
	GameServer::getGameListener() const noexcept
	{
		return listener_;
	}

	void
	GameServer::log(std::string_view log) noexcept
	{
		if (listener_)
			listener_->onMessage(log);
	}

	bool
	GameServer::isQuitRequest() const noexcept
	{
		return isQuitRequest_;
	}

	bool
	GameServer::openScene(std::string_view filename) noexcept
	{
		assert(!filename.empty());

		try
		{
			std::ifstream stream((std::string)filename);
			if (stream)
			{
				auto scene = std::make_shared<GameScene>();
				scene->_setGameServer(this);
				scene->setGameListener(listener_);
				scene->load(nlohmann::json::parse(stream));

				return this->addScene(scene);
			}
			else
			{
				return false;
			}
		}
		catch (const std::exception& e)
		{
			if (listener_)
				listener_->onMessage(e.what());

			return false;
		}
	}

	void
	GameServer::closeScene(std::string_view sceneName) noexcept
	{
		assert(!sceneName.empty());

		auto scene = this->findScene(sceneName);
		if (scene)
			this->closeScene(scene);
	}

	GameScenePtr
	GameServer::findScene(std::string_view sceneName) noexcept
	{
		assert(!sceneName.empty());

		for (auto& it : scenes_)
		{
			if (it->getName() == sceneName)
				return it;
		}

		return nullptr;
	}

	const GameScenes&
	GameServer::getScenes() const noexcept
	{
		return scenes_;
	}

	bool
	GameServer::addScene(const GameScenePtr& scene) noexcept
	{
		assert(scene);
		assert(std::find(scenes_.begin(), scenes_.end(), scene) == scenes_.end());

		try
		{
			if (listener_)
				listener_->onMessage(std::string("GameServer : Scene adding : ") + scene->getName());

			if (this->getActive())
			{
				for (auto& feature : features_)
				{
					if (listener_)
						listener_->onMessage(std::string("GameServer : Scene adding : ") + scene->getName() + " with feature : " + feature->type_name());

					feature->onOpenScene(scene);
				}

				scene->setActive(true);
			}

			scene->_setGameServer(this);
			scenes_.push_back(scene);

			if (listener_)
				listener_->onMessage(std::string("GameServer : Scene added : ") + scene->getName());

			return true;
		}
		catch (const std::exception& e)
		{
			if (listener_)
				listener_->onMessage(e.what());

			return false;
		}
	}

	void
	GameServer::closeScene(const GameScenePtr& scene) noexcept
	{
		assert(scene);

		auto it = std::find(scenes_.begin(), scenes_.end(), scene);
		if (it != scenes_.end())
		{
			if (listener_)
				listener_->onMessage(std::string("GameServer : Removeing scene with : ") + scene->getName());

			if (this->getActive())
			{
				for (auto& feature : features_)
					feature->onCloseScene(*it);

				(*it)->setActive(false);
			}

			scenes_.erase(it);

			if (listener_)
				listener_->onMessage(std::string("GameServer : Removed scene with : ") + scene->getName());
		}
	}

	void
	GameServer::cleanupScenes() noexcept
	{
		for (auto& it : scenes_)
		{
			if (listener_)
				listener_->onMessage(std::string("GameServer : Stopping : ") + it->type_name() + " : " + it->getName());

			it->setActive(false);

			if (listener_)
				listener_->onMessage(std::string("GameServer : Stopped : ") + it->type_name() + " : " + it->getName());
		}

		for (auto& it : features_)
		{
			for (auto& scene : scenes_)
			{
				if (listener_)
					listener_->onMessage(std::string("GameServer : Stopping feature : ") + it->type_name() + " with scene : " + scene->getName());

				it->onCloseScene(scene);
			}
		}

		scenes_.clear();
	}

	void
	GameServer::addFeature(GameFeaturePtr&& feature) except
	{
		assert(feature);

		auto it = std::find_if(features_.begin(), features_.end(), [&](const GameFeaturePtr& it) { return feature->isInstanceOf(it->rtti()); });
		if (it != features_.end())
		{
			if (listener_)
				listener_->onMessage(std::string("GameServer : Feature with ") + feature->type_name() + " has been added to server.");

			return;
		}

		if (listener_)
			listener_->onMessage(std::string("GameServer : Feature adding: ") + feature->type_name());

		feature->_setGameServer(this);

		if (this->getActive())
		{
			feature->onActivate();

			for (auto& scene : scenes_)
			{
				if (listener_)
					listener_->onMessage(std::string("GameServer : Feature adding: ") + feature->type_name() + " with scene : " + scene->getName());

				feature->onOpenScene(scene);
			}
		}

		features_.push_back(std::move(feature));

		if (listener_)
			listener_->onMessage(std::string("GameServer : Feature added : ") + features_.back()->type_name());
	}

	GameFeature*
	GameServer::getFeature(const Rtti* rtti) const noexcept
	{
		assert(rtti);

		for (auto& it : features_)
		{
			if (it->isInstanceOf(rtti))
				return it.get();
		}

		return nullptr;
	}

	GameFeature*
	GameServer::getFeature(const Rtti& rtti) const noexcept
	{
		return getFeature(&rtti);
	}

	const GameFeatures&
	GameServer::getFeatures() const noexcept
	{
		return features_;
	}

	void
	GameServer::removeFeature(const Rtti* rtti) noexcept
	{
		auto it = std::find_if(features_.begin(), features_.end(), [rtti](const GameFeaturePtr& it) { return it->isInstanceOf(rtti); });
		if (it != features_.end())
		{
			if (listener_)
				listener_->onMessage(std::string("GameServer : Feature removeing: ") + rtti->type_name());

			(*it)->onDeactivate();
			(*it)->_setGameServer(nullptr);

			features_.erase(it);

			if (listener_)
				listener_->onMessage(std::string("GameServer : Feature removed: ") + rtti->type_name());
		}
	}

	void
	GameServer::removeFeature(const Rtti& rtti) noexcept
	{
		return removeFeature(&rtti);
	}

	void
	GameServer::removeFeature(const GameFeaturePtr& feature) noexcept
	{
		assert(feature);
		auto it = std::find(features_.begin(), features_.end(), feature);
		if (it != features_.end())
		{
			if (listener_)
				listener_->onMessage(std::string("GameServer : Feature removeing: ") + feature->type_name());

			(*it)->onDeactivate();
			(*it)->_setGameServer(nullptr);

			features_.erase(it);

			if (listener_)
				listener_->onMessage(std::string("GameServer : Feature removed: ") + feature->type_name());
		}
	}

	void
	GameServer::cleanupAll() noexcept
	{
		this->cleanupScenes();
		this->cleanupFeatures();
	}

	void
	GameServer::cleanupFeatures() noexcept
	{
		for (auto& it : features_)
		{
			if (listener_)
				listener_->onMessage(std::string("GameServer : Stopping : ") + it->type_name());

			it->setActive(false);

			if (listener_)
				listener_->onMessage(std::string("GameServer : Stopped : ") + it->type_name());
		}

		features_.clear();
		dispatchEvents_.clear();
	}

	void
	GameServer::sendMessage(std::string_view event, const std::any& data) noexcept(false)
	{
		dispatchEvents_[std::string(event)].call_all_slots(data);
	}

	void 
	GameServer::addMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept
	{
		dispatchEvents_[std::string(event)].connect(listener);
	}

	void 
	GameServer::removeMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept
	{
		dispatchEvents_[std::string(event)].disconnect(listener);
	}

	void
	GameServer::setGameApp(GameApp* app) noexcept
	{
		gameApp_ = app;
	}

	GameApp*
	GameServer::getGameApp() noexcept
	{
		return gameApp_;
	}

	void
	GameServer::update() noexcept(false)
	{
		if (this->isQuitRequest() || !this->getActive())
			return;

		try
		{
			if (!isQuitRequest_)
			{
				for (auto& it : features_)
					it->onFrameBegin();

				for (auto& it : features_)
					it->onFrame();

				for (auto& it : features_)
					it->onFrameEnd();
			}
		}
		catch (const std::exception& e)
		{
			if (listener_)
				listener_->onMessage(e.what());

			isQuitRequest_ = true;

			throw e;
		}
	}

	void
	GameServer::onActivate() except
	{
		if (this->isQuitRequest())
			return;

		if (listener_)
			listener_->onMessage("GameServer : Starting.");

		try
		{
			for (auto& it : features_)
			{
				if (listener_)
					listener_->onMessage(std::string("GameServer : Starting : ") + it->type_name());

				it->setActive(true);

				if (listener_)
					listener_->onMessage(std::string("GameServer : Started : ") + it->type_name());
			}

			for (auto& it : features_)
			{
				for (auto& scene : scenes_)
				{
					if (listener_)
						listener_->onMessage(std::string("GameServer : Starting feature : ") + it->type_name() + " with scene : " + scene->getName());

					it->onOpenScene(scene);
				}
			}

			for (auto& it : scenes_)
			{
				if (listener_)
					listener_->onMessage(std::string("GameServer : Starting scene : ") + it->getName());

				it->setActive(true);
			}

			if (listener_)
				listener_->onMessage("GameServer : Started.");
		}
		catch (const std::exception& e)
		{
			isQuitRequest_ = true;

			if (listener_)
				listener_->onMessage(std::string("GameServer : except with ") + e.what());

			throw e;
		}
	}

	void
	GameServer::onDeactivate() noexcept
	{
		for (auto& it : features_)
		{
			for (auto& scene : scenes_)
			{
				if (listener_)
					listener_->onMessage(std::string("GameServer : Stopping feature : ") + it->type_name() + " with scene : " + scene->getName());

				it->onCloseScene(scene);
			}
		}

		for (auto& it : features_)
		{
			if (listener_)
				listener_->onMessage(std::string("GameServer : Stopping : ") + it->type_name());

			it->setActive(false);

			if (listener_)
				listener_->onMessage(std::string("GameServer : Stoped : ") + it->type_name());
		}

		for (auto& it : scenes_)
		{
			if (listener_)
				listener_->onMessage(std::string("GameServer : Starting scene : ") + it->getName());

			it->setActive(false);
		}

		if (listener_)
			listener_->onMessage("GameServer : stopped.");
	}
}