#ifndef OCTOON_MESH_RENDERER_COMPONENT_H_
#define OCTOON_MESH_RENDERER_COMPONENT_H_

#include <octoon/render_component.h>
#include <octoon/geometry/geometry.h>

namespace octoon
{
	class OCTOON_EXPORT MeshRendererComponent : public RenderComponent
	{
		OctoonDeclareSubClass(MeshRendererComponent, RenderComponent)
	public:
		MeshRendererComponent() noexcept;
		explicit MeshRendererComponent(material::Materials&& materials) noexcept;
		explicit MeshRendererComponent(material::MaterialPtr&& material) noexcept;
		explicit MeshRendererComponent(const material::Materials& materials) noexcept;
		explicit MeshRendererComponent(const material::MaterialPtr& material) noexcept;
		virtual ~MeshRendererComponent() noexcept;

		virtual void setVisible(bool visable) noexcept override;
		virtual bool getVisible() const noexcept override;

		virtual void setRenderOrder(std::int32_t order) noexcept override;
		virtual std::int32_t getRenderOrder() const noexcept override;

		virtual void uploadMeshData(const mesh::MeshPtr& mesh) noexcept;
		virtual void uploadMaterialData(const material::Materials& material) noexcept;

		GameComponentPtr clone() const noexcept override;

	protected:
		virtual void onActivate() noexcept override;
		virtual void onDeactivate() noexcept override;

		virtual void onMoveAfter() noexcept override;

		virtual void onMeshReplace(const std::any& mesh) noexcept;
		virtual void onMaterialReplace(const material::Materials& materials) noexcept override;

		virtual void onLayerChangeAfter() noexcept override;

	private:
		MeshRendererComponent(const MeshRendererComponent&) = delete;
		MeshRendererComponent& operator=(const MeshRendererComponent&) = delete;

	private:
		bool visible_;

		std::int32_t renderOrder_;

		std::vector<std::shared_ptr<geometry::Geometry>> geometries_;
	};
}

#endif