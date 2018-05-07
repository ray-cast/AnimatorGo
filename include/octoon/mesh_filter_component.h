#ifndef OCTOON_MESH_FILTER_COMPONENT_H_
#define OCTOON_MESH_FILTER_COMPONENT_H_

#include <octoon/model/mesh.h>
#include <octoon/game_component.h>
#include <functional>

namespace octoon
{
	class OCTOON_EXPORT MeshFilterComponent : public GameComponent
	{
		OctoonDeclareSubClass(MeshFilterComponent, GameComponent)
	public:
		typedef std::function<void(const model::MeshPtr&)> OnMeshReplaceEvent;
		typedef std::vector<OnMeshReplaceEvent*> OnMeshReplaceEvents;

	public:
		MeshFilterComponent() noexcept;
		MeshFilterComponent(model::Mesh&& mesh) noexcept;
		MeshFilterComponent(model::MeshPtr&& mesh) noexcept;
		MeshFilterComponent(const model::Mesh& mesh) noexcept;
		MeshFilterComponent(const model::MeshPtr& mesh) noexcept;
		virtual ~MeshFilterComponent() noexcept;

		void setMesh(model::Mesh&& mesh) noexcept;
		void setMesh(model::MeshPtr&& mesh) noexcept;
		void setMesh(const model::MeshPtr& mesh) noexcept;
		const model::MeshPtr& getMesh() const noexcept;

		void setSharedMesh(model::Mesh&& mesh) noexcept;
		void setSharedMesh(model::MeshPtr&& mesh) noexcept;
		void setSharedMesh(const model::MeshPtr& mesh) noexcept;
		const model::MeshPtr& getSharedMesh() const noexcept;

		void uploadMeshData() noexcept;

		void addMeshListener(OnMeshReplaceEvent* func) noexcept;
		void removeMeshListener(const OnMeshReplaceEvent* func) noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void onMeshReplace(const model::MeshPtr& mesh) noexcept;
		virtual void onSharedMeshReplace(const model::MeshPtr& mesh) noexcept;

	private:
		MeshFilterComponent(const MeshFilterComponent&) = delete;
		MeshFilterComponent& operator=(const MeshFilterComponent&) = delete;

	private:
		model::MeshPtr mesh_;
		model::MeshPtr sharedMesh_;
		OnMeshReplaceEvents delegates_;
		OnMeshReplaceEvents sharedDelegates_;
	};
}

#endif