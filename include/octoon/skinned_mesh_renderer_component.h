#ifndef OCTOON_SKINNED_MESH_RENDERER_COMPONENT_H_
#define OCTOON_SKINNED_MESH_RENDERER_COMPONENT_H_

#include <octoon/render_component.h>
#include <octoon/video/geometry.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/model/mesh.h>

namespace octoon
{
	class OCTOON_EXPORT SkinnedMeshRendererComponent final : public RenderComponent
	{
		OctoonDeclareSubClass(SkinnedMeshRendererComponent, RenderComponent)
	public:
		SkinnedMeshRendererComponent() noexcept;
		SkinnedMeshRendererComponent(video::MaterialPtr&& material) noexcept;
		SkinnedMeshRendererComponent(const video::MaterialPtr& material) noexcept;
		virtual ~SkinnedMeshRendererComponent() noexcept;

		void setTransforms(const GameObjects& transforms) noexcept;
		void setTransforms(GameObjects&& transforms) noexcept;
		const GameObjects& getTransforms() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveBefore() noexcept override;
		void onMoveAfter() noexcept override;

		void onFrameBegin() noexcept override;
		void onFrame() noexcept override;
		void onFrameEnd() noexcept override;

		void onMeshReplace(const runtime::any& mesh) noexcept;
		void onAnimationUpdate(const runtime::any& data) noexcept;
		void onMaterialReplace(const video::MaterialPtr& material) noexcept override;

		void onLayerChangeAfter() noexcept override;
	
	private:
		void uploadMeshData(const model::Mesh& mesh) noexcept;

	private:
		SkinnedMeshRendererComponent(const SkinnedMeshRendererComponent&) = delete;
		SkinnedMeshRendererComponent& operator=(const SkinnedMeshRendererComponent&) = delete;

	private:
		bool needUpdate_;

		GameObjects transforms_;
		math::BoundingBox boundingBox_;
		model::MeshPtr mesh_;
		model::MeshPtr skinnedMesh_;
		video::GeometryPtr geometry_;
	};
}

#endif