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
		GameObjectManager::instance()->_instanceObject(this, instanceID_);

		this->addComponent(std::make_shared<TransformComponent>());
	}

	GameObject::~GameObject() noexcept
	{
		this->cleanupChildren();
		this->cleanupComponents();

		GameObjectManager::instance()->_unsetObject(this);
	}

	void
	GameObject::setName(const std::string& name) noexcept
	{
		name_ = name;
	}

	void
	GameObject::setName(std::string&& name) noexcept
	{
		name_ = std::move(name);
	}

	const std::string&
	GameObject::getName() const noexcept
	{
		return name_;
	}

	void
	GameObject::setActive(bool active) except
	{
		if (active_ != active)
		{
			if (active)
				this->_onActivate();
			else
				this->_onDeactivate();

			active_ = active;
		}
	}

	void
	GameObject::setActiveUpwards(bool active) except
	{
		if (active_ != active)
		{
			if (active)
				this->_onActivate();
			else
				this->_onDeactivate();

			auto parent = this->getParent();
			if (parent)
				parent->setActiveUpwards(active);

			active_ = active;
		}
	}

	void
	GameObject::setActiveDownwards(bool active) except
	{
		if (active_ != active)
		{
			if (active)
				this->_onActivate();
			else
				this->_onDeactivate();

			for (auto& it : children_)
				it->setActiveDownwards(active);

			active_ = active;
		}
	}

	bool
	GameObject::getActive() const noexcept
	{
		return active_;
	}

	void
	GameObject::setLayer(std::uint8_t layer) noexcept
	{
		if (layer_ != layer)
		{
			this->_onLayerChangeBefore();

			layer_ = layer;

			this->_onLayerChangeAfter();
		}
	}

	std::uint8_t
	GameObject::getLayer() const noexcept
	{
		return layer_;
	}

	std::size_t
	GameObject::getInstanceID() const noexcept
	{
		return instanceID_;
	}

	void
	GameObject::setParent(const GameObjectPtr& parent) noexcept
	{
		assert(this != parent.get());

		auto _weak = parent_.lock();
		if (_weak != parent)
		{
			this->_onMoveBefore();

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

			this->_onMoveAfter();
		}
	}

	GameObject*
	GameObject::getParent() const noexcept
	{
		return parent_.lock().get();
	}

	GameScene*
	GameObject::getGameScene() noexcept
	{
		auto parent = this->getParent();
		if (parent)
			return parent->getGameScene();
		return nullptr;
	}

	const GameScene*
	GameObject::getGameScene() const noexcept
	{
		auto parent = this->getParent();
		if (parent)
			return parent->getGameScene();
		return nullptr;
	}

	void
	GameObject::addChild(GameObjectPtr& entity) noexcept
	{
		assert(entity);
		entity->setParent(std::dynamic_pointer_cast<GameObject>(this->shared_from_this()));
	}

	void
	GameObject::removeChild(GameObjectPtr& entity) noexcept
	{
		assert(entity);

		auto it = children_.begin();
		auto end = children_.end();
		for (; it != end; ++it)
		{
			if ((*it) == entity)
			{
				(*it)->setParent(nullptr);
				break;
			}
		}

		if (it != end)
		{
			children_.erase(it);
		}
	}

	void
	GameObject::cleanupChildren() noexcept
	{
		for (auto& it : children_)
			it.reset();

		children_.clear();
	}

	GameObjectPtr
	GameObject::findChild(const std::string& name, bool recuse) noexcept
	{
		for (auto& it : children_)
		{
			if (it->getName() == name)
			{
				return it;
			}
		}

		if (recuse)
		{
			for (auto& it : children_)
			{
				auto result = it->findChild(name, recuse);
				if (result)
				{
					return result;
				}
			}
		}

		return nullptr;
	}

	std::size_t
	GameObject::getChildCount() const noexcept
	{
		return children_.size();
	}

	GameObjects&
	GameObject::getChildren() noexcept
	{
		return children_;
	}

	const GameObjects&
	GameObject::getChildren() const noexcept
	{
		return children_;
	}

	void
	GameObject::addComponent(const GameComponentPtr& gameComponent) except
	{
		assert(gameComponent);
		assert(gameComponent->gameObject_ == nullptr);

		auto it = std::find(components_.begin(), components_.end(), gameComponent);
		if (it == components_.end())
		{
			gameComponent->_setGameObject(this);
			gameComponent->onAttach();

			if (this->getActive() && gameComponent->getActive())
				gameComponent->onActivate();

			for (auto& component : components_)
				gameComponent->onAttachComponent(component);

			for (auto& component : components_)
				component->onAttachComponent(gameComponent);

			components_.push_back(gameComponent);
		}
	}

	void
	GameObject::addComponent(GameComponentPtr&& component) except
	{
		this->addComponent(component);
	}

	void
	GameObject::removeComponent(const GameComponentPtr& gameComponent) noexcept
	{
		assert(gameComponent);
		assert(gameComponent->gameObject_ == this);

		auto it = std::find(components_.begin(), components_.end(), gameComponent);
		if (it != components_.end())
		{
			components_.erase(it);

			for (auto& compoent : components_)
				compoent->onDetachComponent(gameComponent);

			for (auto& component : components_)
				gameComponent->onDetachComponent(component);

			if (this->getActive() && gameComponent->getActive())
				gameComponent->onDeactivate();

			gameComponent->onDetach();
			gameComponent->_setGameObject(nullptr);

			this->removeComponentDispatchs(gameComponent);
		}
	}

	void
	GameObject::cleanupComponents() noexcept
	{
		for (auto it = components_.begin(); it != components_.end();)
		{
			auto gameComponent = *it;
			auto nextComponent = components_.erase(it);

			for (auto& compoent : components_)
				compoent->onDetachComponent(gameComponent);

			for (auto& component : components_)
				gameComponent->onDetachComponent(component);

			if (this->getActive() && (gameComponent)->getActive())
				gameComponent->onDeactivate();

			gameComponent->onDetach();
			gameComponent->_setGameObject(nullptr);

			this->removeComponentDispatchs(gameComponent);

			it = nextComponent;
		}
	}

	GameComponentPtr
	GameObject::getComponent(const runtime::Rtti* type) const noexcept
	{
		assert(type);

		for (auto& it : components_)
		{
			if (it->isA(type))
				return it;
		}

		return nullptr;
	}

	GameComponentPtr
	GameObject::getComponent(const runtime::Rtti& type) const noexcept
	{
		return this->getComponent(&type);
	}

	GameComponentPtr
	GameObject::getComponentInChildren(const runtime::Rtti* type) const noexcept
	{
		assert(type);

		for (auto& it : children_)
		{
			for (auto& component : it->components_)
			{
				if (component->isA(type))
					return component;
			}

			auto component = it->getComponentInChildren(type);
			if (component)
				return component;
		}

		return nullptr;
	}

	GameComponentPtr
	GameObject::getComponentInChildren(const runtime::Rtti& type) const noexcept
	{
		return this->getComponentInChildren(&type);
	}

	void
	GameObject::getComponentsInChildren(const runtime::Rtti* type, GameComponents& components) const noexcept
	{
		assert(type);

		for (auto& it : children_)
		{
			for (auto& component : it->components_)
			{
				if (component->isA(type))
					components.push_back(component);
			}

			it->getComponentInChildren(type);
		}
	}

	void
	GameObject::getComponentsInChildren(const runtime::Rtti& type, GameComponents& components) const noexcept
	{
		return this->getComponentsInChildren(&type, components);
	}

	const GameComponents&
	GameObject::getComponents() const noexcept
	{
		return components_;
	}

	void
	GameObject::addComponentDispatch(GameDispatchType type, const GameComponentPtr& component) noexcept
	{
		assert(component);

		if (dispatchComponents_.empty())
			dispatchComponents_.resize(GameDispatchType::GameDispatchTypeRangeSize);

		if (std::find(dispatchComponents_[type].begin(), dispatchComponents_[type].end(), component) != dispatchComponents_[type].end())
			return;

		if (this->getActive())
		{
			if (type == GameDispatchType::GameDispatchTypeFrame ||
				type == GameDispatchType::GameDispatchTypeFrameBegin ||
				type == GameDispatchType::GameDispatchTypeFrameEnd)
			{
				if (dispatchComponents_[GameDispatchType::GameDispatchTypeFrame].empty() &&
					dispatchComponents_[GameDispatchType::GameDispatchTypeFrameBegin].empty() &&
					dispatchComponents_[GameDispatchType::GameDispatchTypeFrameEnd].empty())
				{
					GameObjectManager::instance()->_activeObject(this, true);
				}
			}
		}

		dispatchComponents_[type].push_back(component);
	}

	void
	GameObject::removeComponentDispatch(GameDispatchType type, const GameComponentPtr& component) noexcept
	{
		assert(component);

		if (dispatchComponents_.empty())
			return;

		auto it = std::find(dispatchComponents_[type].begin(), dispatchComponents_[type].end(), component);
		if (it == dispatchComponents_[type].end())
			return;

		if (this->getActive())
		{
			if (type == GameDispatchType::GameDispatchTypeFrame ||
				type == GameDispatchType::GameDispatchTypeFrameBegin ||
				type == GameDispatchType::GameDispatchTypeFrameEnd)
			{
				if (dispatchComponents_[GameDispatchType::GameDispatchTypeFrame].empty() &&
					dispatchComponents_[GameDispatchType::GameDispatchTypeFrameBegin].empty() &&
					dispatchComponents_[GameDispatchType::GameDispatchTypeFrameEnd].empty())
				{
					GameObjectManager::instance()->_activeObject(this, false);
				}
			}
		}

		dispatchComponents_[type].erase(it);
	}

	void
	GameObject::removeComponentDispatchs(const GameComponentPtr& component) noexcept
	{
		assert(component);

		for (auto& dispatch : dispatchComponents_)
		{
			auto it = std::find(dispatch.begin(), dispatch.end(), component);
			if (it != dispatch.end())
				dispatch.erase(it);
		}
	}

	void
	GameObject::destroy() noexcept
	{
		this->cleanupChildren();
		this->cleanupComponents();
		this->setParent(nullptr);
	}

	GameObjectPtr
	GameObject::clone() const except
	{
		auto instance = std::make_shared<GameObject>();
		instance->setParent(parent_.lock());
		instance->setName(this->getName());
		instance->setLayer(this->getLayer());

		for (auto& it : components_)
			instance->addComponent(it->clone());

		for (auto& it : this->getChildren())
			instance->addChild(it->clone());

		return instance;
	}

	void
	GameObject::_onFrameBegin() except
	{
		assert(!dispatchComponents_.empty());

		auto& components = dispatchComponents_[GameDispatchType::GameDispatchTypeFrameBegin];
		for (auto& it : components)
			it->onFrameBegin();
	}

	void
	GameObject::_onFrame() except
	{
		assert(!dispatchComponents_.empty());

		auto& components = dispatchComponents_[GameDispatchType::GameDispatchTypeFrame];
		for (auto& it : components)
			it->onFrame();
	}

	void
	GameObject::_onFrameEnd() except
	{
		assert(!dispatchComponents_.empty());

		auto& components = dispatchComponents_[GameDispatchType::GameDispatchTypeFrameEnd];
		for (auto& it : components)
			it->onFrameEnd();
	}

	void
	GameObject::_onActivate() except
	{
		for (auto& it : components_)
		{
			if (it->getActive())
				it->onActivate();
		}

		if (!dispatchComponents_.empty())
		{
			if (!dispatchComponents_[GameDispatchType::GameDispatchTypeFrame].empty() ||
				!dispatchComponents_[GameDispatchType::GameDispatchTypeFrameBegin].empty() ||
				!dispatchComponents_[GameDispatchType::GameDispatchTypeFrameEnd].empty())
			{
				GameObjectManager::instance()->_activeObject(this, true);
			}
		}
	}

	void
	GameObject::_onDeactivate() noexcept
	{
		if (!dispatchComponents_.empty())
		{
			if (!dispatchComponents_[GameDispatchType::GameDispatchTypeFrame].empty() ||
				!dispatchComponents_[GameDispatchType::GameDispatchTypeFrameBegin].empty() ||
				!dispatchComponents_[GameDispatchType::GameDispatchTypeFrameEnd].empty())
			{
				GameObjectManager::instance()->_activeObject(this, false);
			}
		}

		for (auto& it : components_)
		{
			if (it->getActive())
				it->onDeactivate();
		}
	}

	void
	GameObject::_onLayerChangeBefore() except
	{
		if (this->getActive())
		{
			for (auto& it : components_)
			{
				if (it->getActive())
					it->onLayerChangeBefore();
			}
		}
	}

	void
	GameObject::_onLayerChangeAfter() except
	{
		if (this->getActive())
		{
			for (auto& it : components_)
			{
				if (it->getActive())
					it->onLayerChangeAfter();
			}
		}
	}

	void
	GameObject::_onMoveBefore() except
	{
		if (!this->getActive())
			return;

		if (!dispatchComponents_.empty())
		{
			auto& components = dispatchComponents_[GameDispatchType::GameDispatchTypeMoveBefore];
			for (auto& it : components)
			{
				if (it->getActive())
					it->onMoveBefore();
			}
		}

		for (auto& it : children_)
		{
			if (it->getActive())
				it->_onMoveBefore();
		}
	}

	void
	GameObject::_onMoveAfter() except
	{
		if (!this->getActive())
			return;

		if (!dispatchComponents_.empty())
		{
			auto& components = dispatchComponents_[GameDispatchType::GameDispatchTypeMoveAfter];
			for (auto& it : components)
			{
				if (it->getActive())
					it->onMoveAfter();
			}
		}

		for (auto& it : children_)
		{
			if (it->getActive())
				it->_onMoveAfter();
		}
	}
}