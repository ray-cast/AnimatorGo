#ifndef OCTOON_MESH_RENDERER_COMPONENT_H_
#define OCTOON_MESH_RENDERER_COMPONENT_H_

#include <octoon/render_component.h>
#include <octoon/video/geometry.h>

namespace octoon
{
	class OCTOON_EXPORT MeshRendererComponent : public RenderComponent
	{
		OctoonDeclareSubClass(MeshRendererComponent, RenderComponent)
	public:
		MeshRendererComponent() noexcept;
		explicit MeshRendererComponent(model::Materials&& materials) noexcept;
		explicit MeshRendererComponent(model::MaterialPtr&& material) noexcept;
		explicit MeshRendererComponent(const model::Materials& materials) noexcept;
		explicit MeshRendererComponent(const model::MaterialPtr& material) noexcept;
		virtual ~MeshRendererComponent() noexcept;

		virtual void uploadMeshData(const model::Mesh& mesh) noexcept;
		virtual void uploadMaterialData(const model::Materials& material) noexcept;

		GameComponentPtr clone() const noexcept override;

	protected:
		virtual void onActivate() noexcept override;
		virtual void onDeactivate() noexcept override;

		virtual void onMoveAfter() noexcept override;

		virtual void onMeshReplace(const runtime::any& mesh) noexcept;
		virtual void onMaterialReplace(const model::Materials& materials) noexcept override;

		virtual void onLayerChangeAfter() noexcept override;

	private:
		MeshRendererComponent(const MeshRendererComponent&) = delete;
		MeshRendererComponent& operator=(const MeshRendererComponent&) = delete;

	protected:
		video::Geometrys geometries_;
		video::Materials materials_;
	};
}

#endif