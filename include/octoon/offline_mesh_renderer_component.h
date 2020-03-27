#ifndef OCTOON_OFFLINE_MESH_RENDERER_COMPONENT_H_
#define OCTOON_OFFLINE_MESH_RENDERER_COMPONENT_H_

#include <octoon/offline_render_component.h>
#include <octoon/mesh/mesh.h>
#include <octoon/material/material.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineMeshRendererComponent : public OfflineRenderComponent
	{
		OctoonDeclareSubClass(OfflineMeshRendererComponent, RenderComponent)
	public:
		OfflineMeshRendererComponent() noexcept;
		OfflineMeshRendererComponent(material::Materials&& material) noexcept;
		OfflineMeshRendererComponent(material::MaterialPtr&& material) noexcept;
		OfflineMeshRendererComponent(const material::Materials& material) noexcept;
		OfflineMeshRendererComponent(const material::MaterialPtr& material) noexcept;
		virtual ~OfflineMeshRendererComponent() noexcept;

		virtual void uploadMeshData(const mesh::MeshPtr& mesh) noexcept;
		virtual void uploadMaterialData(const material::Materials& material) noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void updateMaterial(bool force) noexcept;

	protected:
		virtual void onActivate() noexcept override;
		virtual void onDeactivate() noexcept override;

		virtual void onMoveAfter() noexcept override;

		virtual void onMeshReplace(const std::any& mesh) noexcept;
		virtual void onMaterialReplace(const material::Materials& material) noexcept override;

		virtual void onLayerChangeAfter() noexcept override;

		virtual void onPreRender() noexcept override;

	private:
		OfflineMeshRendererComponent(const OfflineMeshRendererComponent&) = delete;
		OfflineMeshRendererComponent& operator=(const OfflineMeshRendererComponent&) = delete;

	private:
		std::vector<void*> shapes_;
		std::vector<void*> materials_;
		std::vector<std::pair<void*, material::MaterialPtr>> materialBinds_;
		std::map<void*, std::int32_t> refCount_;
	};
}

#endif