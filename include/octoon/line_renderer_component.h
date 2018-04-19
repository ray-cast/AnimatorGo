#ifndef OCTOON_LINE_RENDERER_COMPONENT_H_
#define OCTOON_LINE_RENDERER_COMPONENT_H_

#include <octoon/render_component.h>
#include <octoon/video/geometry.h>
#include <octoon/mesh_filter_component.h>

namespace octoon
{
	class OCTOON_EXPORT LineRendererComponent final : public RenderComponent
	{
		OctoonDeclareSubClass(LineRendererComponent, RenderComponent)
	public:
		LineRendererComponent() noexcept;
		LineRendererComponent(video::MaterialPtr&& material) noexcept;
		LineRendererComponent(const video::MaterialPtr& material) noexcept;
		virtual ~LineRendererComponent() noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onAttachComponent(const GameComponentPtr& component) noexcept override;
		void onDetachComponent(const GameComponentPtr& component) noexcept override;

		void onMoveBefore() noexcept override;
		void onMoveAfter() noexcept override;

		void onMeshReplace(const video::MeshPtr& mesh) noexcept;
		void onMaterialReplace(const video::MaterialPtr& material) noexcept;

	private:
		LineRendererComponent(const LineRendererComponent&) = delete;
		LineRendererComponent& operator=(const LineRendererComponent&) = delete;

	private:
		MeshFilter::OnMeshReplaceEvent onMeshReplaceEvent_;
		video::GeometryPtr geometry_;
	};
}

#endif