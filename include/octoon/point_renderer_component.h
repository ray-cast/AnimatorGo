#ifndef OCTOON_POINT_RENDERER_COMPONENT_H_
#define OCTOON_POINT_RENDERER_COMPONENT_H_

#include <octoon/render_component.h>
#include <octoon/video/geometry.h>
#include <octoon/mesh_filter_component.h>

namespace octoon
{
	class OCTOON_EXPORT PointRendererComponent final : public RenderComponent
	{
		OctoonDeclareSubClass(PointRendererComponent, RenderComponent)
	public:
		PointRendererComponent() noexcept;
		PointRendererComponent(video::MaterialPtr&& material) noexcept;
		PointRendererComponent(const video::MaterialPtr& material) noexcept;
		virtual ~PointRendererComponent() noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onAttachComponent(const GameComponentPtr& component) noexcept override;
		void onDetachComponent(const GameComponentPtr& component) noexcept override;

		void onMoveBefore() noexcept override;
		void onMoveAfter() noexcept override;

		void onMeshReplace(const model::MeshPtr& mesh) noexcept;
		void onMaterialReplace(const video::MaterialPtr& material) noexcept;

	private:
		PointRendererComponent(const PointRendererComponent&) = delete;
		PointRendererComponent& operator=(const PointRendererComponent&) = delete;

	private:
		MeshFilterComponent::OnMeshReplaceEvent onMeshReplaceEvent_;
		video::GeometryPtr geometry_;
	};
}

#endif