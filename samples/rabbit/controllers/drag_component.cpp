#include "drag_component.h"
#include "rabbit_behaviour.h"
#include <octoon/mesh/cube_wireframe_mesh.h>

namespace rabbit
{
	DragComponent::DragComponent() noexcept
	{
	}

	DragComponent::~DragComponent() noexcept
	{
	}

	void
	DragComponent::setActive(bool active) noexcept
	{
		this->getModel()->setEnable(active);
	}

	bool
	DragComponent::getActive() const noexcept
	{
		return this->getModel()->getEnable();
	}

	const std::optional<octoon::RaycastHit>&
	DragComponent::getSelectedItem() const noexcept
	{
		return this->selectedItem_;
	}
	
	std::optional<octoon::RaycastHit>
	DragComponent::intersectObjects(float x, float y) noexcept
	{
		auto preofile = this->getContext()->profile;
		if (preofile->entitiesModule->camera)
		{
			auto cameraComponent = preofile->entitiesModule->camera->getComponent<octoon::CameraComponent>();
			if (cameraComponent)
			{
				octoon::Raycaster raycaster(cameraComponent->screenToRay(octoon::math::float2(x, y)));
				auto& intersects = raycaster.intersectObjects(preofile->entitiesModule->objects);
				if (!intersects.empty())
					return intersects[0];
			}
		}

		return std::nullopt;
	}

	void
	DragComponent::onEnable() noexcept
	{
		this->gizmoHoverMtl_ = std::make_shared<octoon::material::LineBasicMaterial>(octoon::math::float3(0, 1, 0));
		this->gizmoSelectedMtl_ = std::make_shared<octoon::material::LineBasicMaterial>(octoon::math::float3(0, 0, 1));

		gizmoHover_ = octoon::GameObject::create("GizmoHover");
		gizmoHover_->addComponent<octoon::MeshFilterComponent>(octoon::mesh::CubeWireframeMesh::create(1.0f, 1.0f, 1.0f));
		auto meshRenderHover = gizmoHover_->addComponent<octoon::MeshRendererComponent>(this->gizmoHoverMtl_);
		meshRenderHover->setVisible(false);
		meshRenderHover->setRenderOrder(1);

		gizmoSelected_ = octoon::GameObject::create("GizmoSelect");
		gizmoSelected_->addComponent<octoon::MeshFilterComponent>(octoon::mesh::CubeWireframeMesh::create(1.0f, 1.0f, 1.0f));
		auto meshRenderSelected = gizmoSelected_->addComponent<octoon::MeshRendererComponent>(this->gizmoSelectedMtl_);
		meshRenderSelected->setVisible(false);
		meshRenderSelected->setRenderOrder(1);
	}

	void
	DragComponent::onDisable() noexcept
	{
	}

	void
	DragComponent::onMouseDown(float x, float y) noexcept
	{
		this->selectedItem_ = this->intersectObjects(x, y);
	}

	void
	DragComponent::onMouseUp(float x, float y) noexcept
	{
	}

	void
	DragComponent::onMousePress(float x, float y) noexcept
	{
	}

	void
	DragComponent::onMouseMotion(float x, float y) noexcept
	{
		this->selectedItemHover_ = this->intersectObjects(x, y);
	}

	void
	DragComponent::onUpdate() noexcept
	{
		auto inputFeature = this->getFeature<octoon::InputFeature>();
		if (inputFeature)
		{
			auto input = inputFeature->getInput();
			if (input)
			{
				float x, y;
				input->getMousePos(x, y);

				if (input->isButtonDown(octoon::input::InputButton::Left)) {
					this->onMouseDown(x, y);
				}

				if (input->isButtonUp(octoon::input::InputButton::Left)) {
					this->onMouseUp(x, y);
				}

				if (input->isButtonPressed(octoon::input::InputButton::Left)) {
					this->onMousePress(x, y);
				} else if (!input->isButtonPressed(octoon::input::InputButton::Right) && !input->isButtonPressed(octoon::input::InputButton::Middle)) {
					this->onMouseMotion(x, y);
				}
			}
		}

		if (this->selectedItem_)
		{
			auto hit = this->selectedItem_.value();

			auto meshFilter = hit.object->getComponent<octoon::MeshFilterComponent>();
			auto mesh = meshFilter->getMesh();
			auto& box = mesh->getBoundingBox(hit.mesh).aabb();

			auto transform = this->gizmoSelected_->getComponent<octoon::TransformComponent>();
			transform->setTranslate(box.center());
			transform->setScale(box.size());

			gizmoSelected_->getComponent<octoon::MeshRendererComponent>()->setVisible(true);
		}
		else
		{
			gizmoSelected_->getComponent<octoon::MeshRendererComponent>()->setVisible(false);
		}

		if (this->selectedItemHover_ && this->selectedItem_ != this->selectedItemHover_)
		{
			auto hit = this->selectedItemHover_.value();

			auto meshFilter = hit.object->getComponent<octoon::MeshFilterComponent>();
			auto mesh = meshFilter->getMesh();
			auto& box = mesh->getBoundingBox(hit.mesh).aabb();

			auto transform = this->gizmoHover_->getComponent<octoon::TransformComponent>();
			transform->setTranslate(box.center());
			transform->setScale(box.size());

			gizmoHover_->getComponent<octoon::MeshRendererComponent>()->setVisible(true);
		}
		else
		{
			gizmoHover_->getComponent<octoon::MeshRendererComponent>()->setVisible(false);
		}
	}
}