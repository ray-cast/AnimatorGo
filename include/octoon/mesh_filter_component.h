#ifndef OCTOON_MESH_FILTER_COMPONENT_H_
#define OCTOON_MESH_FILTER_COMPONENT_H_

#include <octoon/video/mesh.h>
#include <octoon/game_component.h>
#include <functional>

namespace octoon
{
	class OCTOON_EXPORT MeshFilterComponent : public GameComponent
	{
		OctoonDeclareSubClass(MeshFilterComponent, GameComponent)
	public:
		typedef std::function<void(const video::MeshPtr&)> OnMeshReplaceEvent;
		typedef std::vector<OnMeshReplaceEvent*> OnMeshReplaceEvents;

	public:
		MeshFilterComponent() noexcept;
		MeshFilterComponent(video::MeshPtr&& mesh) noexcept;
		MeshFilterComponent(const video::MeshPtr& mesh) noexcept;
		virtual ~MeshFilterComponent() noexcept;

		void setMesh(video::MeshPtr&& mesh) noexcept;
		void setMesh(const video::MeshPtr& mesh) noexcept;
		const video::MeshPtr& getMesh() const noexcept;

		void addMeshListener(OnMeshReplaceEvent* func) noexcept;
		void removeMeshListener(const OnMeshReplaceEvent* func) noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void onMeshReplace(const video::MeshPtr& mesh) noexcept;

	private:
		MeshFilterComponent(const MeshFilterComponent&) = delete;
		MeshFilterComponent& operator=(const MeshFilterComponent&) = delete;

	private:
		video::MeshPtr mesh_;
		OnMeshReplaceEvents delegates_;
	};
}

#endif