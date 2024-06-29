#include <octoon/game_component.h>
#include <octoon/game_scene.h>
#include <octoon/runtime/rtti_factory.h>

namespace octoon
{
	OctoonImplementSubInterface(GameComponent, Object, "GameComponent")

	GameComponent::GameComponent() noexcept
		: active_(true)
		, gameObject_(nullptr)
		, attributes_(0)
	{
	}

	GameComponent::~GameComponent() noexcept
	{
	}

	GameComponentPtr
	GameComponent::getComponent(const Rtti* type) const noexcept
	{
		assert(this->rtti() != type);
		return gameObject_->getComponent(type);
	}

	GameComponentPtr
	GameComponent::getComponent(const Rtti& type) const noexcept
	{
		assert(this->rtti() != &type);
		return gameObject_->getComponent(type);
	}

	void
	GameComponent::getComponents(const Rtti* type, GameComponents& components) const noexcept
	{
		assert(this->rtti() != type);
		return gameObject_->getComponents(type, components);
	}

	void
	GameComponent::getComponents(const Rtti& type, GameComponents& components) const noexcept
	{
		assert(this->rtti() != &type);
		return gameObject_->getComponents(type, components);
	}

	GameComponentPtr
	GameComponent::getComponentInChildren(const Rtti* type) const noexcept
	{
		assert(this->rtti() != type);
		return gameObject_->getComponentInChildren(type);
	}

	GameComponentPtr
	GameComponent::getComponentInChildren(const Rtti& type) const noexcept
	{
		assert(this->rtti() != &type);
		return gameObject_->getComponentInChildren(type);
	}

	void
	GameComponent::getComponentsInChildren(const Rtti* type, GameComponents& components) const noexcept
	{
		assert(this->rtti() != type);
		return gameObject_->getComponentsInChildren(type, components);
	}

	void
	GameComponent::getComponentsInChildren(const Rtti& type, GameComponents& components) const noexcept
	{
		assert(this->rtti() != &type);
		return gameObject_->getComponentsInChildren(type, components);
	}

	const GameComponents&
	GameComponent::getComponents() const noexcept
	{
		return gameObject_->getComponents();
	}

	void
	GameComponent::addComponentDispatch(GameDispatchTypes type) noexcept
	{
		assert(gameObject_);
		gameObject_->addComponentDispatch(type, this);
	}

	void
	GameComponent::removeComponentDispatch(GameDispatchTypes type) noexcept
	{
		assert(gameObject_);
		gameObject_->removeComponentDispatch(type, this);
	}

	void
	GameComponent::removeComponentDispatchs() noexcept
	{
		assert(gameObject_);
		gameObject_->removeComponentDispatchs(this);
	}

	void
	GameComponent::tryAddComponentDispatch(GameDispatchTypes type) noexcept
	{
		if (gameObject_)
			gameObject_->addComponentDispatch(type, this);
	}

	void
	GameComponent::tryRemoveComponentDispatch(GameDispatchTypes type) noexcept
	{
		if (gameObject_)
			gameObject_->removeComponentDispatch(type, this);
	}

	void
	GameComponent::tryRemoveComponentDispatchs() noexcept
	{
		if (gameObject_)
			gameObject_->removeComponentDispatchs(this);
	}

	void
	GameComponent::setAttributes(ObjectAttributes attributes) noexcept
	{
		attributes_ = attributes;
	}

	ObjectAttributes
	GameComponent::getAttributes() const noexcept
	{
		return attributes_;
	}

	void
	GameComponent::_setGameObject(GameObject* gameobj) noexcept
	{
		gameObject_ = gameobj;
	}

	GameObject*
	GameComponent::getGameObject() noexcept
	{
		return gameObject_;
	}

	const GameObject*
	GameComponent::getGameObject() const noexcept
	{
		return gameObject_;
	}

	GameScene*
	GameComponent::getGameScene() noexcept
	{
		return gameObject_ ? gameObject_->getGameScene() : nullptr;
	}

	const GameScene*
	GameComponent::getGameScene() const noexcept
	{
		return gameObject_ ? gameObject_->getGameScene() : nullptr;
	}

	void
	GameComponent::setActive(bool active) except
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

	bool
	GameComponent::getActive() const noexcept
	{
		return active_;
	}

	void
	GameComponent::setName(std::string_view name) noexcept
	{
		name_ = name;
	}

	const std::string&
	GameComponent::getName() const noexcept
	{
		return name_;
	}

	GameFeature*
	GameComponent::getFeature(const Rtti* rtti) const noexcept
	{
		assert(this->getGameScene());
		return this->getGameScene()->getFeature(rtti);
	}
	
	GameFeature*
	GameComponent::getFeature(const Rtti& rtti) const noexcept
	{
		assert(this->getGameScene());
		return this->getGameScene()->getFeature(rtti);
	}

	void
	GameComponent::sendMessage(std::string_view event, const std::any& data) noexcept
	{
		assert(gameObject_);
		gameObject_->sendMessage(event, data);
	}

	void
	GameComponent::sendMessageUpwards(std::string_view event, const std::any& data) noexcept
	{
		assert(gameObject_);
		gameObject_->sendMessageUpwards(event, data);
	}

	void
	GameComponent::sendMessageDownwards(std::string_view event, const std::any& data) noexcept
	{
		assert(gameObject_);
		gameObject_->sendMessageDownwards(event, data);
	}

	void
	GameComponent::addMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept
	{
		assert(gameObject_);
		gameObject_->addMessageListener(event, listener);
	}

	void
	GameComponent::removeMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept
	{
		assert(gameObject_);
		gameObject_->removeMessageListener(event, listener);
	}

	GameFeature*
	GameComponent::tryGetFeature(const Rtti* rtti) const noexcept
	{
		if (!this->getGameScene())
			return nullptr;
		return this->getGameScene()->getFeature(rtti);
	}
	
	GameFeature*
	GameComponent::tryGetFeature(const Rtti& rtti) const noexcept
	{
		if (!this->getGameScene())
			return nullptr;
		return this->getGameScene()->getFeature(rtti);
	}

	bool
	GameComponent::trySendMessage(std::string_view event, const std::any& data) noexcept
	{
		if (gameObject_)
		{
			gameObject_->sendMessage(event, data);
			return true;
		}
		return false;
	}

	bool
	GameComponent::trySendMessageUpwards(std::string_view event, const std::any& data) noexcept
	{
		if (gameObject_)
		{
			gameObject_->sendMessageUpwards(event, data);
			return true;
		}
		return false;
	}

	bool
	GameComponent::trySendMessageDownwards(std::string_view event, const std::any& data) noexcept
	{
		if (gameObject_)
		{
			gameObject_->sendMessageDownwards(event, data);
			return true;
		}
		return false;
	}

	bool
	GameComponent::tryAddMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept
	{
		if (gameObject_)
		{
			gameObject_->addMessageListener(event, listener);
			return true;
		}
		return false;
	}

	bool
	GameComponent::tryRemoveMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept
	{
		if (gameObject_)
		{
			gameObject_->removeMessageListener(event, listener);
			return true;
		}
		return false;
	}

	void
	GameComponent::load(const nlohmann::json& json) noexcept(false)
	{
		if (json.contains("active"))
			setActive(json["active"].get<bool>());
		if (json.contains("name"))
			setName(json["name"].get<std::string>());
	}

	void
	GameComponent::save(nlohmann::json& json) const noexcept(false)
	{
		if (!name_.empty())
			json["name"] = name_;
		if (!active_)
			json["active"] = active_;
	}

	GameComponentPtr
	GameComponent::instantiate(const GameComponent* component) except
	{
		assert(component);
		return octoon::instantiate(component);
	}

	GameComponentPtr
	GameComponent::instantiate(const GameComponent& component) except
	{
		return instantiate(&component);
	}

	void
	GameComponent::onAttach() except
	{
	}

	void
	GameComponent::onDetach() noexcept
	{
	}

	void
	GameComponent::onActivate() except
	{
	}

	void
	GameComponent::onDeactivate() noexcept
	{
	}

	void
	GameComponent::onFixedUpdate() except
	{
	}

	void
	GameComponent::onUpdate() except
	{
	}

	void
	GameComponent::onLateUpdate() except
	{
	}

	void
	GameComponent::onMoveBefore() except
	{
	}

	void
	GameComponent::onMoveAfter() except
	{
	}

	void
	GameComponent::onLayerChangeBefore() except
	{
	}

	void
	GameComponent::onLayerChangeAfter() except
	{
	}

	void
	GameComponent::onAttachComponent(const GameComponentPtr& component) except
	{
	}

	void
	GameComponent::onDetachComponent(const GameComponentPtr& component) noexcept
	{
	}

	void
	GameComponent::onGui() except
	{
	}
}