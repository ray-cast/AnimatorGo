#ifndef OCTOON_SKINNED_MORPH_COMPONENT_H_
#define OCTOON_SKINNED_MORPH_COMPONENT_H_

#include <octoon/animation/animation.h>
#include <octoon/skinned_component.h>

namespace octoon
{
	class OCTOON_EXPORT SkinnedMorphComponent final : public SkinnedComponent
	{
		OctoonDeclareSubClass(SkinnedMorphComponent, SkinnedComponent)
	public:
		SkinnedMorphComponent() noexcept;
		explicit SkinnedMorphComponent(math::float3s&& offsets, math::uint1s&& indices, float control = 0.0f) noexcept;
		explicit SkinnedMorphComponent(const math::float3s& vertices, const math::uint1s& indices, float control = 0.0f) noexcept;
		virtual ~SkinnedMorphComponent() noexcept;

		void setOffsets(math::float3s&& offsets) noexcept;
		void setOffsets(const math::float3s& offsets) noexcept;
		const math::float3s& getOffsets() const noexcept;

		void setIndices(math::uint1s&& indices) noexcept;
		void setIndices(const math::uint1s& indices) noexcept;
		const math::uint1s& getIndices() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onAnimationUpdate(const runtime::any& mesh) noexcept;
		void onTargetReplace(const std::string& name) noexcept override;

	private:
		SkinnedMorphComponent(const SkinnedMorphComponent&) = delete;
		SkinnedMorphComponent& operator=(const SkinnedMorphComponent&) = delete;

	private:
		math::uint1s indices_;
		math::float3s offsets_;
	};
}

#endif