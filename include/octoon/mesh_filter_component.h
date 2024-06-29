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
		MeshFilterComponent(Mesh&& mesh, bool sharedMesh = true) noexcept;
		MeshFilterComponent(MeshPtr&& mesh, bool sharedMesh = true) noexcept;
		MeshFilterComponent(const Mesh& mesh, bool sharedMesh = true) noexcept;
		MeshFilterComponent(const MeshPtr& mesh, bool sharedMesh = true) noexcept;
		virtual ~MeshFilterComponent() noexcept;

		void setMesh(Mesh&& mesh, bool sharedMesh = true) noexcept;
		void setMesh(MeshPtr&& mesh, bool sharedMesh = true) noexcept;
		void setMesh(const MeshPtr& mesh, bool sharedMesh = true) noexcept;
		const MeshPtr& getMesh() const noexcept;

		bool isSharedMesh() const noexcept;

		void uploadMeshData() noexcept;

		void load(const nlohmann::json& json) noexcept(false) override;
		void save(nlohmann::json& json) const noexcept(false) override;

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
		MeshPtr mesh_;
	};
}

#endif