#include "object_manager.h"

#include <octoon/octoon.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(ObjectManager, GameComponent, "ObjectManager")

		ObjectManager::ObjectManager() noexcept
		{
		}

		ObjectManager::~ObjectManager() noexcept
		{
		}

		void
		ObjectManager::onActivate() noexcept
		{
			this->addComponentDispatch(GameDispatchType::Gui);
			this->addMessageListener("editor:menu:create:object", std::bind(&ObjectManager::createGameObject, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:create:cube", std::bind(&ObjectManager::createCubeObject, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:create:sphere", std::bind(&ObjectManager::createSphereObject, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:create:plane", std::bind(&ObjectManager::createPlaneObject, this, std::placeholders::_1));
		}

		void
		ObjectManager::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();
			this->removeMessageListener("editor:menu:create:object", std::bind(&ObjectManager::createGameObject, this, std::placeholders::_1));
		}

		void
		ObjectManager::createGameObject(const runtime::any& data) noexcept
		{
			objects_.push_back(GameObject::create("GameObject"));
		}

		void
		ObjectManager::createCubeObject(const runtime::any& data) noexcept
		{
			objects_.push_back(GamePrefabs::instance()->createCube(1, 1, 1));
		}

		void
		ObjectManager::createSphereObject(const runtime::any& data) noexcept
		{
			objects_.push_back(GamePrefabs::instance()->createSphere(1));
		}

		void
		ObjectManager::createPlaneObject(const runtime::any& data) noexcept
		{
			objects_.push_back(GamePrefabs::instance()->createPlane(10, 10));
		}

		GameComponentPtr 
		ObjectManager::clone() const noexcept
		{
			return std::make_shared<ObjectManager>();
		}
	}
}