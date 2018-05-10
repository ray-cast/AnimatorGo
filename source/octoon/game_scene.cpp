#include <octoon/game_scene.h>
#include <octoon/game_scene_manager.h>
#include <octoon/game_listener.h>
#include <octoon/game_component.h>

namespace octoon
{
	OctoonImplementSubClass(GameScene, runtime::RttiInterface, "GameScene")

	GameScene::RootObject::RootObject(GameScene* scene) noexcept
		: scene_(scene)
	{
		assert(scene);
	}

	GameScene::RootObject::~RootObject() noexcept
	{
	}

	GameScene*
	GameScene::RootObject::getGameScene() noexcept
	{
		return scene_;
	}

	const GameScene*
	GameScene::RootObject::getGameScene() const noexcept
	{
		return scene_;
	}

	GameScene::GameScene() noexcept
		: root_(std::make_unique<RootObject>(this))
	{
		GameSceneManager::instance()->_instanceScene(this, instance_id_);
	}

	GameScene::GameScene(const std::string& name) noexcept
		: name_(name)
	{
		GameSceneManager::instance()->_instanceScene(this, instance_id_);
	}

	GameScene::~GameScene() noexcept
	{
		assert(root_.use_count() == 1);

		this->setActive(false);

		GameSceneManager::instance()->_unsetScene(this);
	}

	void
	GameScene::setActive(bool active) except
	{
		if (this->getActive() != active)
		{
			GameSceneManager::instance()->_activeScene(this, active);

			root_->setActiveDownwards(active);
		}
	}

	bool
	GameScene::getActive() const noexcept
	{
		return root_->getActive();
	}

	void
	GameScene::setGameListener(const GameListenerPtr& listener) noexcept
	{
		if (listener_ != listener)
			listener_ = listener;
	}

	GameListenerPtr
	GameScene::getGameListener() const noexcept
	{
		return listener_;
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
	GameScene::id() const noexcept
	{
		return instance_id_;
	}

	const GameObjectPtr&
	GameScene::root() const noexcept
	{
		return root_;
	}

	GameScenePtr
	GameScene::clone() const noexcept
	{
		auto scene = std::make_shared<GameScene>();
		scene->setName(this->getName());
		scene->root_ = root_->clone();
		return scene;
	}
}