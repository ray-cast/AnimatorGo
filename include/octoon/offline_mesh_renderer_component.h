#ifndef OCTOON_OFFLINE_MESH_RENDERER_COMPONENT_H_
#define OCTOON_OFFLINE_MESH_RENDERER_COMPONENT_H_

#include <octoon/offline_render_component.h>
#include <octoon/video/geometry.h>
#include <octoon/mesh_filter_component.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineMeshRendererComponent : public OfflineRenderComponent
	{
		OctoonDeclareSubClass(OfflineMeshRendererComponent, RenderComponent)
	public:
		OfflineMeshRendererComponent() noexcept;
		OfflineMeshRendererComponent(video::MaterialPtr&& material) noexcept;
		OfflineMeshRendererComponent(const video::MaterialPtr& material) noexcept;
		virtual ~OfflineMeshRendererComponent() noexcept;

		virtual void uploadMeshData(const model::Mesh& mesh) noexcept;

		GameComponentPtr clone() const noexcept override;

	protected:
		virtual void onActivate() noexcept override;
		virtual void onDeactivate() noexcept override;

		virtual void onMoveBefore() noexcept override;
		virtual void onMoveAfter() noexcept override;

		virtual void onMeshReplace(const runtime::any& mesh) noexcept;
		virtual void onMaterialReplace(const video::MaterialPtr& material) noexcept override;

		virtual void onLayerChangeAfter() noexcept override;

	private:
		void uploadMatData(const video::Material& mat) noexcept;

	private:
		OfflineMeshRendererComponent(const OfflineMeshRendererComponent&) = delete;
		OfflineMeshRendererComponent& operator=(const OfflineMeshRendererComponent&) = delete;

	private:
		void* rprShape_;
		void* rprMaterial_;
	};
}

#endif