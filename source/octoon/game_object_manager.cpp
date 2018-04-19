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

		if (emptyLists_.empty())
		{
			instanceLists_.push_back(entity);
			instanceID = instanceLists_.size();
		}
		else
		{
			std::size_t instance_id_ = emptyLists_.top();
			emptyLists_.pop();
			instanceLists_[instance_id_ - 1] = entity;
			instanceID = instance_id_;
		}
	}

	void
	GameObjectManager::_unsetObject(GameObject* entity) noexcept
	{
		assert(entity);

		auto instanceID = entity->id();
		instanceLists_[instanceID - 1] = nullptr;
		emptyLists_.push(instanceID);
		this->_activeObject(entity, false);
	}

	void
	GameObjectManager::_activeObject(GameObject* entity, bool active) noexcept
	{
		if (active)
		{
			activeActors_.push_back(entity);
		}
		else
		{
			for (auto& actor : activeActors_)
			{
				if (actor == entity)
				{
					actor = nullptr;
					hasEmptyActors_ = true;
					break;
				}
			}
		}
	}

	GameObjectPtr
	GameObjectManager::findObject(const std::string& name) noexcept
	{
		for (auto& it : instanceLists_)
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
		for (auto& it : activeActors_)
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
		for (auto& it : instanceLists_)
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
		for (std::size_t i = 0; i < activeActors_.size(); i++)
		{
			if (activeActors_[i])
				activeActors_[i]->onFrameBegin();
		}
	}

	void
	GameObjectManager::onFrame() noexcept
	{
		for (std::size_t i = 0; i < activeActors_.size(); i++)
		{
			if (activeActors_[i])
				activeActors_[i]->onFrame();
		}
	}

	void
	GameObjectManager::onFrameEnd() noexcept
	{
		for (std::size_t i = 0; i < activeActors_.size(); i++)
		{
			if (activeActors_[i])
				activeActors_[i]->onFrameEnd();
		}

		if (hasEmptyActors_)
		{
			for (auto it = activeActors_.begin(); it != activeActors_.end();)
			{
				if (!(*it))
					it = activeActors_.erase(it);
				else
					++it;
			}

			hasEmptyActors_ = false;
		}
	}

	void
	GameObjectManager::onGui() noexcept
	{
		for (std::size_t i = 0; i < activeActors_.size(); i++)
		{
			if (activeActors_[i])
				activeActors_[i]->onGui();
		}
	}
}