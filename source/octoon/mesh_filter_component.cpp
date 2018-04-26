#include <octoon/mesh_filter_component.h>

namespace octoon
{
	OctoonImplementSubClass(MeshFilterComponent, GameComponent, "MeshFilter")

	MeshFilterComponent::MeshFilterComponent() noexcept
	{
	}

	MeshFilterComponent::MeshFilterComponent(model::Mesh&& mesh) noexcept
	{
		this->setMesh(std::make_shared<model::Mesh>(std::move(mesh)));
	}

	MeshFilterComponent::MeshFilterComponent(const model::Mesh& mesh) noexcept
	{
		this->setMesh(std::make_shared<model::Mesh>(mesh));
	}

	MeshFilterComponent::MeshFilterComponent(model::MeshPtr&& mesh) noexcept
	{
		this->setMesh(std::move(mesh));
	}

	MeshFilterComponent::MeshFilterComponent(const model::MeshPtr& mesh) noexcept
	{
		this->setMesh(mesh);
	}

	MeshFilterComponent::~MeshFilterComponent() noexcept
	{
	}

	void
	MeshFilterComponent::setMesh(model::MeshPtr&& mesh) noexcept
	{
		if (mesh_ != mesh)
		{
			this->onMeshReplace(mesh);
			mesh_ = std::move(mesh);
		}
	}

	void
	MeshFilterComponent::setMesh(const model::MeshPtr& mesh) noexcept
	{
		if (mesh_ != mesh)
		{
			this->onMeshReplace(mesh);
			mesh_ = mesh;
		}
	}

	const model::MeshPtr&
	MeshFilterComponent::getMesh() const noexcept
	{
		return mesh_;
	}

	void
	MeshFilterComponent::addMeshListener(OnMeshReplaceEvent* func) noexcept
	{
		delegates_.push_back(func);
	}

	void
	MeshFilterComponent::removeMeshListener(const OnMeshReplaceEvent* func) noexcept
	{
		auto it = std::find(delegates_.begin(), delegates_.end(), func);
		if (it != delegates_.end())
			delegates_.erase(it);
	}

	GameComponentPtr
	MeshFilterComponent::clone() const noexcept
	{
		auto instance = std::make_shared<MeshFilterComponent>();
		instance->setMesh(mesh_ ? mesh_->clone() : nullptr);
		return instance;
	}

	void
	MeshFilterComponent::onMeshReplace(const model::MeshPtr& mesh) noexcept
	{
		for (auto& it : delegates_)
			(*it)(mesh);
	}
}