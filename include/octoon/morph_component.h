#ifndef OCTOON_MORPH_COMPONENT_H_
#define OCTOON_MORPH_COMPONENT_H_

#include <octoon/animation/animation.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT MorphComponent final : public GameComponent
	{
		OctoonDeclareSubClass(MorphComponent, GameComponent)
	public:
		MorphComponent() noexcept;
		explicit MorphComponent(const std::string& name) noexcept;
		explicit MorphComponent(math::float3s&& offsets, math::uint1s&& indices, float control = 0.0f) noexcept;
		explicit MorphComponent(const math::float3s& vertices, const math::uint1s& indices, float control = 0.0f) noexcept;
		virtual ~MorphComponent() noexcept;
	
		void setControl(float control) noexcept;
		float getControl() const noexcept;

		void setOffsets(math::float3s&& offsets) noexcept;
		void setOffsets(const math::float3s& offsets) noexcept;
		const math::float3s& getOffsets() const noexcept;

		void setIndices(math::uint1s&& indices) noexcept;
		void setIndices(const math::uint1s& indices) noexcept;
		const math::uint1s& getIndices() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onAnimationUpdate(const runtime::any& mesh) noexcept;

	private:
		void update(float delta = 0.0f) noexcept;

	private:
		MorphComponent(const MorphComponent&) = delete;
		MorphComponent& operator=(const MorphComponent&) = delete;

	private:
		float control_;

		math::uint1s indices_;
		math::float3s offsets_;
	};
}

#endif