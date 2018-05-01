#include <octoon/game_server.h>
#include <octoon/game_scene.h>
#include <octoon/game_feature.h>
#include <octoon/game_listener.h>
#include <octoon/runtime/algorithm.h>

namespace octoon
{
	OctoonImplementSubClass(GameServer, runtime::RttiInterface, "GameServer")

	GameServer::GameServer() noexcept
		: is_actived_(false)
		, is_quit_request_(false)
		, game_app_(nullptr)
	{
	}

	GameServer::~GameServer() noexcept
	{
		this->cleanupAll();
	}

	void
	GameServer::setActive(bool active) except
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
	GameServer::getActive() const noexcept
	{
		return is_actived_;
	}

	void
	GameServer::setGameListener(GameListenerPtr&& listener) noexcept
	{
		if (game_listener_ != listener)
		{
			for (auto& it : scenes_)
				it->setGameListener(listener);

			game_listener_ = std::move(listener);
		}
	}

	void
	GameServer::setGameListener(const GameListenerPtr& listener) noexcept
	{
		if (game_listener_ != listener)
		{
			for (auto& it : scenes_)
				it->setGameListener(listener);

			game_listener_ = listener;
		}
	}

	const GameListenerPtr&
	GameServer::getGameListener() const noexcept
	{
		return game_listener_;
	}

	bool
	GameServer::isQuitRequest() const noexcept
	{
		return is_quit_request_;
	}

	bool
	GameServer::openScene(const std::string& filename) noexcept
	{
		assert(!filename.empty());

		try
		{
			auto scene = std::make_shared<GameScene>();
			scene->setGameListener(game_listener_);

			return this->addScene(scene);
		}
		catch (const std::exception& e)
		{
			if (game_listener_)
				game_listener_->onMessage(e.what());

			return false;
		}
	}

	void
	GameServer::closeScene(const std::string& sceneName) noexcept
	{
		assert(!sceneName.empty());

		auto scene = this->findScene(sceneName);
		if (scene)
			this->closeScene(scene);
	}

	GameScenePtr
	GameServer::findScene(const std::string& sceneName) noexcept
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
			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Scene adding : ") + scene->getName());

			if (this->getActive())
			{
				for (auto& feature : features_)
				{
					if (game_listener_)
						game_listener_->onMessage(std::string("GameServer : Scene adding : ") + scene->getName() + " with feature : " + feature->type_name());

					feature->onOpenScene(scene);
				}

				scene->setActive(true);
			}

			scenes_.push_back(scene);

			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Scene added : ") + scene->getName());

			return true;
		}
		catch (const std::exception& e)
		{
			if (game_listener_)
				game_listener_->onMessage(e.what());

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
			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Removeing scene with : ") + scene->getName());

			if (this->getActive())
			{
				for (auto& feature : features_)
					feature->onCloseScene(*it);

				(*it)->setActive(false);
			}

			scenes_.erase(it);

			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Removed scene with : ") + scene->getName());
		}
	}

	void
	GameServer::cleanupScenes() noexcept
	{
		for (auto& it : scenes_)
		{
			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Stopping : ") + it->type_name() + " : " + it->getName());

			it->setActive(false);

			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Stopped : ") + it->type_name() + " : " + it->getName());
		}

		for (auto& it : features_)
		{
			for (auto& scene : scenes_)
			{
				if (game_listener_)
					game_listener_->onMessage(std::string("GameServer : Stopping feature : ") + it->type_name() + " with scene : " + scene->getName());

				it->onCloseScene(scene);
			}
		}

		scenes_.clear();
	}

	void
	GameServer::addFeature(const GameFeaturePtr& feature) except
	{
		assert(feature);

		auto it = octoon::runtime::find_if(features_, [feature](const GameFeaturePtr& it) { return feature->isInstanceOf(it->rtti()); });
		if (it != features_.end())
		{
			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Feature with ") + feature->type_name() + " has been added to server.");

			return;
		}

		if (game_listener_)
			game_listener_->onMessage(std::string("GameServer : Feature adding: ") + feature->type_name());

		feature->_setGameServer(this);

		if (this->getActive())
		{
			feature->onActivate();

			for (auto& scene : scenes_)
			{
				if (game_listener_)
					game_listener_->onMessage(std::string("GameServer : Feature adding: ") + feature->type_name() + " with scene : " + scene->getName());

				feature->onOpenScene(scene);
			}
		}

		features_.push_back(feature);

		if (game_listener_)
			game_listener_->onMessage(std::string("GameServer : Feature added : ") + feature->type_name());
	}

	void
	GameServer::addFeature(GameFeaturePtr&& component) except
	{
		return this->addFeature(component);
	}

	GameFeaturePtr
	GameServer::getFeature(const runtime::Rtti* rtti) const noexcept
	{
		assert(rtti);

		for (auto& it : features_)
		{
			if (it->isInstanceOf(rtti))
				return it;
		}

		return nullptr;
	}

	GameFeaturePtr
	GameServer::getFeature(const runtime::Rtti& rtti) const noexcept
	{
		return getFeature(&rtti);
	}

	const GameFeatures&
	GameServer::getFeatures() const noexcept
	{
		return features_;
	}

	void
	GameServer::removeFeature(const runtime::Rtti* rtti) noexcept
	{
		auto it = octoon::runtime::find_if(features_, [rtti](const GameFeaturePtr& it) { return it->isInstanceOf(rtti); });
		if (it != features_.end())
		{
			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Feature removeing: ") + rtti->type_name());

			(*it)->onDeactivate();
			(*it)->_setGameServer(nullptr);

			features_.erase(it);

			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Feature removed: ") + rtti->type_name());
		}
	}

	void
	GameServer::removeFeature(const runtime::Rtti& rtti) noexcept
	{
		return removeFeature(&rtti);
	}

	void
	GameServer::removeFeature(const GameFeaturePtr& feature) noexcept
	{
		assert(feature);
		auto it = octoon::runtime::find(features_, feature);
		if (it != features_.end())
		{
			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Feature removeing: ") + feature->type_name());

			(*it)->onDeactivate();
			(*it)->_setGameServer(nullptr);

			features_.erase(it);

			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Feature removed: ") + feature->type_name());
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
			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Stopping : ") + it->type_name());

			it->setActive(false);

			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Stopped : ") + it->type_name());
		}

		features_.clear();
	}

	void
	GameServer::sendInputEvent(const input::InputEvent& event) noexcept
	{
		if (this->getActive())
		{
			for (auto& feature : features_)
				feature->onInputEvent(event);
		}
	}

	void
	GameServer::setGameApp(GameApp* app) noexcept
	{
		game_app_ = app;
	}

	GameApp*
	GameServer::getGameApp() noexcept
	{
		return game_app_;
	}

	void
	GameServer::update() noexcept
	{
		if (this->isQuitRequest() || !this->getActive())
			return;

		try
		{
			if (!is_quit_request_)
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
			if (game_listener_)
				game_listener_->onMessage(e.what());

			is_quit_request_ = true;
		}
	}

	void
	GameServer::onActivate() except
	{
		if (this->isQuitRequest())
			return;

		if (game_listener_)
			game_listener_->onMessage("GameServer : Starting.");

		try
		{
			for (auto& it : features_)
			{
				if (game_listener_)
					game_listener_->onMessage(std::string("GameServer : Starting : ") + it->type_name());

				it->setActive(true);

				if (game_listener_)
					game_listener_->onMessage(std::string("GameServer : Started : ") + it->type_name());
			}

			for (auto& it : features_)
			{
				for (auto& scene : scenes_)
				{
					if (game_listener_)
						game_listener_->onMessage(std::string("GameServer : Starting feature : ") + it->type_name() + " with scene : " + scene->getName());

					it->onOpenScene(scene);
				}
			}

			for (auto& it : scenes_)
			{
				if (game_listener_)
					game_listener_->onMessage(std::string("GameServer : Starting scene : ") + it->getName());

				it->setActive(true);
			}

			if (game_listener_)
				game_listener_->onMessage("GameServer : Started.");
		}
		catch (const std::exception& e)
		{
			is_quit_request_ = true;

			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : except with ") + e.what());

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
				if (game_listener_)
					game_listener_->onMessage(std::string("GameServer : Stopping feature : ") + it->type_name() + " with scene : " + scene->getName());

				it->onCloseScene(scene);
			}
		}

		for (auto& it : features_)
		{
			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Stopping : ") + it->type_name());

			it->setActive(false);

			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Stoped : ") + it->type_name());
		}

		for (auto& it : scenes_)
		{
			if (game_listener_)
				game_listener_->onMessage(std::string("GameServer : Starting scene : ") + it->getName());

			it->setActive(false);
		}

		if (game_listener_)
			game_listener_->onMessage("GameServer : stopped.");
	}
}