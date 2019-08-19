#include "edit_controller.h"

#include <octoon/octoon.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(EditController, GameComponent, "EditController")

		EditController::EditController() noexcept
		{
		}

		EditController::~EditController() noexcept
		{
		}

		void
		EditController::onActivate() noexcept
		{
			this->addComponentDispatch(GameDispatchType::Gui);
			this->addMessageListener("editor:menu:create:object", std::bind(&EditController::createGameObject, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:create:cube", std::bind(&EditController::createCubeObject, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:create:sphere", std::bind(&EditController::createSphereObject, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:create:plane", std::bind(&EditController::createPlaneObject, this, std::placeholders::_1));
		}

		void
		EditController::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();
			this->removeMessageListener("editor:menu:create:object", std::bind(&EditController::createGameObject, this, std::placeholders::_1));
			this->removeMessageListener("editor:menu:create:cube", std::bind(&EditController::createCubeObject, this, std::placeholders::_1));
			this->removeMessageListener("editor:menu:create:sphere", std::bind(&EditController::createSphereObject, this, std::placeholders::_1));
			this->removeMessageListener("editor:menu:create:plane", std::bind(&EditController::createPlaneObject, this, std::placeholders::_1));
		}

		void
		EditController::createGameObject(const runtime::any& data) noexcept
		{
			objects.push_back(GameObject::create("GameObject"));
		}

		void
		EditController::createCubeObject(const runtime::any& data) noexcept
		{
			objects.push_back(GamePrefabs::instance()->createCube(1, 1, 1));
		}

		void
		EditController::createSphereObject(const runtime::any& data) noexcept
		{
			objects.push_back(GamePrefabs::instance()->createSphere(1));
		}

		void
		EditController::createPlaneObject(const runtime::any& data) noexcept
		{
			objects.push_back(GamePrefabs::instance()->createPlane(10, 10));
		}

		GameComponentPtr 
		EditController::clone() const noexcept
		{
			return std::make_shared<EditController>();
		}
	}
}