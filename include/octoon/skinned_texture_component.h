#ifndef OCTOON_SKINNED_TEXTURE_COMPONENT_H_
#define OCTOON_SKINNED_TEXTURE_COMPONENT_H_

#include <octoon/animation/animation.h>
#include <octoon/skinned_component.h>

namespace octoon
{
	class OCTOON_EXPORT SkinnedTextureComponent final : public SkinnedComponent
	{
		OctoonDeclareSubClass(SkinnedTextureComponent, SkinnedComponent)
	public:
		SkinnedTextureComponent() noexcept;
		explicit SkinnedTextureComponent(math::float2s&& offsets, math::uint1s&& indices, float control = 0.0f) noexcept;
		explicit SkinnedTextureComponent(const math::float2s& vertices, const math::uint1s& indices, float control = 0.0f) noexcept;
		virtual ~SkinnedTextureComponent() noexcept;

		void setOffsets(math::float2s&& offsets) noexcept;
		void setOffsets(const math::float2s& offsets) noexcept;
		const math::float2s& getOffsets() const noexcept;

		void setIndices(math::uint1s&& indices) noexcept;
		void setIndices(const math::uint1s& indices) noexcept;
		const math::uint1s& getIndices() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onAnimationUpdate(const std::any& mesh) noexcept;
		void onTargetReplace(std::string_view name) noexcept override;

	private:
		SkinnedTextureComponent(const SkinnedTextureComponent&) = delete;
		SkinnedTextureComponent& operator=(const SkinnedTextureComponent&) = delete;

	private:
		math::uint1s indices_;
		math::float2s offsets_;
	};
}

#endif