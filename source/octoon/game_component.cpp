#include <octoon/game_component.h>

namespace octoon
{
	OctoonImplementSubInterface(GameComponent, RttiInterface, "Component")

	GameComponent::GameComponent() noexcept
		: _active(true)
		, _gameObject(nullptr)
	{
	}

	GameComponent::~GameComponent() noexcept
	{
	}

	GameComponentPtr
	GameComponent::getComponent(const runtime::Rtti* type) const noexcept
	{
		assert(this->rtti() != type);
		return _gameObject->getComponent(type);
	}

	GameComponentPtr
	GameComponent::getComponent(const runtime::Rtti& type) const noexcept
	{
		assert(this->rtti() != &type);
		return _gameObject->getComponent(type);
	}

	const GameComponents&
	GameComponent::getComponents() const noexcept
	{
		return _gameObject->getComponents();
	}

	void
	GameComponent::addComponentDispatch(GameDispatchType type, const GameComponentPtr& component) noexcept
	{
		assert(_gameObject && component);
		_gameObject->addComponentDispatch(type, component);
	}

	void
	GameComponent::removeComponentDispatch(GameDispatchType type, const GameComponentPtr& component) noexcept
	{
		assert(_gameObject && component);
		_gameObject->removeComponentDispatch(type, component);
	}

	void
	GameComponent::addComponentDispatch(GameDispatchType type, GameComponent* component) noexcept
	{
		assert(_gameObject && component);
		_gameObject->addComponentDispatch(type, component->cast_pointer<GameComponent>());
	}

	void
	GameComponent::removeComponentDispatch(GameDispatchType type, GameComponent* component) noexcept
	{
		assert(_gameObject && component);
		_gameObject->removeComponentDispatch(type, component->cast_pointer<GameComponent>());
	}

	void
	GameComponent::_setGameObject(GameObject* gameobj) noexcept
	{
		_gameObject = gameobj;
	}

	GameObjectPtr
	GameComponent::getGameObject() const noexcept
	{
		if (_gameObject)
			return _gameObject->cast_pointer<GameObject>();
		return nullptr;
	}

	void
	GameComponent::setActive(bool active) except
	{
		if (_active != active)
		{
			if (active)
				this->onActivate();
			else
				this->onDeactivate();

			_active = active;
		}
	}

	bool
	GameComponent::getActive() const noexcept
	{
		return _active;
	}

	void
	GameComponent::setName(const std::string& name) noexcept
	{
		_name = name;
	}

	void
	GameComponent::setName(std::string&& name) noexcept
	{
		_name = std::move(name);
	}

	const std::string&
	GameComponent::getName() const noexcept
	{
		return _name;
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