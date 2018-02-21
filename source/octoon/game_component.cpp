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
	GameComponent::get_component(const runtime::Rtti* type) const noexcept
	{
		assert(this->rtti() != type);
		return gameObject_->get_component(type);
	}

	GameComponentPtr
	GameComponent::get_component(const runtime::Rtti& type) const noexcept
	{
		assert(this->rtti() != &type);
		return gameObject_->get_component(type);
	}

	const GameComponents&
	GameComponent::get_components() const noexcept
	{
		return gameObject_->get_components();
	}

	void
	GameComponent::add_component_dispatch(GameDispatchType type, const GameComponentPtr& component) noexcept
	{
		assert(gameObject_ && component);
		gameObject_->add_component_dispatch(type, component);
	}

	void
	GameComponent::remove_component_dispatch(GameDispatchType type, const GameComponentPtr& component) noexcept
	{
		assert(gameObject_ && component);
		gameObject_->remove_component_dispatch(type, component);
	}

	void
	GameComponent::add_component_dispatch(GameDispatchType type, GameComponent* component) noexcept
	{
		assert(gameObject_ && component);
		gameObject_->add_component_dispatch(type, component->cast_pointer<GameComponent>());
	}

	void
	GameComponent::remove_component_dispatch(GameDispatchType type, GameComponent* component) noexcept
	{
		assert(gameObject_ && component);
		gameObject_->remove_component_dispatch(type, component->cast_pointer<GameComponent>());
	}

	void
	GameComponent::_setGameObject(GameObject* gameobj) noexcept
	{
		gameObject_ = gameobj;
	}

	GameObjectPtr
	GameComponent::get_game_object() const noexcept
	{
		if (gameObject_)
			return gameObject_->cast_pointer<GameObject>();
		return nullptr;
	}

	void
	GameComponent::set_active(bool active) except
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

	bool
	GameComponent::get_active() const noexcept
	{
		return active_;
	}

	void
	GameComponent::set_name(const std::string& name) noexcept
	{
		name_ = name;
	}

	void
	GameComponent::set_name(std::string&& name) noexcept
	{
		name_ = std::move(name);
	}

	const std::string&
	GameComponent::get_name() const noexcept
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
	GameComponent::on_attach() except
	{
	}

	void
	GameComponent::on_detach() noexcept
	{
	}

	void
	GameComponent::on_activate() except
	{
	}

	void
	GameComponent::on_deactivate() noexcept
	{
	}

	void
	GameComponent::on_frame_begin() except
	{
	}

	void
	GameComponent::on_frame() except
	{
	}

	void
	GameComponent::on_frame_end() except
	{
	}

	void
	GameComponent::on_move_before() except
	{
	}

	void
	GameComponent::on_move_after() except
	{
	}

	void
	GameComponent::on_layer_change_before() except
	{
	}

	void
	GameComponent::on_layer_change_after() except
	{
	}

	void
	GameComponent::on_attach_component(const GameComponentPtr& component) except
	{
	}

	void
	GameComponent::on_detach_component(const GameComponentPtr& component) noexcept
	{
	}

	void
	GameComponent::on_gui() except
	{
	}
}