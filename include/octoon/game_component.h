#ifndef OCTOON_GAME_COMPONENT_H_
#define OCTOON_GAME_COMPONENT_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT GameComponent : public runtime::RttiInterface
	{
		OctoonDeclareSubInterface(GameComponent, runtime::RttiInterface)
	public:
		GameComponent() noexcept;
		virtual ~GameComponent() noexcept;

		virtual void setActive(bool active) except;
		virtual bool getActive() const noexcept;

		virtual void setName(std::string_view name) noexcept;
		virtual const std::string& getName() const noexcept;

		GameObject* getGameObject() noexcept;
		const GameObject* getGameObject() const noexcept;

		GameScene* getGameScene() noexcept;
		const GameScene* getGameScene() const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		std::shared_ptr<T> getComponent() const noexcept { return std::dynamic_pointer_cast<T>(this->getComponent(T::RTTI)); }
		GameComponentPtr getComponent(const runtime::Rtti* type) const noexcept;
		GameComponentPtr getComponent(const runtime::Rtti& type) const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		void getComponents(GameComponents& components) const noexcept { this->getComponents(T::RTTI, components); }
		void getComponents(const runtime::Rtti* type, GameComponents& components) const noexcept;
		void getComponents(const runtime::Rtti& type, GameComponents& components) const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		std::shared_ptr<T> getComponentInChildren() const noexcept { return std::dynamic_pointer_cast<T>(this->getComponentInChildren(T::RTTI)); }
		GameComponentPtr getComponentInChildren(const runtime::Rtti* type) const noexcept;
		GameComponentPtr getComponentInChildren(const runtime::Rtti& type) const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		void getComponentsInChildren(GameComponents& components) const noexcept { this->getComponentsInChildren(T::RTTI, components); }
		void getComponentsInChildren(const runtime::Rtti* type, GameComponents& components) const noexcept;
		void getComponentsInChildren(const runtime::Rtti& type, GameComponents& components) const noexcept;

		const GameComponents& getComponents() const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		T* getFeature() const noexcept { return dynamic_cast<T*>(this->getFeature(T::RTTI)); }
		GameFeature* getFeature(const runtime::Rtti* rtti) const noexcept;
		GameFeature* getFeature(const runtime::Rtti& rtti) const noexcept;

		void sendMessage(std::string_view event, const std::any& data = nullptr) noexcept;
		void sendMessageUpwards(std::string_view event, const std::any& data = nullptr) noexcept;
		void sendMessageDownwards(std::string_view event, const std::any& data = nullptr) noexcept;
		void addMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept;
		void removeMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameFeature, T>::value>>
		T* tryGetFeature() const noexcept { return dynamic_cast<T*>(this->tryGetFeature(T::RTTI)); }
		GameFeature* tryGetFeature(const runtime::Rtti* rtti) const noexcept;
		GameFeature* tryGetFeature(const runtime::Rtti& rtti) const noexcept;

		bool trySendMessage(std::string_view event, const std::any& data = nullptr) noexcept;
		bool trySendMessageUpwards(std::string_view event, const std::any& data = nullptr) noexcept;
		bool trySendMessageDownwards(std::string_view event, const std::any& data = nullptr) noexcept;
		bool tryAddMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept;
		bool tryRemoveMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept;

		virtual void load(const io::archivebuf& reader) except;
		virtual void save(io::archivebuf& write) except;

		static GameComponentPtr instantiate(const GameComponent* component) except;
		static GameComponentPtr instantiate(const GameComponent& component) except;

		virtual GameComponentPtr clone() const noexcept = 0;

	protected:
		void addComponentDispatch(GameDispatchTypes type) noexcept;
		void removeComponentDispatch(GameDispatchTypes type) noexcept;
		void removeComponentDispatchs() noexcept;

		void tryAddComponentDispatch(GameDispatchTypes type) noexcept;
		void tryRemoveComponentDispatch(GameDispatchTypes type) noexcept;
		void tryRemoveComponentDispatchs() noexcept;

	private:
		virtual void onAttach() except;
		virtual void onDetach() noexcept;

		virtual void onAttachComponent(const GameComponentPtr& component) except;
		virtual void onDetachComponent(const GameComponentPtr& component) noexcept;

		virtual void onActivate() except;
		virtual void onDeactivate() noexcept;

		virtual void onFixedUpdate() except;
		virtual void onUpdate() except;
		virtual void onLateUpdate() except;

		virtual void onMoveBefore() except;
		virtual void onMoveAfter() except;

		virtual void onLayerChangeBefore() except;
		virtual void onLayerChangeAfter() except;

		virtual void onGui() except;

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