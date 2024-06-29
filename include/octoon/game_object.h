#ifndef OCTOON_GAME_OBJECT_H_
#define OCTOON_GAME_OBJECT_H_

#include <octoon/game_types.h>
#include <octoon/runtime/sigslot.h>
#include <octoon/runtime/json.h>
#include <octoon/io/iarchive.h>

#include <any>
#include <functional>

namespace octoon
{
	struct ObjectAttributeBits
	{
		enum Flags
		{
			NonSerialized = 1
		};
	};

	typedef std::uint32_t ObjectAttributes;

	class OCTOON_EXPORT GameObject : public Object
	{
		OctoonDeclareSubClass(GameObject, Object)
	public:
		GameObject() noexcept;
		GameObject(std::string_view name) noexcept;
		virtual ~GameObject() noexcept;

		void setAttributes(ObjectAttributes attributes) noexcept;
		ObjectAttributes getAttributes() const noexcept;

		void setName(std::string_view name) noexcept;
		const std::string& getName() const noexcept;

		void setActive(bool active) noexcept(false);
		void setActiveUpwards(bool active) noexcept(false);
		void setActiveDownwards(bool active) noexcept(false);
		bool getActive() const noexcept;

		void setLayer(std::uint8_t layer) noexcept;
		std::uint8_t getLayer() const noexcept;

		void setRaycastEnable(bool enable) noexcept;
		bool getRaycastEnable() const noexcept;

		std::size_t id() const noexcept;

		void setParent(const GameObjectPtr& parent) noexcept;
		GameObjectPtr getParent() const noexcept;

		void addChild(GameObjects&& child) noexcept;
		void addChild(GameObjectPtr&& child) noexcept;
		void addChild(const GameObjects& child) noexcept;
		void addChild(const GameObjectPtr& child) noexcept;
		void removeChild(const GameObjectPtr& child) noexcept;
		void cleanupChildren() noexcept;
		GameObjectPtr findChild(std::string_view name, bool recurse = true) noexcept;
		GameObjectPtr& getChild(std::size_t n) noexcept;
		const GameObjectPtr& getChild(std::size_t n) const noexcept;

		std::size_t getChildCount() const noexcept;
		GameObjects& getChildren() noexcept;
		const GameObjects& getChildren() const noexcept;
		GameObjectPtr findChild(std::string_view name) const noexcept;

		template<typename T, typename ...Args, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		std::shared_ptr<T> addComponent(Args&&... args) noexcept(false) { auto t = std::make_shared<T>(std::forward<Args>(args)...); this->addComponent(t); return t; }
		void addComponent(const GameComponentPtr& component) noexcept(false);
		void addComponent(GameComponentPtr&& component) noexcept(false);
		void addComponent(GameComponents&& component) noexcept(false);

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		std::shared_ptr<T> getComponent() const noexcept { return std::dynamic_pointer_cast<T>(this->getComponent(T::RTTI)); }
		GameComponentPtr getComponent(const Rtti* type) const noexcept;
		GameComponentPtr getComponent(const Rtti& type) const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		void getComponents(GameComponents& components) const noexcept { this->getComponents(T::RTTI, components); }
		void getComponents(const Rtti* type, GameComponents& components) const noexcept;
		void getComponents(const Rtti& type, GameComponents& components) const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		std::shared_ptr<T> getComponentInChildren() const noexcept { return std::dynamic_pointer_cast<T>(this->getComponentInChildren(T::RTTI)); }
		GameComponentPtr getComponentInChildren(const Rtti* type) const noexcept;
		GameComponentPtr getComponentInChildren(const Rtti& type) const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		void getComponentsInChildren(GameComponents& components) const noexcept { this->getComponentsInChildren(T::RTTI, components); }
		void getComponentsInChildren(const Rtti* type, GameComponents& components) const noexcept;
		void getComponentsInChildren(const Rtti& type, GameComponents& components) const noexcept;

		const GameComponents& getComponents() const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		void removeComponent() noexcept { this->removeComponent(T::RTTI); }
		void removeComponent(const Rtti* type) noexcept;
		void removeComponent(const Rtti& type) noexcept;
		void removeComponent(const GameComponentPtr& component) noexcept;
		void cleanupComponents() noexcept;

		void destroy() noexcept;

		void sendMessage(std::string_view event, const std::any& data = std::any()) noexcept;
		void sendMessageUpwards(std::string_view event, const std::any& = std::any()) noexcept;
		void sendMessageDownwards(std::string_view event, const std::any& data = std::any()) noexcept;
		void addMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept;
		void removeMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept;

		virtual GameScene* getGameScene() noexcept;
		virtual const GameScene* getGameScene() const noexcept;

		virtual void load(const nlohmann::json& json) noexcept(false);
		virtual void save(nlohmann::json& json) const noexcept(false);

		GameObjectPtr clone() const noexcept(false);

	public:
		static GameObjectPtr find(std::string_view name) noexcept;

		static const GameObjectRaws& instances() noexcept;

	private:
		friend class GameComponent;
		void addComponentDispatch(GameDispatchTypes type, GameComponent* component) noexcept;
		void removeComponentDispatch(GameDispatchTypes type, const GameComponent* component) noexcept;
		void removeComponentDispatchs(const GameComponent* component) noexcept;

	private:
		friend class GameObjectManager;
		friend class TransformComponent;

		void onActivate() noexcept(false);
		void onDeactivate() noexcept;

		void onMoveBefore() noexcept(false);
		void onMoveAfter() noexcept(false);

		void onLayerChangeBefore() noexcept(false);
		void onLayerChangeAfter() noexcept(false);

		void onGui() noexcept(false);

		void onFixedUpdate() noexcept(false);
		void onUpdate() noexcept(false);
		void onLateUpdate() noexcept(false);

	private:
		GameObject(const GameObject& copy) noexcept = delete;
		GameObject& operator=(const GameObject& copy) noexcept = delete;

	private:
		bool active_;
		bool raycastEnable_;

		ObjectAttributes attributes_;

		std::uint8_t layer_;
		std::size_t instance_id_;

		std::string name_;

		GameObjects children_;
		GameObjectWeakPtr parent_;

		GameComponents components_;
		std::vector<GameComponentRaws> dispatchComponents_;
		std::map<std::string, runtime::signal<void(const std::any&)>, std::less<>> dispatchEvents_;
	};
}

#endif