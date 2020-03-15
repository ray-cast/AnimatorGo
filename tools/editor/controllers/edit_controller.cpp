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
		EditController::createGameObject(const std::any& data) noexcept
		{
			objects_.push_back(GameObject::create("GameObject"));
		}

		void
		EditController::createCubeObject(const std::any& data) noexcept
		{
			objects_.push_back(CubeHelper::create(1, 1, 1));
		}

		void
		EditController::createSphereObject(const std::any& data) noexcept
		{
			objects_.push_back(SphereHelper::create(1));
		}

		void
		EditController::createPlaneObject(const std::any& data) noexcept
		{
			objects_.push_back(PlaneHelper::create(10, 10));
		}

		GameComponentPtr 
		EditController::clone() const noexcept
		{
			return std::make_shared<EditController>();
		}
	}
}