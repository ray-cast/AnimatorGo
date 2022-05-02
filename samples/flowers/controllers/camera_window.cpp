#include "camera_window.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>
#include <octoon/editor_camera_component.h>

namespace flower
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
	CameraWindow::setCamera(const std::any& data) noexcept
	{
		if (data.type() == typeid(octoon::GameObjectPtr))
			camera_ = std::any_cast<octoon::GameObjectPtr>(data);
	}

	octoon::GameComponentPtr
	CameraWindow::clone() const noexcept
	{
		return std::make_shared<CameraWindow>();
	}
}