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
	GameScene::RootObject::get_game_scene() noexcept
	{
		return scene_;
	}

	const GameScene*
	GameScene::RootObject::get_game_scene() const noexcept
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
		assert(root_.unique());

		this->set_active(false);
		root_.unique();

		GameSceneManager::instance()->_unsetScene(this);
	}

	void
	GameScene::set_active(bool active) except
	{
		if (this->get_active() != active)
		{
			GameSceneManager::instance()->_activeScene(this, active);

			root_->set_active_downwards(active);
		}
	}

	bool
	GameScene::get_active() const noexcept
	{
		return root_->get_active();
	}

	void
	GameScene::set_game_listener(const GameListenerPtr& listener) noexcept
	{
		if (game_listener_ != listener)
			game_listener_ = listener;
	}

	GameListenerPtr
	GameScene::get_game_listener() const noexcept
	{
		return game_listener_;
	}

	void
	GameScene::set_name(std::string&& name) noexcept
	{
		name_ = std::move(name);
	}

	void
	GameScene::set_name(const std::string& name) noexcept
	{
		name_ = name;
	}

	const std::string&
	GameScene::get_name() const noexcept
	{
		return name_;
	}

	std::size_t
	GameScene::get_instance_id() const noexcept
	{
		return instance_id_;
	}

	GameObjectPtr
	GameScene::get_root_object() const noexcept
	{
		return root_;
	}

	GameScenePtr
	GameScene::clone() const noexcept
	{
		auto scene = std::make_shared<GameScene>();
		scene->set_name(this->get_name());
		scene->root_ = root_->clone();
		return scene;
	}
}