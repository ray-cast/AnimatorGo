#ifndef OCTOON_COMPONENT_H_
#define OCTOON_COMPONENT_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT GameComponent : public runtime::RttiInterface
	{
		OctoonDeclareSubInterface(GameComponent, runtime::RttiInterface)
	public:
		GameComponent() noexcept;
		virtual ~GameComponent() noexcept;

		void set_active(bool active) except;
		bool get_active() const noexcept;

		void set_name(const std::string& name) noexcept;
		void set_name(std::string&& name) noexcept;
		const std::string& get_name() const noexcept;

		GameObjectPtr get_game_object() const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		std::shared_ptr<T> get_component() const noexcept { return std::dynamic_pointer_cast<T>(this->get_component(T::RTTI)); }
		GameComponentPtr get_component(const runtime::Rtti* type) const noexcept;
		GameComponentPtr get_component(const runtime::Rtti& type) const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		void get_components(GameComponents& components) const noexcept { this->get_components(T::RTTI, components); }
		void get_components(const runtime::Rtti* type, GameComponents& components) const noexcept;
		void get_components(const runtime::Rtti& type, GameComponents& components) const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		std::shared_ptr<T> get_component_in_children() const noexcept { return std::dynamic_pointer_cast<T>(this->get_component_in_children(T::RTTI)); }
		GameComponentPtr get_component_in_children(const runtime::Rtti* type) const noexcept;
		GameComponentPtr get_component_in_children(const runtime::Rtti& type) const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		void get_components_in_children(GameComponents& components) const noexcept { this->get_components_in_children(T::RTTI, components); }
		void get_components_in_children(const runtime::Rtti* type, GameComponents& components) const noexcept;
		void get_components_in_children(const runtime::Rtti& type, GameComponents& components) const noexcept;

		const GameComponents& get_components() const noexcept;

		static GameComponentPtr instantiate(const GameComponent* component) except;
		static GameComponentPtr instantiate(const GameComponent& component) except;

		virtual GameComponentPtr clone() const noexcept = 0;

	protected:
		void add_component_dispatch(GameDispatchTypes type, GameComponent* component) noexcept;
		void add_component_dispatch(GameDispatchTypes type, const GameComponentPtr& component) noexcept;

		void remove_component_dispatch(GameDispatchTypes type, GameComponent* component) noexcept;
		void remove_component_dispatch(GameDispatchTypes type, const GameComponentPtr& component) noexcept;

	private:
		virtual void on_attach() except;
		virtual void on_detach() noexcept;

		virtual void on_attach_component(const GameComponentPtr& component) except;
		virtual void on_detach_component(const GameComponentPtr& component) noexcept;

		virtual void on_activate() except;
		virtual void on_deactivate() noexcept;

		virtual void on_frame_begin() except;
		virtual void on_frame() except;
		virtual void on_frame_end() except;

		virtual void on_move_before() except;
		virtual void on_move_after() except;

		virtual void on_layer_change_before() except;
		virtual void on_layer_change_after() except;

		virtual void on_gui() except;

	private:
		friend GameObject;
		void _setGameObject(GameObject* gameobj) noexcept;

	private:
		bool active_;

		std::string name_;

		GameObject* gameObject_;
	};
}

#endif