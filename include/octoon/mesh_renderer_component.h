#ifndef OCTOON_MESH_RENDERER_COMPONENT_H_
#define OCTOON_MESH_RENDERER_COMPONENT_H_

#include <octoon/render_component.h>
#include <octoon/video/geometry.h>
#include <octoon/mesh_filter_component.h>

namespace octoon
{
	class OCTOON_EXPORT MeshRendererComponent final : public RenderComponent
	{
		OctoonDeclareSubClass(MeshRendererComponent, RenderComponent)
	public:
		MeshRendererComponent() noexcept;
		MeshRendererComponent(video::MaterialPtr&& material) noexcept;
		MeshRendererComponent(const video::MaterialPtr& material) noexcept;
		virtual ~MeshRendererComponent() noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onAttachComponent(const GameComponentPtr& component) noexcept override;
		void onDetachComponent(const GameComponentPtr& component) noexcept override;

		void onMoveBefore() noexcept override;
		void onMoveAfter() noexcept override;

		void onMeshReplace(const video::MeshPtr& mesh) noexcept;
		void onMaterialReplace(const video::MaterialPtr& material) noexcept override;

	private:
		MeshRendererComponent(const MeshRendererComponent&) = delete;
		MeshRendererComponent& operator=(const MeshRendererComponent&) = delete;

	private:
		MeshFilter::OnMeshReplaceEvent onMeshReplaceEvent_;
		video::GeometryPtr geometry_;
	};
}

#endif