#include <octoon/mesh_filter_component.h>

namespace octoon
{
	OctoonImplementSubClass(MeshFilterComponent, GameComponent, "MeshFilter")

	MeshFilterComponent::MeshFilterComponent() noexcept
		: isSharedMesh_(false)
	{
	}

	MeshFilterComponent::MeshFilterComponent(model::Mesh&& mesh, bool sharedMesh) noexcept
	{
		this->setMesh(std::make_shared<model::Mesh>(std::move(mesh)), sharedMesh);
	}

	MeshFilterComponent::MeshFilterComponent(const model::Mesh& mesh, bool sharedMesh) noexcept
	{
		this->setMesh(std::make_shared<model::Mesh>(mesh), sharedMesh);
	}

	MeshFilterComponent::MeshFilterComponent(model::MeshPtr&& mesh, bool sharedMesh) noexcept
	{
		this->setMesh(std::move(mesh), sharedMesh);
	}

	MeshFilterComponent::MeshFilterComponent(const model::MeshPtr& mesh, bool sharedMesh) noexcept
	{
		this->setMesh(mesh, sharedMesh);
	}

	MeshFilterComponent::~MeshFilterComponent() noexcept
	{
	}

	void
	MeshFilterComponent::setMesh(model::Mesh&& mesh, bool sharedMesh) noexcept
	{
		this->setMesh(std::make_shared<model::Mesh>(std::move(mesh)));
	}

	void
	MeshFilterComponent::setMesh(model::MeshPtr&& mesh, bool sharedMesh) noexcept
	{
		if (mesh_ != mesh)
		{
			mesh_ = std::move(mesh);
			isSharedMesh_ = sharedMesh;
			this->onMeshReplace(mesh_);
		}
	}

	void
	MeshFilterComponent::setMesh(const model::MeshPtr& mesh, bool sharedMesh) noexcept
	{
		if (mesh_ != mesh)
		{
			mesh_ = mesh;
			isSharedMesh_ = sharedMesh;
			this->onMeshReplace(mesh_);
		}
	}

	const model::MeshPtr&
	MeshFilterComponent::getMesh() const noexcept
	{
		return mesh_;
	}

	bool
	MeshFilterComponent::isSharedMesh() const noexcept
	{
		return isSharedMesh_;
	}

	void
	MeshFilterComponent::uploadMeshData() noexcept
	{
		for (auto& it : delegates_)
			(*it)(mesh_);
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
		instance->setName(instance->getName());

		if (isSharedMesh_)
			instance->setMesh(mesh_, isSharedMesh_);
		else
			instance->setMesh(mesh_ ? mesh_->clone() : nullptr, isSharedMesh_);

		return instance;
	}

	void
	MeshFilterComponent::onMeshReplace(const model::MeshPtr& mesh) noexcept
	{
		for (auto& it : delegates_)
			(*it)(mesh);
	}
}