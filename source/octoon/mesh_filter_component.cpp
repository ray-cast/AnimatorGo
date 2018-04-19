#include <octoon/mesh_filter_component.h>

namespace octoon
{
	OctoonImplementSubClass(MeshFilter, GameComponent, "MeshFilter")

	MeshFilter::MeshFilter() noexcept
	{
	}

	MeshFilter::MeshFilter(video::MeshPtr&& mesh) noexcept
	{
		this->setMesh(std::move(mesh));
	}

	MeshFilter::MeshFilter(const video::MeshPtr& mesh) noexcept
	{
		this->setMesh(mesh);
	}

	MeshFilter::~MeshFilter() noexcept
	{
	}

	void
	MeshFilter::setMesh(video::MeshPtr&& mesh) noexcept
	{
		if (mesh_ != mesh)
		{
			this->onMeshReplace(mesh);
			mesh_ = std::move(mesh);
		}
	}

	void
	MeshFilter::setMesh(const video::MeshPtr& mesh) noexcept
	{
		if (mesh_ != mesh)
		{
			this->onMeshReplace(mesh);
			mesh_ = mesh;
		}
	}

	const video::MeshPtr&
	MeshFilter::getMesh() const noexcept
	{
		return mesh_;
	}

	void
	MeshFilter::addMeshListener(OnMeshReplaceEvent* func) noexcept
	{
		delegates_.push_back(func);
	}

	void
	MeshFilter::removeMeshListener(const OnMeshReplaceEvent* func) noexcept
	{
		auto it = std::find(delegates_.begin(), delegates_.end(), func);
		if (it != delegates_.end())
			delegates_.erase(it);
	}

	GameComponentPtr
	MeshFilter::clone() const noexcept
	{
		auto instance = std::make_shared<MeshFilter>();
		instance->setMesh(mesh_ ? mesh_->clone() : nullptr);
		return instance;
	}

	void
	MeshFilter::onMeshReplace(const video::MeshPtr& mesh) noexcept
	{
		for (auto& it : delegates_)
			(*it)(mesh);
	}
}