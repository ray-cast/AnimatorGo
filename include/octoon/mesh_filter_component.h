#ifndef OCTOON_MESH_FILTER_COMPONENT_H_
#define OCTOON_MESH_FILTER_COMPONENT_H_

#include <octoon/mesh/mesh.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT MeshFilterComponent final : public GameComponent
	{
		OctoonDeclareSubClass(MeshFilterComponent, GameComponent)
	public:
		MeshFilterComponent() noexcept;
		MeshFilterComponent(mesh::Mesh&& mesh, bool sharedMesh = false) noexcept;
		MeshFilterComponent(mesh::MeshPtr&& mesh, bool sharedMesh = false) noexcept;
		MeshFilterComponent(const mesh::Mesh& mesh, bool sharedMesh = false) noexcept;
		MeshFilterComponent(const mesh::MeshPtr& mesh, bool sharedMesh = false) noexcept;
		virtual ~MeshFilterComponent() noexcept;

		void setMesh(mesh::Mesh&& mesh, bool sharedMesh = false) noexcept;
		void setMesh(mesh::MeshPtr&& mesh, bool sharedMesh = false) noexcept;
		void setMesh(const mesh::MeshPtr& mesh, bool sharedMesh = false) noexcept;
		const mesh::MeshPtr& getMesh() const noexcept;

		bool isSharedMesh() const noexcept;

		void uploadMeshData() noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onMeshReplace() noexcept;

	private:
		MeshFilterComponent(const MeshFilterComponent&) = delete;
		MeshFilterComponent& operator=(const MeshFilterComponent&) = delete;

	private:
		bool isSharedMesh_;
		mesh::MeshPtr mesh_;
	};
}

#endif