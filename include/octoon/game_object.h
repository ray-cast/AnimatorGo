#ifndef OCTOON_GAME_OBJECT_H_
#define OCTOON_GAME_OBJECT_H_

#include <octoon/game_types.h>

namespace octoon
{
	class OCTOON_EXPORT GameObject : public runtime::RttiInterface
	{
		OctoonDeclareSubClass(GameObject, runtime::RttiInterface)
	public:
		GameObject() noexcept;
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

		void addChild(GameObjectPtr& child) noexcept;
		void addChild(GameObjectPtr&& child) noexcept;
		void removeChild(GameObjectPtr& child) noexcept;
		void cleanupChildren() noexcept;
		GameObjectPtr findChild(const std::string& name, bool recurse = true) noexcept;

		std::size_t getChildCount() const noexcept;
		GameObjects& getChildren() noexcept;
		const GameObjects& getChildren() const noexcept;

		template<typename T, typename ...Args, typename = std::enable_if_t<std::is_base_of<GameComponent, T>::value>>
		void addComponent(Args&&... args) noexcept(false) { this->addComponent(std::make_shared<T>(std::forward<Args>(args)...)); }
		void addComponent(const GameComponentPtr& component) except;
		void addComponent(GameComponentPtr&& component) except;

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

		virtual GameScene* getGameScene() noexcept;
		virtual const GameScene* getGameScene() const noexcept;

		GameObjectPtr clone() const except;

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

		void onFrameBegin() except;
		void onFrame() except;
		void onFrameEnd() except;

		void onMoveBefore() except;
		void onMoveAfter() except;

		void onLayerChangeBefore() except;
		void onLayerChangeAfter() except;

		void onGui() except;

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
		std::vector<GameComponentRaws> dispatch_components_;
	};
}

#endif