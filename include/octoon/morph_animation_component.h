#ifndef OCTOON_MORPH_ANIMATION_COMPONENT_H_
#define OCTOON_MORPH_ANIMATION_COMPONENT_H_

#include <octoon/animation/animation.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT MorphAnimationComponent final : public GameComponent
	{
		OctoonDeclareSubClass(MorphAnimationComponent, GameComponent)
	public:
		MorphAnimationComponent() noexcept;
		explicit MorphAnimationComponent(math::float3s&& offsets, math::uint1s&& indices) noexcept;
		explicit MorphAnimationComponent(const math::float3s& vertices, const math::uint1s& indices) noexcept;
		virtual ~MorphAnimationComponent() noexcept;
	
		void play() noexcept;
		void pause() noexcept;
		void stop() noexcept;

		void setTime(float time) noexcept;

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

		void onFixedUpdate() except override;

	private:
		void update(float delta = 0.0f) noexcept;

	private:
		MorphAnimationComponent(const MorphAnimationComponent&) = delete;
		MorphAnimationComponent& operator=(const MorphAnimationComponent&) = delete;

	private:
		float time_;
		float timeStep_;
		float timeInterval_;

		math::uint1s indices_;
		math::float3s offsets_;
	};
}

#endif