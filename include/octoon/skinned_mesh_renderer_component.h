#ifndef OCTOON_SKINNED_MESH_RENDERER_COMPONENT_H_
#define OCTOON_SKINNED_MESH_RENDERER_COMPONENT_H_

#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/skinned_component.h>
#include <octoon/cloth_component.h>

namespace octoon
{
	class OCTOON_EXPORT SkinnedMeshRendererComponent final : public MeshRendererComponent
	{
		OctoonDeclareSubClass(SkinnedMeshRendererComponent, MeshRendererComponent)
	public:
		SkinnedMeshRendererComponent() noexcept;
		SkinnedMeshRendererComponent(material::Materials&& materials, GameObjects&& transforms) noexcept;
		SkinnedMeshRendererComponent(material::MaterialPtr&& material, GameObjects&& transforms) noexcept;
		SkinnedMeshRendererComponent(const material::Materials& materials, const GameObjects& transforms) noexcept;
		SkinnedMeshRendererComponent(const material::MaterialPtr& material, const GameObjects& transforms) noexcept;
		virtual ~SkinnedMeshRendererComponent() noexcept;

		void setTransforms(const GameObjects& transforms) noexcept;
		void setTransforms(GameObjects&& transforms) noexcept;
		const GameObjects& getTransforms() const noexcept;

		void setClothBlendEnable(bool enable) noexcept;
		bool getClothBlendEnable() const noexcept;

		void setMorphBlendEnable(bool enable) noexcept;
		bool getMorphBlendEnable() const noexcept;

		void setTextureBlendEnable(bool enable) noexcept;
		bool getTextureBlendEnable() const noexcept;

		const mesh::MeshPtr& getSkinnedMesh() const noexcept;

		void uploadMeshData(const mesh::MeshPtr& mesh) noexcept override;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onFixedUpdate() noexcept override;
		void onAnimationUpdate(const std::any& mesh) noexcept;

		void onAttachComponent(const GameComponentPtr& component) noexcept override;
		void onDetachComponent(const GameComponentPtr& component) noexcept override;

		void onMeshReplace(const std::any& mesh) noexcept;

		void onPreRender(const camera::Camera& camera) noexcept override;

	private:
		void updateMeshData() noexcept;
		void updateJointData() noexcept;
		void updateBoneData() noexcept;
		void updateClothBlendData() noexcept;
		void updateMorphBlendData() noexcept;
		void updateTextureBlendData() noexcept;

	private:
		SkinnedMeshRendererComponent(const SkinnedMeshRendererComponent&) = delete;
		SkinnedMeshRendererComponent& operator=(const SkinnedMeshRendererComponent&) = delete;

	private:
		bool needUpdate_;
		bool clothEnable_;
		bool morphEnable_;
		bool textureEnable_;

		GameObjects transforms_;

		math::float4x4s joints_;
		hal::GraphicsDataPtr jointData_;

		mesh::MeshPtr mesh_;
		mesh::MeshPtr skinnedMesh_;

		std::vector<class ClothComponent*> clothComponents_;
		std::vector<class SkinnedMorphComponent*> morphComponents_;
		std::vector<class SkinnedTextureComponent*> textureComponents_;
	};
}

#endif