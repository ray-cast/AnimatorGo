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
		explicit MeshRendererComponent(Materials&& materials) noexcept;
		explicit MeshRendererComponent(MaterialPtr&& material) noexcept;
		explicit MeshRendererComponent(const Materials& materials) noexcept;
		explicit MeshRendererComponent(const MaterialPtr& material) noexcept;
		virtual ~MeshRendererComponent() noexcept;

		virtual void setVisible(bool visable) noexcept override;
		virtual bool getVisible() const noexcept override;

		virtual void setRendererPriority(std::int32_t order) noexcept override;
		virtual std::int32_t getRendererPriority() const noexcept override;

		virtual void setGlobalIllumination(bool enable) noexcept;
		virtual bool getGlobalIllumination() const noexcept;

		virtual void uploadMeshData(const MeshPtr& mesh) noexcept;
		virtual void uploadMaterialData(const Materials& material) noexcept;

		virtual void load(const nlohmann::json& json) noexcept(false) override;
		virtual void save(nlohmann::json& json) const noexcept(false) override;

		virtual GameComponentPtr clone() const noexcept override;

	protected:
		virtual void onActivate() noexcept override;
		virtual void onDeactivate() noexcept override;

		virtual void onMoveAfter() noexcept override;

		virtual void onMeshReplace(const std::any& mesh) noexcept;
		virtual void onMaterialReplace(const Materials& materials) noexcept override;

		virtual void onLayerChangeAfter() noexcept override;

	private:
		MeshRendererComponent(const MeshRendererComponent&) = delete;
		MeshRendererComponent& operator=(const MeshRendererComponent&) = delete;

	private:
		bool visible_;
		bool globalIllumination_;

		std::int32_t rendererPriority_;
		std::shared_ptr<Geometry> geometry_;
	};
}

#endif