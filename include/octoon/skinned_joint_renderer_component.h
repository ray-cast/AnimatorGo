#ifndef OCTOON_SKINNED_JOINT_RENDERER_COMPONENT_H_
#define OCTOON_SKINNED_JOINT_RENDERER_COMPONENT_H_

#include <octoon/render_component.h>
#include <octoon/video/geometry.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/model/mesh.h>

namespace octoon
{
	class OCTOON_EXPORT SkinnedJointRendererComponent final : public RenderComponent
	{
		OctoonDeclareSubClass(SkinnedJointRendererComponent, RenderComponent)
	public:
		SkinnedJointRendererComponent() noexcept;
		SkinnedJointRendererComponent(video::MaterialPtr&& material) noexcept;
		SkinnedJointRendererComponent(const video::MaterialPtr& material) noexcept;
		virtual ~SkinnedJointRendererComponent() noexcept;

		void setTransforms(const GameObjects& transforms) noexcept;
		void setTransforms(GameObjects&& transforms) noexcept;
		const GameObjects& getTransforms() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveBefore() noexcept override;
		void onMoveAfter() noexcept override;

		void onFixedUpdate() noexcept override;
		void onFrame() noexcept override;
		void onFrameEnd() noexcept override;

		void onJointReplace(const runtime::any& mesh) noexcept;
		void onAnimationUpdate(const runtime::any& data) noexcept;
		void onMaterialReplace(const video::MaterialPtr& material) noexcept override;

		void onLayerChangeAfter() noexcept override;
	
	private:
		void uploadJointData(const model::Mesh& mesh) noexcept;

	private:
		SkinnedJointRendererComponent(const SkinnedJointRendererComponent&) = delete;
		SkinnedJointRendererComponent& operator=(const SkinnedJointRendererComponent&) = delete;

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