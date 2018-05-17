#include <octoon/game_scene_manager.h>
#include <octoon/game_scene.h>

namespace octoon
{
	OctoonImplementSingleton(GameSceneManager)

	GameSceneManager::GameSceneManager() noexcept
	{
	}

	GameSceneManager::~GameSceneManager() noexcept
	{
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
			activeActors_.push_back(entity);
		}
		else
		{
			std::size_t size = activeActors_.size();
			for (std::size_t i = 0; i < size; i++)
			{
				if (activeActors_[i] == entity)
				{
					activeActors_[i] = nullptr;
					hasEmptyActors_ = true;
					break;
				}
			}
		}
	}

	GameScenePtr
	GameSceneManager::findScene(const std::string& name) noexcept
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

	GameScenePtr
	GameSceneManager::findActivedScene(const std::string& name) noexcept
	{
		for (auto& it : activeActors_)
		{
			if (!it)
				continue;

			if (it->getName() == name && it->getActive())
				return it->downcast_pointer<GameScene>();
		}

		return nullptr;
	}

	GameScenePtr
	GameSceneManager::instantiate(const std::string& name) except
	{
		auto scene = this->findScene(name);
		if (scene)
			return scene->clone();
		return nullptr;
	}

	bool
	GameSceneManager::activeScene(const std::string& name) noexcept
	{
		for (auto& it : instanceLists_)
		{
			if (!it)
				continue;

			if (it->getName() == name)
			{
				it->setActive(true);
				return true;
			}
		}

		return false;
	}

	void
	GameSceneManager::onFrameBegin() noexcept
	{
	}

	void
	GameSceneManager::onFrame() noexcept
	{
	}

	void
	GameSceneManager::onFrameEnd() noexcept
	{
	}
}