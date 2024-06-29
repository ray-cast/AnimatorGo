#include <octoon/game_object.h>
#include <octoon/game_object_manager.h>
#include <octoon/game_component.h>
#include <octoon/game_scene_manager.h>
#include <octoon/transform_component.h>
#include <octoon/runtime/guid.h>
#include <octoon/asset_importer.h>
#include <octoon/asset_database.h>

namespace octoon
{
	OctoonImplementSubClass(GameObject, Object, "GameObject")

	GameObject::GameObject() noexcept
		: active_(true)
		, raycastEnable_(true)
		, layer_(0)
		, attributes_(0)
	{
		GameObjectManager::instance()->_instanceObject(this, instance_id_);

		this->addComponent<TransformComponent>();
	}

	GameObject::GameObject(std::string_view name) noexcept
		: GameObject()
	{
		this->setName(name);
	}

	GameObject::~GameObject() noexcept
	{
		this->cleanupChildren();
		this->cleanupComponents();

		GameObjectManager::instance()->_unsetObject(this);
	}

	void
	GameObject::setName(std::string_view name) noexcept
	{
		name_ = name;
	}

	const std::string&
	GameObject::getName() const noexcept
	{
		return name_;
	}

	void
	GameObject::setAttributes(ObjectAttributes attributes) noexcept
	{
		attributes_ = attributes;
	}

	ObjectAttributes
	GameObject::getAttributes() const noexcept
	{
		return attributes_;
	}

	void
	GameObject::setActive(bool active) noexcept(false)
	{
		if (active_ != active)
		{
			if (active)
				this->onActivate();
			else
				this->onDeactivate();

			active_ = active;
		}
	}

	void
	GameObject::setActiveUpwards(bool active) noexcept(false)
	{
		if (active_ != active)
		{
			if (active)
				this->onActivate();
			else
				this->onDeactivate();

			auto parent = this->getParent();
			if (parent)
				parent->setActiveUpwards(active);

			active_ = active;
		}
	}

	void
	GameObject::setActiveDownwards(bool active) noexcept(false)
	{
		if (active_ != active)
		{
			if (active)
				this->onActivate();
			else
				this->onDeactivate();

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
			this->onLayerChangeBefore();

			layer_ = layer;

			this->onLayerChangeAfter();
		}
	}

	std::uint8_t
	GameObject::getLayer() const noexcept
	{
		return layer_;
	}

	void
	GameObject::setRaycastEnable(bool enable) noexcept
	{
		raycastEnable_ = enable;
	}

	bool
	GameObject::getRaycastEnable() const noexcept
	{
		return raycastEnable_;
	}

	std::size_t
	GameObject::id() const noexcept
	{
		return instance_id_;
	}

	void
	GameObject::setParent(const GameObjectPtr& parent) noexcept
	{
		assert(this != parent.get());

		auto _weak = parent_.lock();
		if (_weak != parent)
		{
			this->onMoveBefore();

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

			this->getComponent<TransformComponent>()->updateLocalChildren();
			this->onMoveAfter();
		}
	}

	GameObjectPtr
	GameObject::getParent() const noexcept
	{
		return parent_.lock();
	}

	GameScene*
	GameObject::getGameScene() noexcept
	{
		auto parent = this->getParent();
		if (parent)
			return parent->getGameScene();
		return GameSceneManager::instance()->defaultScene();
	}

	const GameScene*
	GameObject::getGameScene() const noexcept
	{
		auto parent = this->getParent();
		if (parent)
			return parent->getGameScene();
		return GameSceneManager::instance()->defaultScene();
	}

	void
	GameObject::addChild(GameObjects&& child) noexcept
	{
		for (auto& it : child)
			this->addChild(std::move(it));
	}

	void
	GameObject::addChild(GameObjectPtr&& entity) noexcept
	{
		assert(entity);
		entity->setParent(std::dynamic_pointer_cast<GameObject>(this->shared_from_this()));
	}

	void
	GameObject::addChild(const GameObjects& child) noexcept
	{
		for (auto& it : child)
			this->addChild(it);
	}

	void
	GameObject::addChild(const GameObjectPtr& entity) noexcept
	{
		assert(entity);
		entity->setParent(std::dynamic_pointer_cast<GameObject>(this->shared_from_this()));
	}

	void
	GameObject::removeChild(const GameObjectPtr& entity) noexcept
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
	GameObject::findChild(std::string_view name, bool recuse) noexcept
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

	GameObjectPtr&
	GameObject::getChild(std::size_t n) noexcept
	{
		return children_[n];
	}

	const GameObjectPtr&
	GameObject::getChild(std::size_t n) const noexcept
	{
		return children_[n];
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

	GameObjectPtr
	GameObject::findChild(std::string_view name) const noexcept
	{
		for (auto& it : children_)
		{
			if (it->getName() == name)
				return it;
		}

		return nullptr;
	}

	void
	GameObject::addComponent(const GameComponentPtr& gameComponent) noexcept(false)
	{
		assert(gameComponent);
		assert(gameComponent->gameObject_ == nullptr);
		assert(std::find(components_.begin(), components_.end(), gameComponent) == components_.end());

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
	GameObject::addComponent(GameComponentPtr&& component) noexcept(false)
	{
		this->addComponent(component);
	}

	void
	GameObject::addComponent(GameComponents&& components) noexcept(false)
	{
		for (auto& it : components)
			this->addComponent(std::move(it));
	}

	void
	GameObject::removeComponent(const Rtti* type) noexcept
	{
		assert(type);

		auto it = std::find_if(components_.begin(), components_.end(), [type](const GameComponentPtr& it) { return it->isA(type); });
		if (it != components_.end())
		{
			for (auto& compoent : components_)
				compoent->onDetachComponent(*it);

			for (auto& component : components_)
				(*it)->onDetachComponent(component);

			if (this->getActive() && (*it)->getActive())
				(*it)->onDeactivate();

			(*it)->onDetach();
			(*it)->_setGameObject(nullptr);

			components_.erase(it);
		}
	}

	void
	GameObject::removeComponent(const Rtti& type) noexcept
	{
		this->removeComponent(&type);
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

			this->removeComponentDispatchs(gameComponent.get());
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

			this->removeComponentDispatchs(gameComponent.get());

			it = nextComponent;
		}
	}

	GameComponentPtr
	GameObject::getComponent(const Rtti* type) const noexcept
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
	GameObject::getComponent(const Rtti& type) const noexcept
	{
		return this->getComponent(&type);
	}

	void
	GameObject::getComponents(const Rtti* type, GameComponents& components) const noexcept
	{
		assert(type);

		for (auto& it : components_)
		{
			if (it->isA(type))
				components.push_back(it);
		}
	}

	void
	GameObject::getComponents(const Rtti& type, GameComponents& components) const noexcept
	{
		this->getComponents(&type, components);
	}

	GameComponentPtr
	GameObject::getComponentInChildren(const Rtti* type) const noexcept
	{
		assert(type);

		for (auto& component : this->components_)
		{
			if (component->isA(type))
				return component;
		}

		for (auto& it : children_)
		{
			auto component = it->getComponentInChildren(type);
			if (component)
				return component;
		}

		return nullptr;
	}

	GameComponentPtr
	GameObject::getComponentInChildren(const Rtti& type) const noexcept
	{
		return this->getComponentInChildren(&type);
	}

	void
	GameObject::getComponentsInChildren(const Rtti* type, GameComponents& components) const noexcept
	{
		assert(type);

		for (auto& component : this->components_)
		{
			if (component->isA(type))
				components.push_back(component);
		}

		for (auto& child : children_)
		{
			for (auto& component : child->components_)
			{
				if (component->isA(type))
					components.push_back(component);
			}

			child->getComponentsInChildren(type, components);
		}
	}

	void
	GameObject::getComponentsInChildren(const Rtti& type, GameComponents& components) const noexcept
	{
		return this->getComponentsInChildren(&type, components);
	}

	const GameComponents&
	GameObject::getComponents() const noexcept
	{
		return components_;
	}

	void
	GameObject::addComponentDispatch(GameDispatchTypes type, GameComponent* component) noexcept
	{
		assert(component);

		if (dispatchComponents_.empty())
			dispatchComponents_.resize(GameDispatchType::RangeSize_);

		if (std::find(dispatchComponents_[type].begin(), dispatchComponents_[type].end(), component) != dispatchComponents_[type].end())
			return;

		if (this->getActive())
		{
			if (type == GameDispatchType::Frame ||
				type == GameDispatchType::FixedUpdate ||
				type == GameDispatchType::LateUpdate ||
				type == GameDispatchType::Gui)
			{
				if (dispatchComponents_[GameDispatchType::Frame].empty() &&
					dispatchComponents_[GameDispatchType::FixedUpdate].empty() &&
					dispatchComponents_[GameDispatchType::LateUpdate].empty() &&
					dispatchComponents_[GameDispatchType::Gui].empty())
				{
					GameObjectManager::instance()->_activeObject(this, true);
				}
			}
		}

		dispatchComponents_[type].push_back(component);
	}

	void
	GameObject::removeComponentDispatch(GameDispatchTypes type, const GameComponent* component) noexcept
	{
		assert(component);

		if (dispatchComponents_.empty())
			return;

		auto it = std::find(dispatchComponents_[type].begin(), dispatchComponents_[type].end(), component);
		if (it == dispatchComponents_[type].end())
			return;

		dispatchComponents_[type].erase(it);

		if (this->getActive())
		{
			if (type == GameDispatchType::Frame ||
				type == GameDispatchType::FixedUpdate ||
				type == GameDispatchType::LateUpdate ||
				type == GameDispatchType::Gui)
			{
				if (dispatchComponents_[GameDispatchType::Frame].empty() &&
					dispatchComponents_[GameDispatchType::FixedUpdate].empty() &&
					dispatchComponents_[GameDispatchType::LateUpdate].empty() &&
					dispatchComponents_[GameDispatchType::Gui].empty())
				{
					GameObjectManager::instance()->_activeObject(this, false);
				}
			}
		}
	}

	void
	GameObject::removeComponentDispatchs(const GameComponent* component) noexcept
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

	void
	GameObject::sendMessage(std::string_view event, const std::any& data) noexcept
	{
		auto it = dispatchEvents_.find(event);
		if (it != dispatchEvents_.end())
			(*it).second.call_all_slots(data);
	}

	void
	GameObject::sendMessageUpwards(std::string_view event, const std::any& data) noexcept
	{
		this->sendMessage(event, data);

		auto parent = this->getParent();
		if (parent)
			parent->sendMessageUpwards(event, data);
	}

	void
	GameObject::sendMessageDownwards(std::string_view event, const std::any& data) noexcept
	{
		this->sendMessage(event, data);

		for (auto& it : children_)
			it->sendMessageDownwards(event, data);
	}

	void
	GameObject::addMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept
	{
		auto it = dispatchEvents_.find(event);
		if (it != dispatchEvents_.end())
			(*it).second.connect(listener);
		else
			dispatchEvents_[std::string(event)].connect(listener);
	}

	void
	GameObject::removeMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept
	{
		auto it = dispatchEvents_.find(event);
		if (it != dispatchEvents_.end())
			(*it).second.disconnect(listener);
	}

	void
	GameObject::load(const nlohmann::json& json) noexcept(false)
	{
		if (json.contains("name"))
			this->setName(json["name"].get<std::string>());
		
		if (json.contains("active"))
			this->setActive(json["active"].get<bool>());
		
		if (json.contains("layer"))
			this->setLayer(json["layer"].get<std::uint8_t>());

		if (json.contains("components"))
		{
			auto& components = json["components"];

			for (auto item = components.begin(); item != components.end(); ++item)
			{
				if (item.key() == TransformComponent::getRtti()->type_name())
				{
					auto transform = this->getComponent<TransformComponent>();
					transform->load(item.value());
				}
				else
				{
					for (auto& it : item.value())
					{
						auto component = RttiFactory::instance()->make_shared<GameComponent>(item.key());
						this->addComponent(std::move(component));
						component->load(it);
					}
				}
			}
		}

		if (json.contains("children"))
		{
			for (auto& it : json["children"])
			{
				auto object = std::make_shared<GameObject>();
				object->load(it);

				this->addChild(std::move(object));
			}
		}
	}

	void
	GameObject::save(nlohmann::json& json) const noexcept(false)
	{
		json["name"] = name_;
		json["active"] = active_;
		json["layer"] = layer_;
		
		for (auto& it : components_)
		{
			if (it->getAttributes() & ObjectAttributeBits::NonSerialized)
				continue;
			
			nlohmann::json componentJson;
			it->save(componentJson);

			json["components"][it->type_name()].push_back(std::move(componentJson));
		}

		for (auto& it : children_)
		{
			if (it->getAttributes() & ObjectAttributeBits::NonSerialized)
				continue;

			nlohmann::json childrenJson;
			it->save(childrenJson);
			json["children"].push_back(childrenJson);
		}
	}

	GameObjectPtr
	GameObject::clone() const noexcept(false)
	{
		auto instance = std::make_shared<GameObject>();
		instance->setName(this->getName());
		instance->setLayer(this->getLayer());

		for (auto& it : components_)
		{
			auto component = it->clone();
			instance->addComponent(component);
		}

		for (auto& it : children_)
		{
			auto child = it->clone();
			instance->addChild(child);
		}

		return instance;
	}

	GameObjectPtr
	GameObject::find(std::string_view name) noexcept
	{
		return GameObjectManager::instance()->find(name);
	}

	const GameObjectRaws&
	GameObject::instances() noexcept
	{
		return GameObjectManager::instance()->instances();
	}

	void
	GameObject::onFixedUpdate() noexcept(false)
	{
		assert(!dispatchComponents_.empty());

		auto& components = dispatchComponents_[GameDispatchType::FixedUpdate];
		for (auto& it : components)
			it->onFixedUpdate();
	}

	void
	GameObject::onUpdate() noexcept(false)
	{
		assert(!dispatchComponents_.empty());

		auto& components = dispatchComponents_[GameDispatchType::Frame];
		for (auto& it : components)
			it->onUpdate();
	}

	void
	GameObject::onLateUpdate() noexcept(false)
	{
		assert(!dispatchComponents_.empty());

		auto& components = dispatchComponents_[GameDispatchType::LateUpdate];
		for (auto& it : components)
			it->onLateUpdate();
	}

	void
	GameObject::onActivate() noexcept(false)
	{
		for (auto& it : components_)
		{
			if (it->getActive())
				it->onActivate();
		}

		if (!dispatchComponents_.empty())
		{
			if (!dispatchComponents_[GameDispatchType::Frame].empty() ||
				!dispatchComponents_[GameDispatchType::FixedUpdate].empty() ||
				!dispatchComponents_[GameDispatchType::LateUpdate].empty() ||
				!dispatchComponents_[GameDispatchType::Gui].empty())
			{
				GameObjectManager::instance()->_activeObject(this, true);
			}
		}
	}

	void
	GameObject::onDeactivate() noexcept
	{
		if (!dispatchComponents_.empty())
		{
			if (!dispatchComponents_[GameDispatchType::Frame].empty() ||
				!dispatchComponents_[GameDispatchType::FixedUpdate].empty() ||
				!dispatchComponents_[GameDispatchType::LateUpdate].empty()||
				!dispatchComponents_[GameDispatchType::Gui].empty())
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
	GameObject::onLayerChangeBefore() noexcept(false)
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
	GameObject::onLayerChangeAfter() noexcept(false)
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
	GameObject::onMoveBefore() noexcept(false)
	{
		if (!this->getActive())
			return;

		if (!dispatchComponents_.empty())
		{
			auto& components = dispatchComponents_[GameDispatchType::MoveBefore];
			for (auto& it : components)
			{
				if (it->getActive())
					it->onMoveBefore();
			}
		}

		for (auto& it : children_)
		{
			if (it->getActive())
				it->onMoveBefore();
		}
	}

	void
	GameObject::onMoveAfter() noexcept(false)
	{
		if (!this->getActive())
			return;

		if (!dispatchComponents_.empty())
		{
			auto& components = dispatchComponents_[GameDispatchType::MoveAfter];
			for (auto& it : components)
			{
				if (it->getActive())
					it->onMoveAfter();
			}
		}

		for (auto& it : children_)
		{
			if (it->getActive())
				it->onMoveAfter();
		}
	}

	void
	GameObject::onGui() noexcept(false)
	{
		assert(!dispatchComponents_.empty());

		auto& components = dispatchComponents_[GameDispatchType::Gui];
		for (auto& it : components)
			it->onGui();
	}
}