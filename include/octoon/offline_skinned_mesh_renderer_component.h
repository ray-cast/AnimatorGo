#ifndef OCTOON_OFFLINE_SKINNED_MESH_RENDERER_COMPONENT_H_
#define OCTOON_OFFLINE_SKINNED_MESH_RENDERER_COMPONENT_H_

#include <octoon/offline_render_component.h>
#include <octoon/video/geometry.h>
#include <octoon/mesh_filter_component.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineSkinnedMeshRendererComponent final : public OfflineRenderComponent
	{
		OctoonDeclareSubClass(OfflineSkinnedMeshRendererComponent, RenderComponent)
	public:
		OfflineSkinnedMeshRendererComponent() noexcept;
		explicit OfflineSkinnedMeshRendererComponent(video::MaterialPtr&& material) noexcept;
		explicit OfflineSkinnedMeshRendererComponent(const video::MaterialPtr& material) noexcept;
		OfflineSkinnedMeshRendererComponent(video::MaterialPtr&& material, GameObjects&& transforms) noexcept;
		OfflineSkinnedMeshRendererComponent(const video::MaterialPtr& material, const GameObjects& transforms) noexcept;
		virtual ~OfflineSkinnedMeshRendererComponent() noexcept;

		void setTransforms(const GameObjects& transforms) noexcept;
		void setTransforms(GameObjects&& transforms) noexcept;
		const GameObjects& getTransforms() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveAfter() noexcept override;

		void onMeshReplace(const runtime::any& mesh) noexcept;
		void onMaterialReplace(const video::MaterialPtr& material) noexcept override;

		void onAnimationUpdate(const runtime::any& data) noexcept;

		void onLayerChangeAfter() noexcept override;

		void onPreRender() noexcept override;

		void onFixedUpdate() noexcept override;

	protected:
		void uploadBoneData() noexcept;
		void uploadMatData(const video::Material& mat) noexcept;
		void uploadMeshData(const model::Mesh& mesh) noexcept;

	private:
		OfflineSkinnedMeshRendererComponent(const OfflineSkinnedMeshRendererComponent&) = delete;
		OfflineSkinnedMeshRendererComponent& operator=(const OfflineSkinnedMeshRendererComponent&) = delete;

	private:
		bool needUpdate_;

		void* rprShape_;
		void* rprMaterial_;

		GameObjects transforms_;
		math::BoundingBox boundingBox_;
		model::MeshPtr mesh_;
		model::MeshPtr skinnedMesh_;
	};
}

#endif