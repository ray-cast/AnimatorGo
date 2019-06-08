#include "camera_window.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>
#include <octoon/editor_camera_component.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(CameraWindow, GameComponent, "CameraWindow")

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
			this->addComponentDispatch(GameDispatchType::Gui);

			camera_ = GameObject::create("MainCamera");
			camera_->getComponent<TransformComponent>()->setTranslate(math::float3(0, 0, -20));
			camera_->addComponent<EditorCameraComponent>();

			auto camera = camera_->addComponent<PerspectiveCameraComponent>(45.0f);
			camera->setCameraType(video::CameraType::Main);
			camera->setClearColor(octoon::math::float4(0.2f, 0.2f, 0.2f, 1.0f));
		}

		void
		CameraWindow::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();

			camera_.reset();
		}

		void
		CameraWindow::onGui() noexcept
		{
		}

		GameComponentPtr
		CameraWindow::clone() const noexcept
		{
			return std::make_shared<CameraWindow>();
		}
	}
}