#include "grid_component.h"
#include "rabbit_behaviour.h"
#include <octoon/material/material.h>
#include <octoon/mesh/cube_wireframe_mesh.h>

namespace rabbit
{
	GridComponent::GridComponent() noexcept
	{
	}

	GridComponent::~GridComponent() noexcept
	{
	}

	void
	GridComponent::setActive(bool active) noexcept
	{
		return this->getModel()->setEnable(active);
	}

	bool
	GridComponent::getActive() const noexcept
	{
		return this->getModel()->getEnable();
	}
	
	void
	GridComponent::onEnable() noexcept
	{
		auto material = std::make_shared<octoon::material::LineBasicMaterial>(octoon::math::float3(0.5f, 0.5f, 0.5f));
		material->setDepthEnable(false);
		material->setDepthWriteEnable(false);

		this->gizmo_ = octoon::GameObject::create("CoordinateSystem");
		this->gizmo_->addComponent<octoon::BoxColliderComponent>(100, 100, 1);
		this->gizmo_->addComponent<octoon::MeshFilterComponent>(octoon::mesh::PlaneMesh::create(100.0f, 100.0f, 20, 20, true));
		this->gizmo_->addComponent<octoon::MeshRendererComponent>(material)->setRenderOrder(-1);

		auto rigidbody = this->gizmo_->addComponent<octoon::RigidbodyComponent>();
		rigidbody->setIsKinematic(true);
		rigidbody->setGroupMask(0xFFFF);

		auto transform = this->gizmo_->getComponent<octoon::TransformComponent>();
		transform->setQuaternion(octoon::math::Quaternion(octoon::math::float3(octoon::math::PI * 0.5f, 0, 0)));
		transform->setTranslate(octoon::math::float3(0, -1, 0));
	}

	void
	GridComponent::onDisable() noexcept
	{
		this->gizmo_.reset();
	}
}