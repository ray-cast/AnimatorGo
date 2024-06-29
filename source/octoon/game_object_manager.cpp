#include <octoon/game_object_manager.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSingleton(GameObjectManager)

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

		std::unique_lock<std::shared_mutex> guard_lock(lock_);
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
		std::unique_lock<std::shared_mutex> guard_lock(lock_);
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
			assert(std::find(activeActors_.begin(), activeActors_.end(), entity) == activeActors_.end());
			activeActors_.push_back(entity);
		}
		else
		{
			auto it = std::find(activeActors_.begin(), activeActors_.end(), entity);
			if (it != activeActors_.end())
			{
				*it = nullptr;
				hasEmptyActors_ = true;
			}
		}
	}

	GameObjectPtr
	GameObjectManager::find(std::string_view name) noexcept
	{
		std::shared_lock<std::shared_mutex> guard_lock(lock_);

		for (auto& it : instanceLists_)
		{
			if (it)
			{
				if (it->getName() == name)
					return it->downcast_pointer<GameObject>();
			}
		}

		return nullptr;
	}

	const GameObjectRaws&
	GameObjectManager::instances() const noexcept
	{
		return instanceLists_;
	}

	void
	GameObjectManager::sendMessage(std::string_view event, const std::any& data) noexcept(false)
	{
		auto it = dispatchEvents_.find(event);
		if (it != dispatchEvents_.end())
			(*it).second.call_all_slots(data);
	}

	void
	GameObjectManager::addMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept
	{
		auto it = dispatchEvents_.find(event);
		if (it != dispatchEvents_.end())
			(*it).second.connect(listener);
		else
			dispatchEvents_[std::string(event)].connect(listener);
	}

	void
	GameObjectManager::removeMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept
	{
		auto it = dispatchEvents_.find(event);
		if (it != dispatchEvents_.end())
			(*it).second.disconnect(listener);
	}

	void
	GameObjectManager::onFixedUpdate() except
	{
		for (std::size_t i = 0; i < activeActors_.size(); i++)
		{
			if (activeActors_[i])
				activeActors_[i]->onFixedUpdate();
		}
	}

	void
	GameObjectManager::onUpdate() except
	{
		for (std::size_t i = 0; i < activeActors_.size(); i++)
		{
			if (activeActors_[i])
				activeActors_[i]->onUpdate();
		}
	}

	void
	GameObjectManager::onLateUpdate() except
	{
		for (std::size_t i = 0; i < activeActors_.size(); i++)
		{
			if (activeActors_[i])
				activeActors_[i]->onLateUpdate();
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
	GameObjectManager::onGui() except
	{
		for (std::size_t i = 0; i < activeActors_.size(); i++)
		{
			if (activeActors_[i])
				activeActors_[i]->onGui();
		}
	}
}