#include <octoon/mesh_renderer_component.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(MeshRendererComponent, RenderComponent, "MeshRenderer")

	MeshRendererComponent::MeshRendererComponent() noexcept
	{
	}

	MeshRendererComponent::MeshRendererComponent(video::MaterialPtr&& material) noexcept
	{
		this->setMaterial(std::move(material));
	}

	MeshRendererComponent::MeshRendererComponent(const video::MaterialPtr& material) noexcept
	{
		this->setMaterial(material);
	}

	MeshRendererComponent::~MeshRendererComponent() noexcept
	{
	}

	GameComponentPtr
	MeshRendererComponent::clone() const noexcept
	{
		auto instance = std::make_shared<MeshRendererComponent>();
		instance->setMaterial(this->getMaterial()->clone());
		return instance;
	}

	void
	MeshRendererComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);

		auto transform = this->getComponent<TransformComponent>();
		auto meshFilter = this->getComponent<MeshFilterComponent>();

		geometry_ = std::make_shared<video::Geometry>();
		geometry_->setDrawType(video::DrawType::Triangles);
		geometry_->setActive(true);
		geometry_->setMesh(meshFilter ? meshFilter->getMesh() : nullptr);
		geometry_->setMaterial(this->getMaterial());
		geometry_->setTransform(transform->getTransform(), transform->getTransformInverse());
	}

	void
	MeshRendererComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);

		if (geometry_)
			geometry_->setActive(false);
	}

	void
	MeshRendererComponent::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isInstanceOf<MeshFilterComponent>())
		{
			onMeshReplaceEvent_ = std::bind(&MeshRendererComponent::onMeshReplace, this, std::placeholders::_1);
			component->downcast<MeshFilterComponent>()->addMeshListener(&onMeshReplaceEvent_);
		}
	}

	void
	MeshRendererComponent::onDetachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isInstanceOf<MeshFilterComponent>())
			component->downcast<MeshFilterComponent>()->removeMeshListener(&onMeshReplaceEvent_);
	}

	void
	MeshRendererComponent::onMoveBefore() noexcept
	{
	}

	void
	MeshRendererComponent::onMoveAfter() noexcept
	{
		if (geometry_)
		{
			auto transform = this->getComponent<TransformComponent>();
			geometry_->setTransform(transform->getTransform(), transform->getTransformInverse());
		}
	}

	void
	MeshRendererComponent::onMeshReplace(const model::MeshPtr& mesh) noexcept
	{
		if (geometry_)
			geometry_->setMesh(mesh);
	}

	void
	MeshRendererComponent::onMaterialReplace(const video::MaterialPtr& material) noexcept
	{
		if (geometry_)
			geometry_->setMaterial(material);
	}
}