#ifndef OCTOON_GAME_OBJECT_H_
#define OCTOON_GAME_OBJECT_H_

#include <octoon/game_types.h>
#include <octoon/runtime/any.h>
#include <octoon/runtime/sigslot.h>
#include <octoon/io/iarchive.h>
#include <unordered_map>
#include <functional>

namespace octoon
{
	class OCTOON_EXPORT GameObject : public runtime::RttiInterface
	{
		OctoonDeclareSubClass(GameObject, runtime::RttiInterface)
	public:
		GameObject() noexcept;
		GameObject(std::string&& name) noexcept;
		GameObject(const std::string& name) noexcept;
		GameObject(io::archivebuf& reader) except;
		virtual ~GameObject() noexcept;

		void setName(const std::string& name) noexcept;
		void setName(std::string&& name) noexcept;
		const std::string& getName() const noexcept;

		void setActive(bool active) except;
		void setActiveUpwards(bool active) except;
		void setActiveDownwards(bool active) except;
		bool getActive() const noexcept;

		void setLayer(std::uint8_t layer) noexcept;
		std::uint8_t getLayer() const noexcept;

		std::size_t id() const noexcept;

		void setParent(const GameObjectPtr& parent) noexcept;
		GameObject* getParent() const noexcept;

		void addChild(GameObjects&& child) noexcept;
		void addChild(GameObjectPtr&& child) noexcept;
		void addChild(const GameObjects& child) noexcept;
		void addChild(const GameObjectPtr& child) noexcept;
		void removeChild(const GameObjectPtr& child) noexcept;
		void cleanupChildren() noexcept;
		GameObjectPtr findChild(const std::string& name, bool recurse = true) noexcept;

		std::size_t getChildCount() const noexcept;
		GameObjects& getChildren() noexcept;
		const GameObjects& getChildren() const noexcept;

		template<typename T, typename ...Args, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		std::shared_ptr<T> addComponent(Args&&... args) noexcept(false) { auto t = std::make_shared<T>(std::forward<Args>(args)...); this->addComponent(t); return t; }
		void addComponent(const GameComponentPtr& component) except;
		void addComponent(GameComponentPtr&& component) except;
		void addComponent(GameComponents&& component) except;

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

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		void removeComponent() noexcept { this->removeComponent(T::RTTI); }
		void removeComponent(const runtime::Rtti* type) noexcept;
		void removeComponent(const runtime::Rtti& type) noexcept;
		void removeComponent(const GameComponentPtr& component) noexcept;
		void cleanupComponents() noexcept;

		void destroy() noexcept;

		void sendMessage(const std::string& event, const runtime::any& data = nullptr) noexcept;
		void sendMessageUpwards(const std::string& event, const runtime::any& data = nullptr) noexcept;
		void sendMessageDownwards(const std::string& event, const runtime::any& data = nullptr) noexcept;
		void addMessageListener(const std::string& event, std::function<void(const runtime::any&)> listener) noexcept;
		void removeMessageListener(const std::string& event, std::function<void(const runtime::any&)> listener) noexcept;

		virtual GameScene* getGameScene() noexcept;
		virtual const GameScene* getGameScene() const noexcept;

		virtual void load(const io::archivebuf& reader) except;
		virtual void save(io::archivebuf& write) except;

		GameObjectPtr clone() const except;

	public:
		static GameObjectPtr find(const char* name) noexcept;
		static GameObjectPtr find(const std::string& name) noexcept;

		static const GameObjectRaws& instances() noexcept;

	private:
		friend class GameComponent;
		void addComponentDispatch(GameDispatchTypes type, GameComponent* component) noexcept;
		void removeComponentDispatch(GameDispatchTypes type, const GameComponent* component) noexcept;
		void removeComponentDispatchs(const GameComponent* component) noexcept;

	private:
		friend class GameObjectManager;
		friend class TransformComponent;

		void onActivate() except;
		void onDeactivate() noexcept;

		void onMoveBefore() except;
		void onMoveAfter() except;

		void onLayerChangeBefore() except;
		void onLayerChangeAfter() except;

		void onGui() except;

		void onFixedUpdate() except;
		void onUpdate() except;
		void onLateUpdate() except;

	private:
		GameObject(const GameObject& copy) noexcept = delete;
		GameObject& operator=(const GameObject& copy) noexcept = delete;

	private:
		bool active_;

		std::uint8_t layer_;
		std::size_t instance_id_;

		std::string name_;

		GameObjects children_;
		GameObjectWeakPtr parent_;

		GameComponents components_;
		std::vector<GameComponentRaws> dispatchComponents_;
		std::unordered_map<std::string, runtime::signal<void(const runtime::any&)>> dispatchEvents_;
	};
}

#endif