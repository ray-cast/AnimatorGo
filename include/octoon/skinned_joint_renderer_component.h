#ifndef OCTOON_SKINNED_JOINT_RENDERER_COMPONENT_H_
#define OCTOON_SKINNED_JOINT_RENDERER_COMPONENT_H_

#include <octoon/mesh_renderer_component.h>
#include <octoon/video/geometry.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh/mesh.h>

namespace octoon
{
	class OCTOON_EXPORT SkinnedJointRendererComponent final : public MeshRendererComponent
	{
		OctoonDeclareSubClass(SkinnedJointRendererComponent, RenderComponent)
	public:
		SkinnedJointRendererComponent() noexcept;
		SkinnedJointRendererComponent(material::MaterialPtr&& material) noexcept;
		SkinnedJointRendererComponent(const material::MaterialPtr& material) noexcept;
		virtual ~SkinnedJointRendererComponent() noexcept;

		void setTransforms(const GameObjects& transforms) noexcept;
		void setTransforms(GameObjects&& transforms) noexcept;
		const GameObjects& getTransforms() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onFixedUpdate() noexcept override;

		void onAnimationUpdate(const runtime::any& data) noexcept;

		void onPreRender(const video::Camera& camera) noexcept override;

	private:
		SkinnedJointRendererComponent(const SkinnedJointRendererComponent&) = delete;
		SkinnedJointRendererComponent& operator=(const SkinnedJointRendererComponent&) = delete;

	private:
		bool needUpdate_;
		GameObjects transforms_;
	};
}

#endif