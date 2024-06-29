#include <octoon/game_scene_manager.h>

namespace octoon
{
	OctoonImplementSingleton(GameSceneManager)

	GameSceneManager::GameSceneManager() noexcept
	{
	}

	GameSceneManager::~GameSceneManager() noexcept
	{
	}

	GameScenePtr
	GameSceneManager::find(std::string_view name) noexcept
	{
		for (auto& it : instanceLists_)
		{
			if (!it)
				continue;

			if (it->getName() == name)
				return it->downcast_pointer<GameScene>();
		}

		return nullptr;
	}

	GameScene*
	GameSceneManager::defaultScene() noexcept
	{
		if (activeScenes_.empty())
			return nullptr;
		return activeScenes_.front();
	}

	void
	GameSceneManager::_instanceScene(GameScene* entity, std::size_t& instanceID) noexcept
	{
		if (emptyLists_.empty())
			instanceLists_.push_back(entity);
		else
		{
			std::size_t instance_id_ = emptyLists_.back();
			emptyLists_.pop_back();
			instanceLists_[instance_id_ - 1] = entity;
		}

		instanceID = instanceLists_.size();
		activeScenes_.push_back(entity);
	}

	void
	GameSceneManager::_unsetScene(GameScene* entity) noexcept
	{
		auto instanceID = entity->id();
		instanceLists_[instanceID - 1] = nullptr;
		emptyLists_.push_back(instanceID);
		this->_activeScene(entity, false);
	}

	void
	GameSceneManager::_activeScene(GameScene* entity, bool active) noexcept
	{
		if (active)
		{
			activeScenes_.push_back(entity);
		}
		else
		{
			std::size_t size = activeScenes_.size();
			for (std::size_t i = 0; i < size; i++)
			{
				if (activeScenes_[i] == entity)
				{
					activeScenes_[i] = nullptr;
					hasEmptyActors_ = true;
					break;
				}
			}
		}
	}
}