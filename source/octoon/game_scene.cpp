#include <octoon/game_scene.h>
#include <octoon/game_scene_manager.h>
#include <octoon/game_listener.h>
#include <octoon/game_component.h>

namespace octoon
{
	OctoonImplementSubClass(GameScene, runtime::RttiInterface, "GameScene")

	GameScene::RootObject::RootObject(GameScene* scene) noexcept
		: _scene(scene)
	{
		assert(scene);
	}

	GameScene::RootObject::~RootObject() noexcept
	{
	}

	GameScene*
	GameScene::RootObject::getGameScene() noexcept
	{
		return _scene;
	}

	const GameScene*
	GameScene::RootObject::getGameScene() const noexcept
	{
		return _scene;
	}

	GameScene::GameScene() noexcept
		: _root(std::make_unique<RootObject>(this))
	{
		GameSceneManager::instance()->_instanceScene(this, instanceID_);
	}

	GameScene::GameScene(const std::string& name) noexcept
		: name_(name)
	{
		GameSceneManager::instance()->_instanceScene(this, instanceID_);
	}

	GameScene::~GameScene() noexcept
	{
		assert(_root.unique());

		this->setActive(false);
		_root.unique();

		GameSceneManager::instance()->_unsetScene(this);
	}

	void
	GameScene::setActive(bool active) except
	{
		if (this->getActive() != active)
		{
			GameSceneManager::instance()->_activeScene(this, active);

			_root->setActiveDownwards(active);
		}
	}

	bool
	GameScene::getActive() const noexcept
	{
		return _root->getActive();
	}

	void
	GameScene::setGameListener(const GameListenerPtr& listener) noexcept
	{
		if (gameListener_ != listener)
			gameListener_ = listener;
	}

	GameListenerPtr
	GameScene::getGameListener() const noexcept
	{
		return gameListener_;
	}

	void
	GameScene::setName(std::string&& name) noexcept
	{
		name_ = std::move(name);
	}

	void
	GameScene::setName(const std::string& name) noexcept
	{
		name_ = name;
	}

	const std::string&
	GameScene::getName() const noexcept
	{
		return name_;
	}

	std::size_t
	GameScene::getInstanceID() const noexcept
	{
		return instanceID_;
	}

	GameObjectPtr
	GameScene::getRootObject() const noexcept
	{
		return _root;
	}

	GameScenePtr
	GameScene::clone() const noexcept
	{
		auto scene = std::make_shared<GameScene>();
		scene->setName(this->getName());
		scene->_root = _root->clone();
		return scene;
	}
}