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
			if (mesh_->getBoundingBox().empty())
				mesh_->computeBoundingBox();

			isSharedMesh_ = sharedMesh;
			this->uploadMeshData();
		}
	}

	void
	MeshFilterComponent::setMesh(const model::MeshPtr& mesh, bool sharedMesh) noexcept
	{
		if (mesh_ != mesh)
		{
			mesh_ = mesh;
			if (mesh_->getBoundingBox().empty())
				mesh_->computeBoundingBox();

			isSharedMesh_ = sharedMesh;
			this->uploadMeshData();
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
		this->onMeshReplace();
	}

	GameComponentPtr
	MeshFilterComponent::clone() const noexcept
	{
		auto instance = std::make_shared<MeshFilterComponent>();
		instance->setName(instance->getName());
		instance->setMesh(mesh_ ? (isSharedMesh_ ? mesh_ : mesh_->clone()) : nullptr, isSharedMesh_);

		return instance;
	}

	void
	MeshFilterComponent::onActivate() except
	{
		this->addMessageListener("octoon:mesh:get", std::bind(&MeshFilterComponent::uploadMeshData, this));
		this->uploadMeshData();
	}

	void
	MeshFilterComponent::onDeactivate() noexcept
	{
	}

	void
	MeshFilterComponent::onMeshReplace() noexcept
	{
		this->trySendMessage("octoon:mesh:update", mesh_);
	}
}