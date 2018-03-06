#include <octoon/game_object.h>
#include <octoon/game_object_manager.h>
#include <octoon/game_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(GameObject, runtime::RttiInterface, "Object")

	GameObject::GameObject() noexcept
		: active_(false)
		, layer_(0)
	{
		GameObjectManager::instance()->_instanceObject(this, instance_id_);

		this->add_component<TransformComponent>();
	}

	GameObject::~GameObject() noexcept
	{
		this->cleanup_children();
		this->cleanup_components();

		GameObjectManager::instance()->_unsetObject(this);
	}

	void
	GameObject::set_name(const std::string& name) noexcept
	{
		name_ = name;
	}

	void
	GameObject::set_name(std::string&& name) noexcept
	{
		name_ = std::move(name);
	}

	const std::string&
	GameObject::get_name() const noexcept
	{
		return name_;
	}

	void
	GameObject::set_active(bool active) except
	{
		if (active_ != active)
		{
			if (active)
				this->on_activate();
			else
				this->on_deactivate();

			active_ = active;
		}
	}

	void
	GameObject::set_active_upwards(bool active) except
	{
		if (active_ != active)
		{
			if (active)
				this->on_activate();
			else
				this->on_deactivate();

			auto parent = this->get_parent();
			if (parent)
				parent->set_active_upwards(active);

			active_ = active;
		}
	}

	void
	GameObject::set_active_downwards(bool active) except
	{
		if (active_ != active)
		{
			if (active)
				this->on_activate();
			else
				this->on_deactivate();

			for (auto& it : children_)
				it->set_active_downwards(active);

			active_ = active;
		}
	}

	bool
	GameObject::get_active() const noexcept
	{
		return active_;
	}

	void
	GameObject::set_layer(std::uint8_t layer) noexcept
	{
		if (layer_ != layer)
		{
			this->on_layer_change_before();

			layer_ = layer;

			this->on_layer_change_after();
		}
	}

	std::uint8_t
	GameObject::get_layer() const noexcept
	{
		return layer_;
	}

	std::size_t
	GameObject::id() const noexcept
	{
		return instance_id_;
	}

	void
	GameObject::set_parent(const GameObjectPtr& parent) noexcept
	{
		assert(this != parent.get());

		auto _weak = parent_.lock();
		if (_weak != parent)
		{
			this->on_move_before();

			if (_weak)
			{
				auto it = _weak->children_.begin();
				auto end = _weak->children_.end();

				for (; it != end; ++it)
				{
					if ((*it).get() == this)
					{
						_weak->children_.erase(it);
						break;
					}
				}
			}

			parent_ = parent;
			if (parent)
				parent->children_.push_back(this->downcast_pointer<GameObject>());

			this->on_move_after();
		}
	}

	GameObject*
	GameObject::get_parent() const noexcept
	{
		return parent_.lock().get();
	}

	GameScene*
	GameObject::get_game_scene() noexcept
	{
		auto parent = this->get_parent();
		if (parent)
			return parent->get_game_scene();
		return nullptr;
	}

	const GameScene*
	GameObject::get_game_scene() const noexcept
	{
		auto parent = this->get_parent();
		if (parent)
			return parent->get_game_scene();
		return nullptr;
	}

	void
	GameObject::add_child(GameObjectPtr& entity) noexcept
	{
		assert(entity);
		entity->set_parent(std::dynamic_pointer_cast<GameObject>(this->shared_from_this()));
	}

	void
	GameObject::add_child(GameObjectPtr&& entity) noexcept
	{
		assert(entity);
		entity->set_parent(std::dynamic_pointer_cast<GameObject>(this->shared_from_this()));
	}

	void
	GameObject::remove_child(GameObjectPtr& entity) noexcept
	{
		assert(entity);

		auto it = children_.begin();
		auto end = children_.end();
		for (; it != end; ++it)
		{
			if ((*it) == entity)
			{
				(*it)->set_parent(nullptr);
				break;
			}
		}

		if (it != end)
		{
			children_.erase(it);
		}
	}

	void
	GameObject::cleanup_children() noexcept
	{
		for (auto& it : children_)
			it.reset();

		children_.clear();
	}

	GameObjectPtr
	GameObject::find_child(const std::string& name, bool recuse) noexcept
	{
		for (auto& it : children_)
		{
			if (it->get_name() == name)
			{
				return it;
			}
		}

		if (recuse)
		{
			for (auto& it : children_)
			{
				auto result = it->find_child(name, recuse);
				if (result)
				{
					return result;
				}
			}
		}

		return nullptr;
	}

	std::size_t
	GameObject::get_child_count() const noexcept
	{
		return children_.size();
	}

	GameObjects&
	GameObject::get_children() noexcept
	{
		return children_;
	}

	const GameObjects&
	GameObject::get_children() const noexcept
	{
		return children_;
	}

	void
	GameObject::add_component(const GameComponentPtr& gameComponent) except
	{
		assert(gameComponent);
		assert(gameComponent->gameObject_ == nullptr);

		auto it = std::find(components_.begin(), components_.end(), gameComponent);
		if (it == components_.end())
		{
			gameComponent->_setGameObject(this);
			gameComponent->on_attach();

			if (this->get_active() && gameComponent->get_active())
				gameComponent->on_activate();

			for (auto& component : components_)
				gameComponent->on_attach_component(component);

			for (auto& component : components_)
				component->on_attach_component(gameComponent);

			components_.push_back(gameComponent);
		}
	}

	void
	GameObject::add_component(GameComponentPtr&& component) except
	{
		this->add_component(component);
	}

	void
	GameObject::remove_component(const GameComponentPtr& gameComponent) noexcept
	{
		assert(gameComponent);
		assert(gameComponent->gameObject_ == this);

		auto it = std::find(components_.begin(), components_.end(), gameComponent);
		if (it != components_.end())
		{
			components_.erase(it);

			for (auto& compoent : components_)
				compoent->on_detach_component(gameComponent);

			for (auto& component : components_)
				gameComponent->on_detach_component(component);

			if (this->get_active() && gameComponent->get_active())
				gameComponent->on_deactivate();

			gameComponent->on_detach();
			gameComponent->_setGameObject(nullptr);

			this->remove_component_dispatchs(gameComponent);
		}
	}

	void
	GameObject::cleanup_components() noexcept
	{
		for (auto it = components_.begin(); it != components_.end();)
		{
			auto gameComponent = *it;
			auto nextComponent = components_.erase(it);

			for (auto& compoent : components_)
				compoent->on_detach_component(gameComponent);

			for (auto& component : components_)
				gameComponent->on_detach_component(component);

			if (this->get_active() && (gameComponent)->get_active())
				gameComponent->on_deactivate();

			gameComponent->on_detach();
			gameComponent->_setGameObject(nullptr);

			this->remove_component_dispatchs(gameComponent);

			it = nextComponent;
		}
	}

	GameComponentPtr
	GameObject::get_component(const runtime::Rtti* type) const noexcept
	{
		assert(type);

		for (auto& it : components_)
		{
			if (it->is_a(type))
				return it;
		}

		return nullptr;
	}

	GameComponentPtr
	GameObject::get_component(const runtime::Rtti& type) const noexcept
	{
		return this->get_component(&type);
	}

	void
	GameObject::get_components(const runtime::Rtti* type, GameComponents& components) const noexcept
	{
		assert(type);

		for (auto& it : components_)
		{
			if (it->is_a(type))
				components.push_back(it);
		}
	}

	void
	GameObject::get_components(const runtime::Rtti& type, GameComponents& components) const noexcept
	{
		this->get_components(&type, components);
	}

	GameComponentPtr
	GameObject::get_component_in_children(const runtime::Rtti* type) const noexcept
	{
		assert(type);

		for (auto& it : children_)
		{
			for (auto& component : it->components_)
			{
				if (component->is_a(type))
					return component;
			}

			auto component = it->get_component_in_children(type);
			if (component)
				return component;
		}

		return nullptr;
	}

	GameComponentPtr
	GameObject::get_component_in_children(const runtime::Rtti& type) const noexcept
	{
		return this->get_component_in_children(&type);
	}

	void
	GameObject::get_components_in_children(const runtime::Rtti* type, GameComponents& components) const noexcept
	{
		assert(type);

		for (auto& it : children_)
		{
			for (auto& component : it->components_)
			{
				if (component->is_a(type))
					components.push_back(component);
			}

			it->get_component_in_children(type);
		}
	}

	void
	GameObject::get_components_in_children(const runtime::Rtti& type, GameComponents& components) const noexcept
	{
		return this->get_components_in_children(&type, components);
	}

	const GameComponents&
	GameObject::get_components() const noexcept
	{
		return components_;
	}

	void
	GameObject::add_component_dispatch(GameDispatchTypes type, const GameComponentPtr& component) noexcept
	{
		assert(component);

		if (dispatch_components_.empty())
			dispatch_components_.resize(GameDispatchType::RangeSize_);

		if (std::find(dispatch_components_[type].begin(), dispatch_components_[type].end(), component) != dispatch_components_[type].end())
			return;

		if (this->get_active())
		{
			if (type == GameDispatchType::Frame ||
				type == GameDispatchType::FrameBegin ||
				type == GameDispatchType::FrameEnd)
			{
				if (dispatch_components_[GameDispatchType::Frame].empty() &&
					dispatch_components_[GameDispatchType::FrameBegin].empty() &&
					dispatch_components_[GameDispatchType::FrameEnd].empty())
				{
					GameObjectManager::instance()->_activeObject(this, true);
				}
			}
		}

		dispatch_components_[type].push_back(component);
	}

	void
	GameObject::remove_component_dispatch(GameDispatchTypes type, const GameComponentPtr& component) noexcept
	{
		assert(component);

		if (dispatch_components_.empty())
			return;

		auto it = std::find(dispatch_components_[type].begin(), dispatch_components_[type].end(), component);
		if (it == dispatch_components_[type].end())
			return;

		if (this->get_active())
		{
			if (type == GameDispatchType::Frame ||
				type == GameDispatchType::FrameBegin ||
				type == GameDispatchType::FrameEnd)
			{
				if (dispatch_components_[GameDispatchType::Frame].empty() &&
					dispatch_components_[GameDispatchType::FrameBegin].empty() &&
					dispatch_components_[GameDispatchType::FrameEnd].empty())
				{
					GameObjectManager::instance()->_activeObject(this, false);
				}
			}
		}

		dispatch_components_[type].erase(it);
	}

	void
	GameObject::remove_component_dispatchs(const GameComponentPtr& component) noexcept
	{
		assert(component);

		for (auto& dispatch : dispatch_components_)
		{
			auto it = std::find(dispatch.begin(), dispatch.end(), component);
			if (it != dispatch.end())
				dispatch.erase(it);
		}
	}

	void
	GameObject::destroy() noexcept
	{
		this->cleanup_children();
		this->cleanup_components();
		this->set_parent(nullptr);
	}

	GameObjectPtr
	GameObject::clone() const except
	{
		auto instance = std::make_shared<GameObject>();
		instance->set_parent(parent_.lock());
		instance->set_name(this->get_name());
		instance->set_layer(this->get_layer());

		for (auto& it : components_)
			instance->add_component(it->clone());

		for (auto& it : this->get_children())
			instance->add_child(it->clone());

		return instance;
	}

	void
	GameObject::on_frame_begin() except
	{
		assert(!dispatch_components_.empty());

		auto& components = dispatch_components_[GameDispatchType::FrameBegin];
		for (auto& it : components)
			it->on_frame_begin();
	}

	void
	GameObject::on_frame() except
	{
		assert(!dispatch_components_.empty());

		auto& components = dispatch_components_[GameDispatchType::Frame];
		for (auto& it : components)
			it->on_frame();
	}

	void
	GameObject::on_frame_end() except
	{
		assert(!dispatch_components_.empty());

		auto& components = dispatch_components_[GameDispatchType::FrameEnd];
		for (auto& it : components)
			it->on_frame_end();
	}

	void
	GameObject::on_activate() except
	{
		for (auto& it : components_)
		{
			if (it->get_active())
				it->on_activate();
		}

		if (!dispatch_components_.empty())
		{
			if (!dispatch_components_[GameDispatchType::Frame].empty() ||
				!dispatch_components_[GameDispatchType::FrameBegin].empty() ||
				!dispatch_components_[GameDispatchType::FrameEnd].empty())
			{
				GameObjectManager::instance()->_activeObject(this, true);
			}
		}
	}

	void
	GameObject::on_deactivate() noexcept
	{
		if (!dispatch_components_.empty())
		{
			if (!dispatch_components_[GameDispatchType::Frame].empty() ||
				!dispatch_components_[GameDispatchType::FrameBegin].empty() ||
				!dispatch_components_[GameDispatchType::FrameEnd].empty())
			{
				GameObjectManager::instance()->_activeObject(this, false);
			}
		}

		for (auto& it : components_)
		{
			if (it->get_active())
				it->on_deactivate();
		}
	}

	void
	GameObject::on_layer_change_before() except
	{
		if (this->get_active())
		{
			for (auto& it : components_)
			{
				if (it->get_active())
					it->on_layer_change_before();
			}
		}
	}

	void
	GameObject::on_layer_change_after() except
	{
		if (this->get_active())
		{
			for (auto& it : components_)
			{
				if (it->get_active())
					it->on_layer_change_after();
			}
		}
	}

	void
	GameObject::on_move_before() except
	{
		if (!this->get_active())
			return;

		if (!dispatch_components_.empty())
		{
			auto& components = dispatch_components_[GameDispatchType::MoveBefore];
			for (auto& it : components)
			{
				if (it->get_active())
					it->on_move_before();
			}
		}

		for (auto& it : children_)
		{
			if (it->get_active())
				it->on_move_before();
		}
	}

	void
	GameObject::on_move_after() except
	{
		if (!this->get_active())
			return;

		if (!dispatch_components_.empty())
		{
			auto& components = dispatch_components_[GameDispatchType::MoveAfter];
			for (auto& it : components)
			{
				if (it->get_active())
					it->on_move_after();
			}
		}

		for (auto& it : children_)
		{
			if (it->get_active())
				it->on_move_after();
		}
	}

	void
	GameObject::on_gui() except
	{
		assert(!dispatch_components_.empty());

		auto& components = dispatch_components_[GameDispatchType::Gui];
		for (auto& it : components)
			it->on_gui();
	}
}