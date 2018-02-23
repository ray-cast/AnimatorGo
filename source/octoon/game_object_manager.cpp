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

		if (empty_lists_.empty())
		{
			instance_lists_.push_back(entity);
			instanceID = instance_lists_.size();
		}
		else
		{
			std::size_t instance_id_ = empty_lists_.top();
			empty_lists_.pop();
			instance_lists_[instance_id_ - 1] = entity;
			instanceID = instance_id_;
		}
	}

	void
	GameObjectManager::_unsetObject(GameObject* entity) noexcept
	{
		assert(entity);

		auto instanceID = entity->id();
		instance_lists_[instanceID - 1] = nullptr;
		empty_lists_.push(instanceID);
		this->_activeObject(entity, false);
	}

	void
	GameObjectManager::_activeObject(GameObject* entity, bool active) noexcept
	{
		if (active)
		{
			active_actors_.push_back(entity);
		}
		else
		{
			for (auto& actor : active_actors_)
			{
				if (actor == entity)
				{
					actor = nullptr;
					has_empty_actors_ = true;
					break;
				}
			}
		}
	}

	GameObjectPtr
	GameObjectManager::find_object(const std::string& name) noexcept
	{
		for (auto& it : instance_lists_)
		{
			if (!it)
				continue;

			if (it->get_name() == name)
				return it->downcast_pointer<GameObject>();
		}

		return nullptr;
	}

	GameObjectPtr
	GameObjectManager::find_active_object(const std::string& name) noexcept
	{
		for (auto& it : active_actors_)
		{
			if (!it)
				continue;

			if (it->get_name() == name && it->get_active())
				return it->downcast_pointer<GameObject>();
		}

		return nullptr;
	}

	GameObjectPtr
	GameObjectManager::instantiate(const std::string& name) noexcept
	{
		auto object = this->find_object(name);
		if (object)
			return object->clone();
		return nullptr;
	}

	bool
	GameObjectManager::active_object(const std::string& name) noexcept
	{
		for (auto& it : instance_lists_)
		{
			if (it)
			{
				if (it->get_name() == name)
				{
					it->set_active(true);
					return true;
				}
			}
		}

		return false;
	}

	void
	GameObjectManager::on_frame_begin() noexcept
	{
		for (std::size_t i = 0; i < active_actors_.size(); i++)
		{
			if (active_actors_[i])
				active_actors_[i]->on_frame_begin();
		}
	}

	void
	GameObjectManager::on_frame() noexcept
	{
		for (std::size_t i = 0; i < active_actors_.size(); i++)
		{
			if (active_actors_[i])
				active_actors_[i]->on_frame();
		}
	}

	void
	GameObjectManager::on_frame_end() noexcept
	{
		for (std::size_t i = 0; i < active_actors_.size(); i++)
		{
			if (active_actors_[i])
				active_actors_[i]->on_frame_end();
		}

		if (has_empty_actors_)
		{
			for (auto it = active_actors_.begin(); it != active_actors_.end();)
			{
				if (!(*it))
					it = active_actors_.erase(it);
				else
					++it;
			}

			has_empty_actors_ = false;
		}
	}

	void
	GameObjectManager::on_gui() noexcept
	{
		for (std::size_t i = 0; i < active_actors_.size(); i++)
		{
			if (active_actors_[i])
				active_actors_[i]->on_gui();
		}
	}
}