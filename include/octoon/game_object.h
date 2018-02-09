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

		std::size_t getInstanceID() const noexcept;

		void setParent(const GameObjectPtr& parent) noexcept;
		GameObject* getParent() const noexcept;

		void addChild(GameObjectPtr& child) noexcept;
		void removeChild(GameObjectPtr& child) noexcept;
		void cleanupChildren() noexcept;
		GameObjectPtr findChild(const std::string& name, bool recurse = true) noexcept;

		std::size_t getChildCount() const noexcept;
		GameObjects& getChildren() noexcept;
		const GameObjects& getChildren() const noexcept;

		void addComponent(const GameComponentPtr& component) except;
		void addComponent(GameComponentPtr&& component) except;
		void removeComponent(const GameComponentPtr& component) noexcept;
		void cleanupComponents() noexcept;

		template<typename T>
		std::shared_ptr<T> getComponent() const noexcept { return std::dynamic_pointer_cast<T>(this->getComponent(T::RTTI)); }
		GameComponentPtr getComponent(const runtime::Rtti* type) const noexcept;
		GameComponentPtr getComponent(const runtime::Rtti& type) const noexcept;

		template<typename T>
		std::shared_ptr<T> getComponentInChildren() const noexcept { return std::dynamic_pointer_cast<T>(this->getComponentInChildren(T::RTTI)); }
		GameComponentPtr getComponentInChildren(const runtime::Rtti* type) const noexcept;
		GameComponentPtr getComponentInChildren(const runtime::Rtti& type) const noexcept;

		template<typename T>
		void getComponentsInChildren(GameComponents& components) const noexcept { this->getComponentsInChildren(T::RTTI, components); }
		void getComponentsInChildren(const runtime::Rtti* type, GameComponents& components) const noexcept;
		void getComponentsInChildren(const runtime::Rtti& type, GameComponents& components) const noexcept;

		const GameComponents& getComponents() const noexcept;

		void addComponentDispatch(GameDispatchType type, const GameComponentPtr& component) noexcept;
		void removeComponentDispatch(GameDispatchType type, const GameComponentPtr& component) noexcept;
		void removeComponentDispatchs(const GameComponentPtr& component) noexcept;

		void destroy() noexcept;

		virtual GameScene* getGameScene() noexcept;
		virtual const GameScene* getGameScene() const noexcept;

		GameObjectPtr clone() const except;

	private:
		friend class GameObjectManager;
		friend class TransformComponent;

		void _onActivate() except;
		void _onDeactivate() noexcept;

		void _onFrameBegin() except;
		void _onFrame() except;
		void _onFrameEnd() except;

		void _onMoveBefore() except;
		void _onMoveAfter() except;

		void _onLayerChangeBefore() except;
		void _onLayerChangeAfter() except;

	private:
		GameObject(const GameObject& copy) noexcept = delete;
		GameObject& operator=(const GameObject& copy) noexcept = delete;

	private:
		bool active_;

		std::uint8_t layer_;
		std::size_t instanceID_;

		std::string name_;

		GameObjects children_;
		GameObjectWeakPtr parent_;

		GameComponents components_;
		std::vector<GameComponents> dispatchComponents_;
	};
}

#endif