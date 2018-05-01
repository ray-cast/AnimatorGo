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

	GameObjectPtr
	GameComponent::getGameObject() const noexcept
	{
		if (gameObject_)
			return gameObject_->cast_pointer<GameObject>();
		return nullptr;
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
	GameComponent::onFrameBegin() except
	{
	}

	void
	GameComponent::onFrame() except
	{
	}

	void
	GameComponent::onFrameEnd() except
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