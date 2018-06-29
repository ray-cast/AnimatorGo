#ifndef OCTOON_MESH_FILTER_COMPONENT_H_
#define OCTOON_MESH_FILTER_COMPONENT_H_

#include <octoon/model/mesh.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT MeshFilterComponent : public GameComponent
	{
		OctoonDeclareSubClass(MeshFilterComponent, GameComponent)
	public:
		MeshFilterComponent() noexcept;
		MeshFilterComponent(model::Mesh&& mesh, bool sharedMesh = false) noexcept;
		MeshFilterComponent(model::MeshPtr&& mesh, bool sharedMesh = false) noexcept;
		MeshFilterComponent(const model::Mesh& mesh, bool sharedMesh = false) noexcept;
		MeshFilterComponent(const model::MeshPtr& mesh, bool sharedMesh = false) noexcept;
		virtual ~MeshFilterComponent() noexcept;

		void setMesh(model::Mesh&& mesh, bool sharedMesh = false) noexcept;
		void setMesh(model::MeshPtr&& mesh, bool sharedMesh = false) noexcept;
		void setMesh(const model::MeshPtr& mesh, bool sharedMesh = false) noexcept;
		const model::MeshPtr& getMesh() const noexcept;

		bool isSharedMesh() const noexcept;

		void uploadMeshData() noexcept;
		
		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void onMeshReplace(const model::MeshPtr& mesh) noexcept;

	private:
		MeshFilterComponent(const MeshFilterComponent&) = delete;
		MeshFilterComponent& operator=(const MeshFilterComponent&) = delete;

	private:
		bool isSharedMesh_;
		model::MeshPtr mesh_;
	};
}

#endif