#ifndef OCTOON_MESH_FILTER_COMPONENT_H_
#define OCTOON_MESH_FILTER_COMPONENT_H_

#include <octoon/video/mesh.h>
#include <octoon/game_component.h>
#include <functional>

namespace octoon
{
	class OCTOON_EXPORT MeshFilter : public GameComponent
	{
		OctoonDeclareSubClass(MeshFilter, RenderComponent)
	public:
		typedef std::function<void(const video::MeshPtr&)> OnMeshReplaceEvent;
		typedef std::vector<OnMeshReplaceEvent*> OnMeshReplaceEvents;

	public:
		MeshFilter() noexcept;
		MeshFilter(video::MeshPtr&& mesh) noexcept;
		MeshFilter(const video::MeshPtr& mesh) noexcept;
		virtual ~MeshFilter() noexcept;

		void setMesh(video::MeshPtr&& mesh) noexcept;
		void setMesh(const video::MeshPtr& mesh) noexcept;
		const video::MeshPtr& getMesh() const noexcept;

		void addMeshListener(OnMeshReplaceEvent* func) noexcept;
		void removeMeshListener(const OnMeshReplaceEvent* func) noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void onMeshReplace(const video::MeshPtr& mesh) noexcept;

	private:
		MeshFilter(const MeshFilter&) = delete;
		MeshFilter& operator=(const MeshFilter&) = delete;

	private:
		video::MeshPtr mesh_;
		OnMeshReplaceEvents delegates_;
	};
}

#endif