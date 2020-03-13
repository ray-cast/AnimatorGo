#include <octoon/guizmo_component.h>
#include <octoon/camera_component.h>
#include <octoon/transform_component.h>
#include <octoon/input/input.h>
#include <octoon/video_feature.h>

namespace octoon
{
	OctoonImplementSubClass(GuizmoComponent, GameComponent, "Guizmo")

	GuizmoComponent::GuizmoComponent() noexcept
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
	}

	GameComponentPtr
	GuizmoComponent::clone() const noexcept
	{
		auto instance = std::make_shared<GuizmoComponent>();
		instance->setName(this->getName());
		return instance;
	}
}