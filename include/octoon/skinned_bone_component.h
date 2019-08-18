#ifndef OCTOON_SKINNED_BONE_COMPONENT_H_
#define OCTOON_SKINNED_BONE_COMPONENT_H_

#include <octoon/animation/animation.h>
#include <octoon/skinned_component.h>

namespace octoon
{
	class OCTOON_EXPORT SkinnedBoneComponent final : public SkinnedComponent
	{
		OctoonDeclareSubClass(SkinnedBoneComponent, SkinnedComponent)
	public:
		SkinnedBoneComponent() noexcept;
		explicit SkinnedBoneComponent(math::uint1s&& bones, math::float3s&& position, math::Quaternions&& rotation, float control = 0.0f) noexcept;
		explicit SkinnedBoneComponent(const math::uint1s& bones, const math::float3s& vertices, const math::Quaternions& rotation, float control = 0.0f) noexcept;
		virtual ~SkinnedBoneComponent() noexcept;

		void setPosition(math::float3s&& position) noexcept;
		void setPosition(const math::float3s& position) noexcept;
		const math::float3s& getPosition() const noexcept;

		void setRotation(math::Quaternions&& rotation) noexcept;
		void setRotation(const math::Quaternions& rotation) noexcept;
		const math::Quaternions& getRotation() const noexcept;

		void setBones(math::uint1s&& bones) noexcept;
		void setBones(const math::uint1s& bones) noexcept;
		const math::uint1s& getBones() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onAnimationUpdate(const runtime::any& mesh) noexcept;
		void onTargetReplace(const std::string& name) noexcept override;

	private:
		SkinnedBoneComponent(const SkinnedBoneComponent&) = delete;
		SkinnedBoneComponent& operator=(const SkinnedBoneComponent&) = delete;

	private:
		math::uint1s bones_;
		math::float3s position_;
		math::Quaternions rotation_;
	};
}

#endif