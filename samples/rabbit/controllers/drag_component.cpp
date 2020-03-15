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
	}

	bool
	DragComponent::getActive() const noexcept
	{
		return true;
	}

	const std::optional<octoon::RaycastHit>&
	DragComponent::getSelectedItem() const noexcept
	{
		return this->selectedItem_;
	}

	void
	DragComponent::init(const std::shared_ptr<RabbitContext>& context, const std::shared_ptr<DragModule>& model) noexcept
	{
		this->setContext(context);
		this->setModel(model);
		this->onEnable();
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
		this->gizmoSelectedMtl_ = std::make_shared<octoon::material::Material>();
		this->gizmoSelectedMtl_->set(MATKEY_COLOR_DIFFUSE, octoon::math::float3(0, 0, 1));
		this->gizmoSelectedMtl_->setPrimitiveType(octoon::hal::GraphicsVertexType::LineList);

		this->gizmoHoverMtl_ = std::make_shared<octoon::material::Material>();
		this->gizmoHoverMtl_->set(MATKEY_COLOR_DIFFUSE, octoon::math::float3(0, 1, 0));
		this->gizmoHoverMtl_->setPrimitiveType(octoon::hal::GraphicsVertexType::LineList);

		gizmoHover_ = octoon::GameObject::create("GizmoHover");
		gizmoHover_->addComponent<octoon::MeshFilterComponent>(octoon::mesh::CubeWireframeMesh::create(1.0f, 1.0f, 1.0f));
		gizmoHover_->addComponent<octoon::MeshRendererComponent>(this->gizmoHoverMtl_)->setVisible(false);

		gizmoSelected_ = octoon::GameObject::create("GizmoSelect");
		gizmoSelected_->addComponent<octoon::MeshFilterComponent>(octoon::mesh::CubeWireframeMesh::create(1.0f, 1.0f, 1.0f));
		gizmoSelected_->addComponent<octoon::MeshRendererComponent>(this->gizmoSelectedMtl_)->setVisible(false);
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
				} else {
					this->onMouseMotion(x, y);
				}
			}
		}

		if (this->selectedItem_.has_value())
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

		if (this->selectedItemHover_.has_value() && this->selectedItem_ != this->selectedItemHover_)
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