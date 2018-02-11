#include <octoon/game_scene_manager.h>
#include <octoon/game_scene.h>

namespace octoon
{
	GameSceneManager::GameSceneManager() noexcept
	{
	}

	GameSceneManager::~GameSceneManager() noexcept
	{
	}

	void
	GameSceneManager::_instanceScene(GameScene* entity, std::size_t& instanceID) noexcept
	{
		if (empty_lists_.empty())
			instance_lists_.push_back(entity);
		else
		{
			std::size_t instance_id_ = empty_lists_.back();
			empty_lists_.pop_back();
			instance_lists_[instance_id_ - 1] = entity;
		}

		instanceID = instance_lists_.size();
	}

	void
	GameSceneManager::_unsetScene(GameScene* entity) noexcept
	{
		auto instanceID = entity->get_instance_id();
		instance_lists_[instanceID - 1] = nullptr;
		empty_lists_.push_back(instanceID);
		this->_activeScene(entity, false);
	}

	void
	GameSceneManager::_activeScene(GameScene* entity, bool active) noexcept
	{
		if (active)
		{
			active_actors_.push_back(entity);
		}
		else
		{
			std::size_t size = active_actors_.size();
			for (std::size_t i = 0; i < size; i++)
			{
				if (active_actors_[i] == entity)
				{
					active_actors_[i] = nullptr;
					has_empty_actors_ = true;
					break;
				}
			}
		}
	}

	GameScenePtr
	GameSceneManager::find_scene(const std::string& name) noexcept
	{
		for (auto& it : instance_lists_)
		{
			if (!it)
				continue;

			if (it->get_name() == name)
				return it->downcast_pointer<GameScene>();
		}

		return nullptr;
	}

	GameScenePtr
	GameSceneManager::find_actived_scene(const std::string& name) noexcept
	{
		for (auto& it : active_actors_)
		{
			if (!it)
				continue;

			if (it->get_name() == name && it->get_active())
				return it->downcast_pointer<GameScene>();
		}

		return nullptr;
	}

	GameScenePtr
	GameSceneManager::instantiate(const std::string& name) except
	{
		auto scene = this->find_scene(name);
		if (scene)
			return scene->clone();
		return nullptr;
	}

	bool
	GameSceneManager::active_scene(const std::string& name) noexcept
	{
		for (auto& it : instance_lists_)
		{
			if (!it)
				continue;

			if (it->get_name() == name)
			{
				it->set_active(true);
				return true;
			}
		}

		return false;
	}

	void
	GameSceneManager::on_frame_begin() noexcept
	{
	}

	void
	GameSceneManager::on_frame() noexcept
	{
	}

	void
	GameSceneManager::on_frame_end() noexcept
	{
	}
}