#include "camera_window.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>
#include <octoon/editor_camera_component.h>

namespace MysticLit
{
	OctoonImplementSubClass(CameraWindow, octoon::GameComponent, "CameraWindow")

	CameraWindow::CameraWindow() noexcept
		: framebufferSizeW_(0)
		, framebufferSizeH_(0)
		, isShowedCameraWindow_(true)
	{
	}

	CameraWindow::~CameraWindow() noexcept
	{
	}

	void
	CameraWindow::onActivate() noexcept
	{
		this->addComponentDispatch(octoon::GameDispatchType::Gui);

		camera_ = octoon::GameObject::create("MainCamera");
		camera_->addComponent<octoon::EditorCameraComponent>();

		auto camera = camera_->addComponent<octoon::PerspectiveCameraComponent>(60.0f);
		camera->setCameraType(octoon::video::CameraType::Main);
		camera->setClearColor(octoon::math::float4(0.1f, 0.1f, 0.1f, 1.0f));

		this->addMessageListener("editor:camera:set", std::bind(&CameraWindow::setCamera, this, std::placeholders::_1));
	}

	void
	CameraWindow::onDeactivate() noexcept
	{
		this->removeComponentDispatchs();
		this->removeMessageListener("editor:camera:set", std::bind(&CameraWindow::setCamera, this, std::placeholders::_1));

		if (camera_)
			camera_.reset();
	}

	void
	CameraWindow::onGui() noexcept
	{
	}

	void 
	CameraWindow::setCamera(const octoon::runtime::any& data) noexcept
	{
		if (data.type() == typeid(octoon::GameObjectPtr))
			camera_ = octoon::runtime::any_cast<octoon::GameObjectPtr>(data);
	}

	octoon::GameComponentPtr
	CameraWindow::clone() const noexcept
	{
		return std::make_shared<CameraWindow>();
	}
}