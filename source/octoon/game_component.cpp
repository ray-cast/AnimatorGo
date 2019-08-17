#include <octoon/game_component.h>
#include <octoon/runtime/rtti_factory.h>

namespace octoon
{
	OctoonImplementSubInterface(GameComponent, runtime::RttiInterface, "Component")

	GameComponent::GameComponent() noexcept
		: active_(true)
		, gameObject_(nullptr)
	{
	}

	GameComponent::~GameComponent() noexcept
	{
	}

	GameComponentPtr
	GameComponent::getComponent(const runtime::Rtti* type) const noexcept
	{
		assert(this->rtti() != type);
		return gameObject_->getComponent(type);
	}

	GameComponentPtr
	GameComponent::getComponent(const runtime::Rtti& type) const noexcept
	{
		assert(this->rtti() != &type);
		return gameObject_->getComponent(type);
	}

	void
	GameComponent::getComponents(const runtime::Rtti* type, GameComponents& components) const noexcept
	{
		assert(this->rtti() != type);
		return gameObject_->getComponents(type, components);
	}

	void
	GameComponent::getComponents(const runtime::Rtti& type, GameComponents& components) const noexcept
	{
		assert(this->rtti() != &type);
		return gameObject_->getComponents(type, components);
	}

	GameComponentPtr
	GameComponent::getComponentInChildren(const runtime::Rtti* type) const noexcept
	{
		assert(this->rtti() != type);
		return gameObject_->getComponentInChildren(type);
	}

	GameComponentPtr
	GameComponent::getComponentInChildren(const runtime::Rtti& type) const noexcept
	{
		assert(this->rtti() != &type);
		return gameObject_->getComponentInChildren(type);
	}

	void
	GameComponent::getComponentsInChildren(const runtime::Rtti* type, GameComponents& components) const noexcept
	{
		assert(this->rtti() != type);
		return gameObject_->getComponentsInChildren(type, components);
	}

	void
	GameComponent::getComponentsInChildren(const runtime::Rtti& type, GameComponents& components) const noexcept
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
	GameComponent::setName(const std::string& name) noexcept
	{
		name_ = name;
	}

	void
	GameComponent::setName(std::string&& name) noexcept
	{
		name_ = std::move(name);
	}

	const std::string&
	GameComponent::getName() const noexcept
	{
		return name_;
	}

	void
	GameComponent::sendMessage(const std::string& event, const runtime::any& data) noexcept
	{
		assert(gameObject_);
		gameObject_->sendMessage(event, data);
	}

	void
	GameComponent::sendMessageUpwards(const std::string& event, const runtime::any& data) noexcept
	{
		assert(gameObject_);
		gameObject_->sendMessageUpwards(event, data);
	}

	void
	GameComponent::sendMessageDownwards(const std::string& event, const runtime::any& data) noexcept
	{
		assert(gameObject_);
		gameObject_->sendMessageDownwards(event, data);
	}

	void
	GameComponent::addMessageListener(const std::string& event, std::function<void(const runtime::any&)> listener) noexcept
	{
		assert(gameObject_);
		gameObject_->addMessageListener(event, listener);
	}

	void
	GameComponent::removeMessageListener(const std::string& event, std::function<void(const runtime::any&)> listener) noexcept
	{
		assert(gameObject_);
		gameObject_->removeMessageListener(event, listener);
	}

	bool
	GameComponent::trySendMessage(const std::string& event, const runtime::any& data) noexcept
	{
		if (gameObject_)
		{
			gameObject_->sendMessage(event, data);
			return true;
		}
		return false;
	}

	bool
	GameComponent::trySendMessageUpwards(const std::string& event, const runtime::any& data) noexcept
	{
		if (gameObject_)
		{
			gameObject_->sendMessageUpwards(event, data);
			return true;
		}
		return false;
	}

	bool
	GameComponent::trySendMessageDownwards(const std::string& event, const runtime::any& data) noexcept
	{
		if (gameObject_)
		{
			gameObject_->sendMessageDownwards(event, data);
			return true;
		}
		return false;
	}

	bool
	GameComponent::tryAddMessageListener(const std::string& event, std::function<void(const runtime::any&)> listener) noexcept
	{
		if (gameObject_)
		{
			gameObject_->addMessageListener(event, listener);
			return true;
		}
		return false;
	}

	bool
	GameComponent::tryRemoveMessageListener(const std::string& event, std::function<void(const runtime::any&)> listener) noexcept
	{
		if (gameObject_)
		{
			gameObject_->removeMessageListener(event, listener);
			return true;
		}
		return false;
	}

	void
	GameComponent::load(const io::archivebuf& reader) except
	{
		bool active = false;
		reader["name"] >> name_;
		reader["active"] >> active;

		this->setActive(active);
	}

	void
	GameComponent::save(io::archivebuf& write) except
	{
		write["name"] << name_;
		write["active"] << active_;
	}

	GameComponentPtr
	GameComponent::instantiate(const GameComponent* component) except
	{
		assert(component);
		return octoon::runtime::rtti::instantiate(component);
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