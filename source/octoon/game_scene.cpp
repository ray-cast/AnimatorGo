#include <octoon/game_scene.h>
#include <octoon/game_scene_manager.h>
#include <octoon/game_listener.h>
#include <octoon/game_component.h>
#include <octoon/game_server.h>
#include <octoon/game_base_features.h>
#include <octoon/game_object_manager.h>

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
		, gameServer_(nullptr)
	{
		GameSceneManager::instance()->_instanceScene(this, instance_id_);
	}

	GameScene::GameScene(io::archivebuf& reader) noexcept
		: GameScene()
	{
		this->load(reader);
	}

	GameScene::GameScene(std::string&& name) noexcept
		: GameScene()
	{
		this->setName(std::move(name));
	}

	GameScene::GameScene(std::string_view name) noexcept
		: GameScene()
	{
		this->setName(name);
	}

	GameScene::~GameScene() noexcept
	{
		assert(root_.use_count() == 1);

		this->setActive(false);

		GameSceneManager::instance()->_unsetScene(this);
	}

	std::size_t
	GameScene::id() const noexcept
	{
		return instance_id_;
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

	const GameListenerPtr&
	GameScene::getGameListener() const noexcept
	{
		return listener_;
	}

	void
	GameScene::setName(std::string_view name) noexcept
	{
		name_ = name;
	}

	const std::string&
	GameScene::getName() const noexcept
	{
		return name_;
	}

	GameFeature*
	GameScene::getFeature(const runtime::Rtti* rtti) const noexcept
	{
		assert(gameServer_);
		return gameServer_->getFeature(rtti);
	}

	GameFeature*
	GameScene::getFeature(const runtime::Rtti& rtti) const noexcept
	{
		assert(gameServer_);
		return gameServer_->getFeature(rtti);
	}

	void
	GameScene::sendMessage(std::string_view event, const std::any& data) noexcept
	{
		assert(gameServer_);
		return gameServer_->sendMessage(event, data);
	}

	void
	GameScene::load(const io::archivebuf& reader) except
	{
		root_->load(reader);
	}

	void
	GameScene::save(io::archivebuf& write) except
	{
		root_->save(write);
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

	GameScenePtr 
	GameScene::find(std::string_view name) noexcept
	{
		return GameSceneManager::instance()->find(name);
	}

	void
	GameScene::_setGameServer(GameServer* gameServ) noexcept
	{
		this->gameServer_ = gameServ;
	}
}