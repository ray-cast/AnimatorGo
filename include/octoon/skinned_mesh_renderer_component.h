#ifndef OCTOON_SKINNED_MESH_RENDERER_COMPONENT_H_
#define OCTOON_SKINNED_MESH_RENDERER_COMPONENT_H_

#include <octoon/mesh_renderer_component.h>
#include <octoon/video/geometry.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/model/mesh.h>

namespace octoon
{
	class OCTOON_EXPORT SkinnedMeshRendererComponent final : public MeshRendererComponent
	{
		OctoonDeclareSubClass(SkinnedMeshRendererComponent, RenderComponent)
	public:
		SkinnedMeshRendererComponent() noexcept;
		SkinnedMeshRendererComponent(model::Materials&& materials, GameObjects&& transforms) noexcept;
		SkinnedMeshRendererComponent(model::MaterialPtr&& material, GameObjects&& transforms) noexcept;
		SkinnedMeshRendererComponent(const model::Materials& materials, const GameObjects& transforms) noexcept;
		SkinnedMeshRendererComponent(const model::MaterialPtr& material, const GameObjects& transforms) noexcept;
		virtual ~SkinnedMeshRendererComponent() noexcept;

		void setTransforms(const GameObjects& transforms) noexcept;
		void setTransforms(GameObjects&& transforms) noexcept;
		const GameObjects& getTransforms() const noexcept;

		void uploadMeshData(const model::Mesh& mesh) noexcept override;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onFixedUpdate() noexcept override;
		void onAnimationUpdate(const runtime::any& mesh) noexcept;

		void onMeshReplace(const runtime::any& mesh) noexcept;

		void onPreRender(const video::Camera& camera) noexcept override;

	private:
		SkinnedMeshRendererComponent(const SkinnedMeshRendererComponent&) = delete;
		SkinnedMeshRendererComponent& operator=(const SkinnedMeshRendererComponent&) = delete;

	private:
		bool needUpdate_;
		GameObjects transforms_;

		model::MeshPtr mesh_;
		model::MeshPtr skinnedMesh_;
	};
}

#endif