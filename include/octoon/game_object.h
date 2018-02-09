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

		virtual GameScene* getGameScene() noexcept;
		virtual const GameScene* getGameScene() const noexcept;

		void addChild(GameObjectPtr& child) noexcept;
		void removeChild(GameObjectPtr& child) noexcept;
		void cleanupChildren() noexcept;
		GameObjectPtr findChild(const std::string& name, bool recurse = true) noexcept;

		std::size_t getChildCount() const noexcept;
		GameObjects& getChildren() noexcept;
		const GameObjects& getChildren() const noexcept;

		void setTranslate(const math::float3& v) noexcept;
		void setTranslateAccum(const math::float3& v) noexcept;
		const math::float3& getTranslate() const noexcept;

		void setScale(const math::float3& v) noexcept;
		void setScaleAll(const float v) noexcept;
		void setScaleAccum(const math::float3& v) noexcept;
		const math::float3& getScale() const noexcept;

		void setQuaternion(const math::Quaternion& quat) noexcept;
		void setQuaternionAccum(const math::Quaternion& quat) noexcept;
		const math::Quaternion& getQuaternion() const noexcept;

		void setTransform(const math::float4x4& transform) noexcept;
		void setTransformOnlyRotate(const math::float4x4& transform) noexcept;
		const math::float4x4& getTransform() const noexcept;
		const math::float4x4& getTransformInverse() const noexcept;

		void setWorldTranslate(const math::float3& v) noexcept;
		void setWorldTranslateAccum(const math::float3& v) noexcept;
		const math::float3& getWorldTranslate() const noexcept;

		void setWorldScale(const math::float3& v) noexcept;
		void setWorldScaleAccum(const math::float3& v) noexcept;
		const math::float3& getWorldScale() const noexcept;

		void setWorldQuaternion(const math::Quaternion& quat) noexcept;
		void setWorldQuaternionAccum(const math::Quaternion& quat) noexcept;
		const math::Quaternion& getWorldQuaternion() const noexcept;

		const math::float3& getRight() const noexcept;
		const math::float3& getUpVector() const noexcept;
		const math::float3& getForward() const noexcept;

		void setWorldTransform(const math::float4x4& transform) noexcept;
		void setWorldTransformOnlyRotate(const math::float4x4& transform) noexcept;
		const math::float4x4& getWorldTransform() const noexcept;
		const math::float4x4& getWorldTransformInverse() const noexcept;

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

		GameObjectPtr clone() const except;

	private:
		friend GameObjectManager;

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
		void _updateLocalChildren() const noexcept;
		void _updateWorldChildren() const noexcept;
		void _updateLocalTransform() const noexcept;
		void _updateWorldTransform() const noexcept;
		void _updateParentTransform() const noexcept;

	private:
		GameObject(const GameObject& copy) noexcept = delete;
		GameObject& operator=(const GameObject& copy) noexcept = delete;

	private:
		bool _active;

		std::uint8_t _layer;
		std::size_t _instanceID;

		std::string _name;

		mutable math::float3 _localTranslate;
		mutable math::float3 _localScaling;
		mutable math::Quaternion _localRotation;

		mutable math::float3 _worldTranslate;
		mutable math::float3 _worldScaling;
		mutable math::Quaternion _worldRotation;

		mutable math::float4x4 _localTransform;
		mutable math::float4x4 _localTransformInverse;

		mutable math::float4x4 _worldTransform;
		mutable math::float4x4 _worldTransformInverse;

		mutable bool _localNeedUpdates;
		mutable bool _worldNeedUpdates;

		GameObjects _children;
		GameObjectWeakPtr _parent;

		GameComponents _components;
		std::vector<GameComponents> _dispatchComponents;
	};
}

#endif