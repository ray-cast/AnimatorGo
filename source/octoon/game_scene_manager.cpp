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

	bool
	GameSceneManager::open() noexcept
	{
		return true;
	}

	void
	GameSceneManager::close() noexcept
	{
	}

	void
	GameSceneManager::_instanceScene(GameScene* entity, std::size_t& instanceID) noexcept
	{
		if (_emptyLists.empty())
			_instanceLists.push_back(entity);
		else
		{
			std::size_t _instanceID = _emptyLists.back();
			_emptyLists.pop_back();
			_instanceLists[_instanceID - 1] = entity;
		}

		instanceID = _instanceLists.size();
	}

	void
	GameSceneManager::_unsetScene(GameScene* entity) noexcept
	{
		auto instanceID = entity->getInstanceID();
		_instanceLists[instanceID - 1] = nullptr;
		_emptyLists.push_back(instanceID);
		this->_activeScene(entity, false);
	}

	void
	GameSceneManager::_activeScene(GameScene* entity, bool active) noexcept
	{
		if (active)
		{
			_activeActors.push_back(entity);
		}
		else
		{
			std::size_t size = _activeActors.size();
			for (std::size_t i = 0; i < size; i++)
			{
				if (_activeActors[i] == entity)
				{
					_activeActors[i] = nullptr;
					_hasEmptyActors = true;
					break;
				}
			}
		}
	}

	GameScenePtr
	GameSceneManager::findScene(const std::string& name) noexcept
	{
		for (auto& it : _instanceLists)
		{
			if (!it)
				continue;

			if (it->getName() == name)
				return it->downcast_pointer<GameScene>();
		}

		return nullptr;
	}

	GameScenePtr
	GameSceneManager::findActiveScene(const std::string& name) noexcept
	{
		for (auto& it : _activeActors)
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
		for (auto& it : _instanceLists)
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