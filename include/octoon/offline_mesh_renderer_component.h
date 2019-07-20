#ifndef OCTOON_OFFLINE_MESH_RENDERER_COMPONENT_H_
#define OCTOON_OFFLINE_MESH_RENDERER_COMPONENT_H_

#include <octoon/offline_render_component.h>
#include <octoon/video/geometry.h>
#include <octoon/mesh_filter_component.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineMeshRendererComponent final : public OfflineRenderComponent
	{
		OctoonDeclareSubClass(OfflineMeshRendererComponent, RenderComponent)
	public:
		OfflineMeshRendererComponent() noexcept;
		OfflineMeshRendererComponent(video::MaterialPtr&& material) noexcept;
		OfflineMeshRendererComponent(const video::MaterialPtr& material) noexcept;
		virtual ~OfflineMeshRendererComponent() noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveBefore() noexcept override;
		void onMoveAfter() noexcept override;

		void onMeshReplace(const runtime::any& mesh) noexcept;
		void onMaterialReplace(const video::MaterialPtr& material) noexcept override;

		void onLayerChangeAfter() noexcept override;

	protected:
		void uploadMeshData(const model::Mesh& mesh) noexcept;

	private:
		OfflineMeshRendererComponent(const OfflineMeshRendererComponent&) = delete;
		OfflineMeshRendererComponent& operator=(const OfflineMeshRendererComponent&) = delete;

	private:
		void* rprShape_;
		void* rprMaterial_;
	};
}

#endif