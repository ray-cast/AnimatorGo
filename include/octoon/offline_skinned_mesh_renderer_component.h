#ifndef OCTOON_OFFLINE_SKINNED_MESH_RENDERER_COMPONENT_H_
#define OCTOON_OFFLINE_SKINNED_MESH_RENDERER_COMPONENT_H_

#include <octoon/offline_mesh_renderer_component.h>
#include <octoon/video/geometry.h>
#include <octoon/mesh_filter_component.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineSkinnedMeshRendererComponent final : public OfflineMeshRendererComponent
	{
		OctoonDeclareSubClass(OfflineSkinnedMeshRendererComponent, RenderComponent)
	public:
		OfflineSkinnedMeshRendererComponent() noexcept;
		OfflineSkinnedMeshRendererComponent(model::Materials&& material, GameObjects&& transforms) noexcept;
		OfflineSkinnedMeshRendererComponent(model::MaterialPtr&& material, GameObjects&& transforms) noexcept;
		OfflineSkinnedMeshRendererComponent(const model::Materials& material, const GameObjects& transforms) noexcept;
		OfflineSkinnedMeshRendererComponent(const model::MaterialPtr& material, const GameObjects& transforms) noexcept;
		virtual ~OfflineSkinnedMeshRendererComponent() noexcept;

		void setTransforms(const GameObjects& transforms) noexcept;
		void setTransforms(GameObjects&& transforms) noexcept;
		const GameObjects& getTransforms() const noexcept;

		void uploadMeshData(const model::Mesh& mesh) noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMeshReplace(const runtime::any& mesh) noexcept override;
		void onAnimationUpdate(const runtime::any& data) noexcept;

		void onPreRender() noexcept override;

		void onFixedUpdate() noexcept override;

	private:
		OfflineSkinnedMeshRendererComponent(const OfflineSkinnedMeshRendererComponent&) = delete;
		OfflineSkinnedMeshRendererComponent& operator=(const OfflineSkinnedMeshRendererComponent&) = delete;

	private:
		bool needUpdate_;
		GameObjects transforms_;

		model::MeshPtr mesh_;
		model::MeshPtr skinnedMesh_;
	};
}

#endif