#include <octoon/guizmo_component.h>
#include <octoon/camera_component.h>
#include <octoon/transform_component.h>

#include <octoon/ui/imgui.h>

namespace octoon
{
	OctoonImplementSubClass(GuizmoComponent, GameComponent, "GuizmoComponent")

	GuizmoComponent::GuizmoComponent() noexcept
	{
	}

	GuizmoComponent::GuizmoComponent(GameObjectPtr& camera) noexcept
		: camera_(camera)
	{
	}

	void
	GuizmoComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::Gui);
	}

	void
	GuizmoComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::Gui);
	}

	void
	GuizmoComponent::onGui() noexcept
	{
		if (camera_)
		{
			auto transform = this->getComponent<TransformComponent>();

			auto& view = camera_->getComponent<CameraComponent>()->getView();
			auto& project = camera_->getComponent<CameraComponent>()->getProjection();
			auto model = transform->getLocalTransform();

			static imgui::guizmo::Operation op(imgui::guizmo::Rotation);
			if (imgui::is_key_pressed(input::InputKey::Code::Q))
				op = imgui::guizmo::Translate;
			if (imgui::is_key_pressed(input::InputKey::Code::W))
				op = imgui::guizmo::Rotation;
			if (imgui::is_key_pressed(input::InputKey::Code::E))
				op = imgui::guizmo::Scale;

			imgui::guizmo::BeginFrame();
			imgui::guizmo::SetRect(0, 0, imgui::get_display_size().x, imgui::get_display_size().y);
			imgui::guizmo::Manipulate(view.ptr(), project.ptr(), op, imgui::guizmo::Mode::Local, model.ptr());

			transform->setLocalTransform(model);
		}
	}

	GameComponentPtr
	GuizmoComponent::clone() const noexcept
	{
		return std::make_shared<GuizmoComponent>();
	}
}