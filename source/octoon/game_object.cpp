#include <octoon/game_object.h>
#include <octoon/game_object_manager.h>
#include <octoon/game_component.h>

using namespace octoon::math;

namespace octoon
{
	OctoonImplementSubClass(GameObject, runtime::RttiInterface, "Object")

	GameObject::GameObject() noexcept
		: _active(false)
		, _layer(0)
		, _localScaling(math::float3::One)
		, _localTranslate(math::float3::Zero)
		, _localRotation(math::Quaternion::Zero)
		, _worldScaling(math::float3::One)
		, _worldTranslate(math::float3::Zero)
		, _worldRotation(math::Quaternion::Zero)
		, _localNeedUpdates(true)
		, _worldNeedUpdates(true)
	{
		GameObjectManager::instance()->_instanceObject(this, _instanceID);
	}

	GameObject::~GameObject() noexcept
	{
		this->cleanupChildren();
		this->cleanupComponents();

		GameObjectManager::instance()->_unsetObject(this);
	}

	void
	GameObject::setName(const std::string& name) noexcept
	{
		_name = name;
	}

	void
	GameObject::setName(std::string&& name) noexcept
	{
		_name = std::move(name);
	}

	const std::string&
	GameObject::getName() const noexcept
	{
		return _name;
	}

	void
	GameObject::setActive(bool active) except
	{
		if (_active != active)
		{
			if (active)
				this->_onActivate();
			else
				this->_onDeactivate();

			_active = active;
		}
	}

	void
	GameObject::setActiveUpwards(bool active) except
	{
		if (_active != active)
		{
			if (active)
				this->_onActivate();
			else
				this->_onDeactivate();

			auto parent = this->getParent();
			if (parent)
				parent->setActiveUpwards(active);

			_active = active;
		}
	}

	void
	GameObject::setActiveDownwards(bool active) except
	{
		if (_active != active)
		{
			if (active)
				this->_onActivate();
			else
				this->_onDeactivate();

			for (auto& it : _children)
				it->setActiveDownwards(active);

			_active = active;
		}
	}

	bool
	GameObject::getActive() const noexcept
	{
		return _active;
	}

	void
	GameObject::setLayer(std::uint8_t layer) noexcept
	{
		if (_layer != layer)
		{
			this->_onLayerChangeBefore();

			_layer = layer;

			this->_onLayerChangeAfter();
		}
	}

	std::uint8_t
	GameObject::getLayer() const noexcept
	{
		return _layer;
	}

	std::size_t
	GameObject::getInstanceID() const noexcept
	{
		return _instanceID;
	}

	void
	GameObject::setParent(const GameObjectPtr& parent) noexcept
	{
		assert(this != parent.get());

		auto _weak = _parent.lock();
		if (_weak != parent)
		{
			this->_onMoveBefore();

			if (_weak)
			{
				auto it = _weak->_children.begin();
				auto end = _weak->_children.end();

				for (; it != end; ++it)
				{
					if ((*it).get() == this)
					{
						_weak->_children.erase(it);
						break;
					}
				}
			}

			_parent = parent;
			if (parent)
				parent->_children.push_back(this->downcast_pointer<GameObject>());

			this->_updateWorldChildren();
			this->_onMoveAfter();
		}
	}

	GameObject*
	GameObject::getParent() const noexcept
	{
		return _parent.lock().get();
	}

	GameScene*
	GameObject::getGameScene() noexcept
	{
		auto parent = this->getParent();
		if (parent)
			return parent->getGameScene();
		return nullptr;
	}

	const GameScene*
	GameObject::getGameScene() const noexcept
	{
		auto parent = this->getParent();
		if (parent)
			return parent->getGameScene();
		return nullptr;
	}

	void
	GameObject::addChild(GameObjectPtr& entity) noexcept
	{
		assert(entity);
		entity->setParent(std::dynamic_pointer_cast<GameObject>(this->shared_from_this()));
	}

	void
	GameObject::removeChild(GameObjectPtr& entity) noexcept
	{
		assert(entity);

		auto it = _children.begin();
		auto end = _children.end();
		for (; it != end; ++it)
		{
			if ((*it) == entity)
			{
				(*it)->setParent(nullptr);
				break;
			}
		}

		if (it != end)
		{
			_children.erase(it);
		}
	}

	void
	GameObject::cleanupChildren() noexcept
	{
		for (auto& it : _children)
			it.reset();

		_children.clear();
	}

	GameObjectPtr
	GameObject::findChild(const std::string& name, bool recuse) noexcept
	{
		for (auto& it : _children)
		{
			if (it->getName() == name)
			{
				return it;
			}
		}

		if (recuse)
		{
			for (auto& it : _children)
			{
				auto result = it->findChild(name, recuse);
				if (result)
				{
					return result;
				}
			}
		}

		return nullptr;
	}

	std::size_t
	GameObject::getChildCount() const noexcept
	{
		return _children.size();
	}

	GameObjects&
	GameObject::getChildren() noexcept
	{
		return _children;
	}

	const GameObjects&
	GameObject::getChildren() const noexcept
	{
		return _children;
	}

	void
	GameObject::setTranslate(const math::float3& pos) noexcept
	{
		if (_localTranslate != pos)
		{
			this->_onMoveBefore();

			_localTranslate = pos;
			_localNeedUpdates = true;

			this->_updateLocalChildren();
			this->_onMoveAfter();
		}
	}

	void
	GameObject::setTranslateAccum(const math::float3& v) noexcept
	{
		this->setTranslate(_localTranslate + v);
	}

	const math::float3&
	GameObject::getTranslate() const noexcept
	{
		_updateLocalTransform();
		return _localTranslate;
	}

	void
	GameObject::setScale(const math::float3& scale) noexcept
	{
		if (_localScaling != scale)
		{
			this->_onMoveBefore();

			_localScaling = scale;
			_localNeedUpdates = true;

			this->_updateLocalChildren();
			this->_onMoveAfter();
		}
	}

	void
	GameObject::setScaleAll(const float scale) noexcept
	{
		this->setScale(float3(scale, scale, scale));
	}

	void
	GameObject::setScaleAccum(const math::float3& scale) noexcept
	{
		this->setScale(_localScaling + scale);
	}

	const math::float3&
	GameObject::getScale() const noexcept
	{
		_updateLocalTransform();
		return _localScaling;
	}

	void
	GameObject::setQuaternion(const math::Quaternion& quat) noexcept
	{
		if (_localRotation != quat)
		{
			this->_onMoveBefore();

			_localRotation = quat;
			_localNeedUpdates = true;

			this->_updateLocalChildren();
			this->_onMoveAfter();
		}
	}

	void
	GameObject::setQuaternionAccum(const math::Quaternion& quat) noexcept
	{
		this->setQuaternion(math::cross(quat, _localRotation));
	}

	const math::Quaternion&
	GameObject::getQuaternion() const noexcept
	{
		_updateLocalTransform();
		return _localRotation;
	}

	const math::float3&
	GameObject::getRight() const noexcept
	{
		_updateLocalTransform();
		return math::right(_localTransform);
	}

	const math::float3&
	GameObject::getUpVector() const noexcept
	{
		_updateLocalTransform();
		return math::up(_localTransform);
	}

	const math::float3&
	GameObject::getForward() const noexcept
	{
		_updateLocalTransform();
		return math::forward(_localTransform);
	}

	void
	GameObject::setTransform(const math::float4x4& transform) noexcept
	{
		this->_onMoveBefore();

		_localTransform = transform.get_transform(_localTranslate, _localRotation, _localScaling);
		_localNeedUpdates = false;

		this->_updateLocalChildren();
		this->_onMoveAfter();
	}

	void
	GameObject::setTransformOnlyRotate(const math::float4x4& transform) noexcept
	{
		this->_onMoveBefore();

		_localTransform = transform.get_transform_without_scaler(_localTranslate, _localRotation);
		_localTransform.scale(_localScaling);
		_localNeedUpdates = false;

		this->_updateLocalChildren();
		this->_onMoveAfter();
	}

	const math::float4x4&
	GameObject::getTransform() const noexcept
	{
		this->_updateLocalTransform();
		return _localTransform;
	}

	const math::float4x4&
	GameObject::getTransformInverse() const noexcept
	{
		this->_updateLocalTransform();
		return _localTransformInverse;
	}

	void
	GameObject::setWorldTranslate(const math::float3& pos) noexcept
	{
		if (_worldTranslate != pos)
		{
			this->_onMoveBefore();

			_worldTranslate = pos;
			_worldNeedUpdates = true;

			this->_updateWorldChildren();
			this->_onMoveAfter();
		}
	}

	void
	GameObject::setWorldTranslateAccum(const math::float3& v) noexcept
	{
		this->setWorldTranslate(_worldTranslate + v);
	}

	const math::float3&
	GameObject::getWorldTranslate() const noexcept
	{
		_updateWorldTransform();
		return _worldTranslate;
	}

	void
	GameObject::setWorldScale(const math::float3& pos) noexcept
	{
		if (_worldScaling != pos)
		{
			this->_onMoveBefore();

			_worldScaling = pos;
			_worldNeedUpdates = true;

			this->_updateWorldChildren();
			this->_onMoveAfter();
		}
	}

	void
	GameObject::setWorldScaleAccum(const math::float3& scale) noexcept
	{
		this->setWorldScale(_worldScaling + scale);
	}

	const math::float3&
	GameObject::getWorldScale() const noexcept
	{
		_updateWorldTransform();
		return _worldScaling;
	}

	void
	GameObject::setWorldQuaternion(const math::Quaternion& quat) noexcept
	{
		if (_worldRotation != quat)
		{
			this->_onMoveBefore();

			_worldRotation = quat;
			_worldNeedUpdates = true;

			this->_updateWorldChildren();
			this->_onMoveAfter();
		}
	}

	void
	GameObject::setWorldQuaternionAccum(const math::Quaternion& quat) noexcept
	{
		this->setQuaternion(math::cross(quat, _worldRotation));
	}

	const math::Quaternion&
	GameObject::getWorldQuaternion() const noexcept
	{
		_updateWorldTransform();
		return _worldRotation;
	}

	void
	GameObject::setWorldTransform(const math::float4x4& transform) noexcept
	{
		this->_onMoveBefore();

		_worldTransform = transform.get_transform(_worldTranslate, _worldRotation, _worldScaling);
		_worldTransformInverse = math::transform_inverse(_worldTransform);
		_worldNeedUpdates = false;

		this->_updateWorldChildren();
		this->_onMoveAfter();
	}

	void
	GameObject::setWorldTransformOnlyRotate(const math::float4x4& transform) noexcept
	{
		this->_onMoveBefore();

		_worldTransform = transform.get_transform_without_scaler(_worldTranslate, _worldRotation);
		_worldTransform.scale(_worldScaling);
		_worldTransformInverse = math::transform_inverse(_worldTransform);

		_worldNeedUpdates = false;

		this->_updateWorldChildren();
		this->_onMoveAfter();
	}

	const math::float4x4&
	GameObject::getWorldTransform() const noexcept
	{
		this->_updateWorldTransform();
		return _worldTransform;
	}

	const math::float4x4&
	GameObject::getWorldTransformInverse() const noexcept
	{
		this->_updateWorldTransform();
		return _worldTransformInverse;
	}

	void
	GameObject::addComponent(const GameComponentPtr& gameComponent) except
	{
		assert(gameComponent);
		assert(gameComponent->_gameObject == nullptr);

		auto it = std::find(_components.begin(), _components.end(), gameComponent);
		if (it == _components.end())
		{
			gameComponent->_setGameObject(this);
			gameComponent->onAttach();

			if (this->getActive() && gameComponent->getActive())
				gameComponent->onActivate();

			for (auto& component : _components)
				gameComponent->onAttachComponent(component);

			for (auto& component : _components)
				component->onAttachComponent(gameComponent);

			_components.push_back(gameComponent);
		}
	}

	void
	GameObject::addComponent(GameComponentPtr&& component) except
	{
		this->addComponent(component);
	}

	void
	GameObject::removeComponent(const GameComponentPtr& gameComponent) noexcept
	{
		assert(gameComponent);
		assert(gameComponent->_gameObject == this);

		auto it = std::find(_components.begin(), _components.end(), gameComponent);
		if (it != _components.end())
		{
			_components.erase(it);

			for (auto& compoent : _components)
				compoent->onDetachComponent(gameComponent);

			for (auto& component : _components)
				gameComponent->onDetachComponent(component);

			if (this->getActive() && gameComponent->getActive())
				gameComponent->onDeactivate();

			gameComponent->onDetach();
			gameComponent->_setGameObject(nullptr);

			this->removeComponentDispatchs(gameComponent);
		}
	}

	void
	GameObject::cleanupComponents() noexcept
	{
		for (auto it = _components.begin(); it != _components.end();)
		{
			auto gameComponent = *it;
			auto nextComponent = _components.erase(it);

			for (auto& compoent : _components)
				compoent->onDetachComponent(gameComponent);

			for (auto& component : _components)
				gameComponent->onDetachComponent(component);

			if (this->getActive() && (gameComponent)->getActive())
				gameComponent->onDeactivate();

			gameComponent->onDetach();
			gameComponent->_setGameObject(nullptr);

			this->removeComponentDispatchs(gameComponent);

			it = nextComponent;
		}
	}

	GameComponentPtr
	GameObject::getComponent(const runtime::Rtti* type) const noexcept
	{
		assert(type);

		for (auto& it : _components)
		{
			if (it->isA(type))
				return it;
		}

		return nullptr;
	}

	GameComponentPtr
	GameObject::getComponent(const runtime::Rtti& type) const noexcept
	{
		return this->getComponent(&type);
	}

	GameComponentPtr
	GameObject::getComponentInChildren(const runtime::Rtti* type) const noexcept
	{
		assert(type);

		for (auto& it : _children)
		{
			for (auto& component : it->_components)
			{
				if (component->isA(type))
					return component;
			}

			auto component = it->getComponentInChildren(type);
			if (component)
				return component;
		}

		return nullptr;
	}

	GameComponentPtr
	GameObject::getComponentInChildren(const runtime::Rtti& type) const noexcept
	{
		return this->getComponentInChildren(&type);
	}

	void
	GameObject::getComponentsInChildren(const runtime::Rtti* type, GameComponents& components) const noexcept
	{
		assert(type);

		for (auto& it : _children)
		{
			for (auto& component : it->_components)
			{
				if (component->isA(type))
					components.push_back(component);
			}

			it->getComponentInChildren(type);
		}
	}

	void
	GameObject::getComponentsInChildren(const runtime::Rtti& type, GameComponents& components) const noexcept
	{
		return this->getComponentsInChildren(&type, components);
	}

	const GameComponents&
	GameObject::getComponents() const noexcept
	{
		return _components;
	}

	void
	GameObject::addComponentDispatch(GameDispatchType type, const GameComponentPtr& component) noexcept
	{
		assert(component);

		if (_dispatchComponents.empty())
			_dispatchComponents.resize(GameDispatchType::GameDispatchTypeRangeSize);

		if (std::find(_dispatchComponents[type].begin(), _dispatchComponents[type].end(), component) != _dispatchComponents[type].end())
			return;

		if (this->getActive())
		{
			if (type == GameDispatchType::GameDispatchTypeFrame ||
				type == GameDispatchType::GameDispatchTypeFrameBegin ||
				type == GameDispatchType::GameDispatchTypeFrameEnd)
			{
				if (_dispatchComponents[GameDispatchType::GameDispatchTypeFrame].empty() &&
					_dispatchComponents[GameDispatchType::GameDispatchTypeFrameBegin].empty() &&
					_dispatchComponents[GameDispatchType::GameDispatchTypeFrameEnd].empty())
				{
					GameObjectManager::instance()->_activeObject(this, true);
				}
			}
		}

		_dispatchComponents[type].push_back(component);
	}

	void
	GameObject::removeComponentDispatch(GameDispatchType type, const GameComponentPtr& component) noexcept
	{
		assert(component);

		if (_dispatchComponents.empty())
			return;

		auto it = std::find(_dispatchComponents[type].begin(), _dispatchComponents[type].end(), component);
		if (it == _dispatchComponents[type].end())
			return;

		if (this->getActive())
		{
			if (type == GameDispatchType::GameDispatchTypeFrame ||
				type == GameDispatchType::GameDispatchTypeFrameBegin ||
				type == GameDispatchType::GameDispatchTypeFrameEnd)
			{
				if (_dispatchComponents[GameDispatchType::GameDispatchTypeFrame].empty() &&
					_dispatchComponents[GameDispatchType::GameDispatchTypeFrameBegin].empty() &&
					_dispatchComponents[GameDispatchType::GameDispatchTypeFrameEnd].empty())
				{
					GameObjectManager::instance()->_activeObject(this, false);
				}
			}
		}

		_dispatchComponents[type].erase(it);
	}

	void
	GameObject::removeComponentDispatchs(const GameComponentPtr& component) noexcept
	{
		assert(component);

		for (auto& dispatch : _dispatchComponents)
		{
			auto it = std::find(dispatch.begin(), dispatch.end(), component);
			if (it != dispatch.end())
				dispatch.erase(it);
		}
	}

	void
	GameObject::destroy() noexcept
	{
		this->cleanupChildren();
		this->cleanupComponents();
		this->setParent(nullptr);
	}

	GameObjectPtr
	GameObject::clone() const except
	{
		auto instance = std::make_shared<GameObject>();
		instance->setParent(_parent.lock());
		instance->setName(this->getName());
		instance->setLayer(this->getLayer());
		instance->setQuaternion(this->getQuaternion());
		instance->setScale(this->getScale());
		instance->setTranslate(this->getTranslate());

		for (auto& it : _components)
			instance->addComponent(it->clone());

		for (auto& it : this->getChildren())
			instance->addChild(it->clone());

		return instance;
	}

	void
	GameObject::_onFrameBegin() except
	{
		assert(!_dispatchComponents.empty());

		auto& components = _dispatchComponents[GameDispatchType::GameDispatchTypeFrameBegin];
		for (auto& it : components)
			it->onFrameBegin();
	}

	void
	GameObject::_onFrame() except
	{
		assert(!_dispatchComponents.empty());

		auto& components = _dispatchComponents[GameDispatchType::GameDispatchTypeFrame];
		for (auto& it : components)
			it->onFrame();
	}

	void
	GameObject::_onFrameEnd() except
	{
		assert(!_dispatchComponents.empty());

		auto& components = _dispatchComponents[GameDispatchType::GameDispatchTypeFrameEnd];
		for (auto& it : components)
			it->onFrameEnd();
	}

	void
	GameObject::_onActivate() except
	{
		for (auto& it : _components)
		{
			if (it->getActive())
				it->onActivate();
		}

		if (!_dispatchComponents.empty())
		{
			if (!_dispatchComponents[GameDispatchType::GameDispatchTypeFrame].empty() ||
				!_dispatchComponents[GameDispatchType::GameDispatchTypeFrameBegin].empty() ||
				!_dispatchComponents[GameDispatchType::GameDispatchTypeFrameEnd].empty())
			{
				GameObjectManager::instance()->_activeObject(this, true);
			}
		}
	}

	void
	GameObject::_onDeactivate() noexcept
	{
		if (!_dispatchComponents.empty())
		{
			if (!_dispatchComponents[GameDispatchType::GameDispatchTypeFrame].empty() ||
				!_dispatchComponents[GameDispatchType::GameDispatchTypeFrameBegin].empty() ||
				!_dispatchComponents[GameDispatchType::GameDispatchTypeFrameEnd].empty())
			{
				GameObjectManager::instance()->_activeObject(this, false);
			}
		}

		for (auto& it : _components)
		{
			if (it->getActive())
				it->onDeactivate();
		}
	}

	void
	GameObject::_onMoveBefore() except
	{
		if (!this->getActive())
			return;

		if (!_dispatchComponents.empty())
		{
			auto& components = _dispatchComponents[GameDispatchType::GameDispatchTypeMoveBefore];
			for (auto& it : components)
			{
				if (it->getActive())
					it->onMoveBefore();
			}
		}

		for (auto& it : _children)
		{
			if (it->getActive())
				it->_onMoveBefore();
		}
	}

	void
	GameObject::_onMoveAfter() except
	{
		if (!this->getActive())
			return;

		if (!_dispatchComponents.empty())
		{
			auto& components = _dispatchComponents[GameDispatchType::GameDispatchTypeMoveAfter];
			for (auto& it : components)
			{
				if (it->getActive())
					it->onMoveAfter();
			}
		}

		for (auto& it : _children)
		{
			if (it->getActive())
				it->_onMoveAfter();
		}
	}

	void
	GameObject::_onLayerChangeBefore() except
	{
		if (this->getActive())
		{
			for (auto& it : _components)
			{
				if (it->getActive())
					it->onLayerChangeBefore();
			}
		}
	}

	void
	GameObject::_onLayerChangeAfter() except
	{
		if (this->getActive())
		{
			for (auto& it : _components)
			{
				if (it->getActive())
					it->onLayerChangeAfter();
			}
		}
	}

	void
	GameObject::_updateLocalChildren() const noexcept
	{
		_worldNeedUpdates = true;

		for (auto& it : _children)
			it->_updateLocalChildren();
	}

	void
	GameObject::_updateWorldChildren() const noexcept
	{
		this->_updateParentTransform();
		this->_updateLocalChildren();
	}

	void
	GameObject::_updateLocalTransform() const noexcept
	{
		if (_localNeedUpdates)
		{
			_localTransform.make_transform(_localTranslate, _localRotation, _localScaling);
			_localTransformInverse = math::transform_inverse(_localTransform);

			_localNeedUpdates = false;
		}
	}

	void
	GameObject::_updateWorldTransform() const noexcept
	{
		if (_worldNeedUpdates)
		{
			if (_parent.lock())
			{
				auto& baseTransform = _parent.lock()->getWorldTransform();
				_worldTransform = math::transform_multiply(baseTransform, this->getTransform());
				_worldTransform.get_transform(_worldTranslate, _worldRotation, _worldScaling);
				_worldTransformInverse = math::transform_inverse(_worldTransform);
			}
			else
			{
				_worldTranslate = _localTranslate;
				_worldScaling = _localScaling;
				_worldRotation = _localRotation;
				_worldTransform.make_transform(_worldTranslate, _worldRotation, _worldScaling);
				_worldTransformInverse = math::transform_inverse(_worldTransform);
			}

			_worldNeedUpdates = false;
		}
	}

	void
	GameObject::_updateParentTransform() const noexcept
	{
		if (_worldNeedUpdates)
		{
			_worldTransform.make_transform(_worldTranslate, _worldRotation, _worldScaling);
			_worldNeedUpdates = false;
		}

		if (_parent.lock())
		{
			auto& baseTransformInverse = _parent.lock()->getWorldTransformInverse();
			_localTransform = math::transform_multiply(baseTransformInverse, _worldTransform);
			_localTransform.get_transform(_localTranslate, _localRotation, _localScaling);
		}
		else
		{
			_localScaling = _worldScaling;
			_localRotation = _worldRotation;
			_localTranslate = _worldTranslate;
		}
	}
}