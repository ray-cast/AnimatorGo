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
		OfflineMeshRendererComponent(model::Materials&& material) noexcept;
		OfflineMeshRendererComponent(model::MaterialPtr&& material) noexcept;
		OfflineMeshRendererComponent(const model::Materials& material) noexcept;
		OfflineMeshRendererComponent(const model::MaterialPtr& material) noexcept;
		virtual ~OfflineMeshRendererComponent() noexcept;

		virtual void uploadMeshData(const model::Mesh& mesh) noexcept;
		virtual void uploadMaterialData(const model::Materials& material) noexcept;

		GameComponentPtr clone() const noexcept override;

	protected:
		virtual void onActivate() noexcept override;
		virtual void onDeactivate() noexcept override;

		virtual void onMoveAfter() noexcept override;

		virtual void onMeshReplace(const runtime::any& mesh) noexcept;
		virtual void onMaterialReplace(const model::Materials& material) noexcept override;

		virtual void onLayerChangeAfter() noexcept override;

	private:
		void* createImage(const std::string& path) noexcept;

	private:
		OfflineMeshRendererComponent(const OfflineMeshRendererComponent&) = delete;
		OfflineMeshRendererComponent& operator=(const OfflineMeshRendererComponent&) = delete;

	private:
		std::vector<void*> shapes_;
		std::vector<void*> materials_;
	};
}

#endif