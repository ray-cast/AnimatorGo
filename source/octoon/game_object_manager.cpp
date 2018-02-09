#include <octoon/game_object_manager.h>
#include <octoon/game_object.h>

namespace octoon
{
	GameObjectManager::GameObjectManager() noexcept
	{
	}

	GameObjectManager::~GameObjectManager() noexcept
	{
	}

	void
	GameObjectManager::_instanceObject(GameObject* entity, std::size_t& instanceID) noexcept
	{
		assert(entity);

		if (_emptyLists.empty())
		{
			_instanceLists.push_back(entity);
			instanceID = _instanceLists.size();
		}
		else
		{
			std::size_t _instanceID = _emptyLists.top();
			_emptyLists.pop();
			_instanceLists[_instanceID - 1] = entity;
			instanceID = _instanceID;
		}
	}

	void
	GameObjectManager::_unsetObject(GameObject* entity) noexcept
	{
		assert(entity);

		auto instanceID = entity->getInstanceID();
		_instanceLists[instanceID - 1] = nullptr;
		_emptyLists.push(instanceID);
		this->_activeObject(entity, false);
	}

	void
	GameObjectManager::_activeObject(GameObject* entity, bool active) noexcept
	{
		if (active)
		{
			_activeActors.push_back(entity);
		}
		else
		{
			for (auto& actor : _activeActors)
			{
				if (actor == entity)
				{
					actor = nullptr;
					_hasEmptyActors = true;
					break;
				}
			}
		}
	}

	GameObjectPtr
	GameObjectManager::findObject(const std::string& name) noexcept
	{
		for (auto& it : _instanceLists)
		{
			if (!it)
				continue;

			if (it->getName() == name)
				return it->downcast_pointer<GameObject>();
		}

		return nullptr;
	}

	GameObjectPtr
	GameObjectManager::findActiveObject(const std::string& name) noexcept
	{
		for (auto& it : _activeActors)
		{
			if (!it)
				continue;

			if (it->getName() == name && it->getActive())
				return it->downcast_pointer<GameObject>();
		}

		return nullptr;
	}

	GameObjectPtr
	GameObjectManager::instantiate(const std::string& name) noexcept
	{
		auto object = this->findObject(name);
		if (object)
			return object->clone();
		return nullptr;
	}

	bool
	GameObjectManager::activeObject(const std::string& name) noexcept
	{
		for (auto& it : _instanceLists)
		{
			if (it)
			{
				if (it->getName() == name)
				{
					it->setActive(true);
					return true;
				}
			}
		}

		return false;
	}

	void
	GameObjectManager::onFrameBegin() noexcept
	{
		for (std::size_t i = 0; i < _activeActors.size(); i++)
		{
			if (_activeActors[i])
				_activeActors[i]->_onFrameBegin();
		}
	}

	void
	GameObjectManager::onFrame() noexcept
	{
		for (std::size_t i = 0; i < _activeActors.size(); i++)
		{
			if (_activeActors[i])
				_activeActors[i]->_onFrame();
		}
	}

	void
	GameObjectManager::onFrameEnd() noexcept
	{
		for (std::size_t i = 0; i < _activeActors.size(); i++)
		{
			if (_activeActors[i])
				_activeActors[i]->_onFrameEnd();
		}

		if (_hasEmptyActors)
		{
			for (auto it = _activeActors.begin(); it != _activeActors.end();)
			{
				if (!(*it))
					it = _activeActors.erase(it);
				else
					++it;
			}

			_hasEmptyActors = false;
		}
	}
}