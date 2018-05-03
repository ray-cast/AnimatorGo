#include <octoon/guizmo_component.h>
#include <octoon/camera_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(GuizmoComponent, GameComponent, "GuizmoComponent")

	GuizmoComponent::GuizmoComponent() noexcept
		: op_(imgui::guizmo::Translate)
	{
	}

	GuizmoComponent::GuizmoComponent(GameObjectPtr& camera) noexcept
		: camera_(camera)
		, op_(imgui::guizmo::Translate)
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
			auto model = transform->getTransform();

			if (imgui::is_key_pressed(input::InputKey::Code::Key1))
				op_ = imgui::guizmo::Translate;
			if (imgui::is_key_pressed(input::InputKey::Code::Key2))
				op_ = imgui::guizmo::Rotation;
			if (imgui::is_key_pressed(input::InputKey::Code::Key3))
				op_ = imgui::guizmo::Scale;

			imgui::guizmo::BeginFrame();
			imgui::guizmo::SetRect(0, 0, imgui::get_display_size().x, imgui::get_display_size().y);
			imgui::guizmo::Manipulate(view.ptr(), project.ptr(), op_, imgui::guizmo::Mode::Local, model.ptr());

			transform->setTransform(model);
		}
	}

	GameComponentPtr
	GuizmoComponent::clone() const noexcept
	{
		auto instance = std::make_shared<GuizmoComponent>();
		instance->setName(this->getName());
		return instance;
	}
}