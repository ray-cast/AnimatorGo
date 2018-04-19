#include <octoon/line_renderer_component.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(LineRendererComponent, RenderComponent, "LineRenderer")

	LineRendererComponent::LineRendererComponent() noexcept
	{
	}

	LineRendererComponent::LineRendererComponent(video::MaterialPtr&& material) noexcept
	{
		this->setMaterial(std::move(material));
	}

	LineRendererComponent::LineRendererComponent(const video::MaterialPtr& material) noexcept
	{
		this->setMaterial(material);
	}

	LineRendererComponent::~LineRendererComponent() noexcept
	{
	}

	GameComponentPtr
	LineRendererComponent::clone() const noexcept
	{
		auto instance = std::make_shared<LineRendererComponent>();
		instance->setMaterial(this->getMaterial()->clone());
		return instance;
	}

	void
	LineRendererComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter, this);

		auto transform = this->getComponent<TransformComponent>();
		auto meshFilter = this->getComponent<MeshFilter>();

		geometry_ = std::make_shared<video::Geometry>();
		geometry_->setDrawType(video::DrawType::Lines);
		geometry_->setActive(true);
		geometry_->setMesh(meshFilter ? meshFilter->getMesh() : nullptr);
		geometry_->setMaterial(this->getMaterial());
		geometry_->setTransform(transform->getTransform(), transform->getTransformInverse());
	}

	void
	LineRendererComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter, this);

		if (geometry_)
			geometry_->setActive(false);
	}

	void
	LineRendererComponent::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isInstanceOf<MeshFilter>())
		{
			onMeshReplaceEvent_ = std::bind(&LineRendererComponent::onMeshReplace, this, std::placeholders::_1);
			component->downcast<MeshFilter>()->addMeshListener(&onMeshReplaceEvent_);
		}
	}

	void
	LineRendererComponent::onDetachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isInstanceOf<MeshFilter>())
			component->downcast<MeshFilter>()->removeMeshListener(&onMeshReplaceEvent_);
	}

	void
	LineRendererComponent::onMoveBefore() noexcept
	{
	}

	void
	LineRendererComponent::onMoveAfter() noexcept
	{
		if (geometry_)
		{
			auto transform = this->getComponent<TransformComponent>();
			geometry_->setTransform(transform->getTransform(), transform->getTransformInverse());
		}
	}

	void
	LineRendererComponent::onMeshReplace(const video::MeshPtr& mesh) noexcept
	{
		if (geometry_)
			geometry_->setMesh(mesh);
	}

	void
	LineRendererComponent::onMaterialReplace(const video::MaterialPtr& material) noexcept
	{
		if (geometry_)
			geometry_->setMaterial(material);
	}
}