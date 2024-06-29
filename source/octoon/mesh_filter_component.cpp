#include <octoon/mesh_filter_component.h>
#include <octoon/asset_database.h>
#include <octoon/asset_importer.h>

namespace octoon
{
	OctoonImplementSubClass(MeshFilterComponent, GameComponent, "MeshFilter")

	MeshFilterComponent::MeshFilterComponent() noexcept
		: isSharedMesh_(false)
	{
	}

	MeshFilterComponent::MeshFilterComponent(Mesh&& mesh, bool sharedMesh) noexcept
	{
		this->setMesh(std::make_shared<Mesh>(std::move(mesh)), sharedMesh);
	}

	MeshFilterComponent::MeshFilterComponent(const Mesh& mesh, bool sharedMesh) noexcept
	{
		this->setMesh(std::make_shared<Mesh>(mesh), sharedMesh);
	}

	MeshFilterComponent::MeshFilterComponent(MeshPtr&& mesh, bool sharedMesh) noexcept
	{
		this->setMesh(std::move(mesh), sharedMesh);
	}

	MeshFilterComponent::MeshFilterComponent(const MeshPtr& mesh, bool sharedMesh) noexcept
	{
		this->setMesh(mesh, sharedMesh);
	}

	MeshFilterComponent::~MeshFilterComponent() noexcept
	{
	}

	void
	MeshFilterComponent::setMesh(Mesh&& mesh, bool sharedMesh) noexcept
	{
		this->setMesh(std::make_shared<Mesh>(std::move(mesh)));
	}

	void
	MeshFilterComponent::setMesh(MeshPtr&& mesh, bool sharedMesh) noexcept
	{
		if (mesh_ != mesh)
		{
			mesh_ = std::move(mesh);
			if (mesh_->getBoundingBoxAll().empty())
				mesh_->computeBoundingBox();

			isSharedMesh_ = sharedMesh;
			this->uploadMeshData();
		}
	}

	void
	MeshFilterComponent::setMesh(const MeshPtr& mesh, bool sharedMesh) noexcept
	{
		if (mesh_ != mesh)
		{
			mesh_ = mesh;
			if (mesh_->getBoundingBoxAll().empty())
				mesh_->computeBoundingBox();

			isSharedMesh_ = sharedMesh;
			this->uploadMeshData();
		}
	}

	const MeshPtr&
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

	void
	MeshFilterComponent::load(const nlohmann::json& json) noexcept(false)
	{
		GameComponent::load(json);

		if (json.contains("mesh"))
		{
			auto& mesh = json["mesh"];

			auto guid = mesh["guid"].get<std::string>();
			auto localId = mesh["localId"].get<std::int64_t>();
			this->setMesh(AssetDatabase::instance()->loadAsset<Mesh>(guid, localId));
		}
	}

	void
	MeshFilterComponent::save(nlohmann::json& json) const noexcept(false)
	{
		GameComponent::save(json);

		std::string guid;
		std::int64_t localId;

		if (AssetDatabase::instance()->getGUIDAndLocalIdentifier(this->mesh_, guid, localId))
		{
			nlohmann::json mesh;
			mesh["guid"] = guid;
			mesh["localId"] = localId;

			json["mesh"] = std::move(mesh);
		}
	}

	GameComponentPtr
	MeshFilterComponent::clone() const noexcept
	{
		auto instance = std::make_shared<MeshFilterComponent>();
		instance->setName(this->getName());
		instance->setMesh(this->mesh_ ? (this->isSharedMesh_ ? mesh_ : mesh_->clone()) : nullptr, this->isSharedMesh_);

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