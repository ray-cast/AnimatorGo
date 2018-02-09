#include <octoon/game_component.h>

namespace octoon
{
	OctoonImplementSubInterface(GameComponent, RttiInterface, "Component")

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

	const GameComponents&
	GameComponent::getComponents() const noexcept
	{
		return gameObject_->getComponents();
	}

	void
	GameComponent::addComponentDispatch(GameDispatchType type, const GameComponentPtr& component) noexcept
	{
		assert(gameObject_ && component);
		gameObject_->addComponentDispatch(type, component);
	}

	void
	GameComponent::removeComponentDispatch(GameDispatchType type, const GameComponentPtr& component) noexcept
	{
		assert(gameObject_ && component);
		gameObject_->removeComponentDispatch(type, component);
	}

	void
	GameComponent::addComponentDispatch(GameDispatchType type, GameComponent* component) noexcept
	{
		assert(gameObject_ && component);
		gameObject_->addComponentDispatch(type, component->cast_pointer<GameComponent>());
	}

	void
	GameComponent::removeComponentDispatch(GameDispatchType type, GameComponent* component) noexcept
	{
		assert(gameObject_ && component);
		gameObject_->removeComponentDispatch(type, component->cast_pointer<GameComponent>());
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
}