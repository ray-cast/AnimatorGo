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
		SkinnedMeshRendererComponent(Materials&& materials, GameObjects&& bones) noexcept;
		SkinnedMeshRendererComponent(MaterialPtr&& material, GameObjects&& bones) noexcept;
		SkinnedMeshRendererComponent(const Materials& materials, const GameObjects& bones) noexcept;
		SkinnedMeshRendererComponent(const MaterialPtr& material, const GameObjects& bones) noexcept;
		virtual ~SkinnedMeshRendererComponent() noexcept;

		void setBones(const GameObjects& bones) noexcept;
		void setBones(GameObjects&& bones) noexcept;
		const GameObjects& getBones() const noexcept;

		void setAutomaticUpdate(bool enable) noexcept;
		void setClothBlendEnable(bool enable) noexcept;
		void setMorphBlendEnable(bool enable) noexcept;
		void setTextureBlendEnable(bool enable) noexcept;
		void setUpdateWhenOffscreen(bool enable) noexcept;

		bool getAutomaticUpdate() const noexcept;
		bool getClothBlendEnable() const noexcept;
		bool getMorphBlendEnable() const noexcept;
		bool getTextureBlendEnable() const noexcept;
		bool getUpdateWhenOffscreen() const noexcept;

		const MeshPtr& getSkinnedMesh() noexcept;

		void uploadMeshData() noexcept;
		void uploadMeshData(const MeshPtr& mesh) noexcept override;

		void load(const nlohmann::json& json) noexcept(false) override;
		void save(nlohmann::json& json) const noexcept(false) override;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onFixedUpdate() noexcept override;
		void onAnimationUpdate(const std::any& mesh) noexcept;

		void onAttachComponent(const GameComponentPtr& component) noexcept override;
		void onDetachComponent(const GameComponentPtr& component) noexcept override;

		void onMeshReplace(const std::any& mesh) noexcept;

		void onPreRender(const Camera& camera) noexcept override;

	private:
		void updateJointData() noexcept;
		void updateBoneData() noexcept;
		void updateClothBlendData() noexcept;
		void updateMorphBlendData() noexcept;
		void updateTextureBlendData() noexcept;
		void updateMeshData() noexcept;

	private:
		SkinnedMeshRendererComponent(const SkinnedMeshRendererComponent&) = delete;
		SkinnedMeshRendererComponent& operator=(const SkinnedMeshRendererComponent&) = delete;

	private:
		bool needUpdate_;
		bool clothEnable_;
		bool morphEnable_;
		bool textureEnable_;
		bool automaticUpdate_;
		bool updateWhenOffscreenEnable_;

		MeshPtr mesh_;
		MeshPtr skinnedMesh_;

		GameObjects bones_;

		math::float4x4s joints_;
		GraphicsDataPtr jointData_;

		std::vector<math::Quaternion> quaternions_;
		std::vector<class ClothComponent*> clothComponents_;
		std::vector<class SkinnedMorphComponent*> morphComponents_;
		std::vector<class SkinnedTextureComponent*> textureComponents_;
	};
}

#endif