#ifndef OCTOON_ANIMATION_TRANSFORM_COMPONENT_H_
#define OCTOON_ANIMATION_TRANSFORM_COMPONENT_H_

#include <octoon/model/animation_curve.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT AnimationTransformComponent final : public GameComponent
	{
		OctoonDeclareSubClass(AnimationTransformComponent, GameComponent)
	public:
		AnimationTransformComponent() noexcept;
		virtual ~AnimationTransformComponent() noexcept;

		void setScale(model::AnimationCurve<math::float3>&& frames) noexcept;
		void setAnchorPoint(model::AnimationCurve<math::float3>&& frames) noexcept;
		void setTranslate(model::AnimationCurve<math::float3>&& frames) noexcept;
		void setOrientation(model::AnimationCurve<math::float3>&& frames) noexcept;
		void setRotationX(model::AnimationCurve<math::float1>&& frames) noexcept;
		void setRotationY(model::AnimationCurve<math::float1>&& frames) noexcept;
		void setRotationZ(model::AnimationCurve<math::float1>&& frames) noexcept;

		void setScale(const model::AnimationCurve<math::float3>& frames) noexcept;
		void setAnchorPoint(const model::AnimationCurve<math::float3>& frames) noexcept;
		void setTranslate(const model::AnimationCurve<math::float3>& frames) noexcept;
		void setOrientation(const model::AnimationCurve<math::float3>& frames) noexcept;
		void setRotationX(const model::AnimationCurve<math::float1>& frames) noexcept;
		void setRotationY(const model::AnimationCurve<math::float1>& frames) noexcept;
		void setRotationZ(const model::AnimationCurve<math::float1>& frames) noexcept;

		void setScale(model::Keyframes<math::float3>&& frames) noexcept;
		void setAnchorPoint(model::Keyframes<math::float3>&& frames) noexcept;
		void setTranslate(model::Keyframes<math::float3>&& frames) noexcept;
		void setOrientation(model::Keyframes<math::float3>&& frames) noexcept;
		void setRotationX(model::Keyframes<math::float1>&& frames) noexcept;
		void setRotationY(model::Keyframes<math::float1>&& frames) noexcept;
		void setRotationZ(model::Keyframes<math::float1>&& frames) noexcept;

		void setScale(const model::Keyframes<math::float3>& frames) noexcept;
		void setAnchorPoint(const model::Keyframes<math::float3>& frames) noexcept;
		void setTranslate(const model::Keyframes<math::float3>& frames) noexcept;
		void setOrientation(const model::Keyframes<math::float3>& frames) noexcept;
		void setRotationX(const model::Keyframes<math::float1>& frames) noexcept;
		void setRotationY(const model::Keyframes<math::float1>& frames) noexcept;
		void setRotationZ(const model::Keyframes<math::float1>& frames) noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void onActivate() override;
		virtual void onDeactivate() noexcept override;

		virtual void onFrame() except;

	private:
		AnimationTransformComponent(const AnimationTransformComponent&) = delete;
		AnimationTransformComponent& operator=(const AnimationTransformComponent&) = delete;

	private:
		model::AnimationCurve<math::float3> pos_;
		model::AnimationCurve<math::float3> scale_;
		model::AnimationCurve<math::float3> anchor_;
		model::AnimationCurve<math::float3> orientation_;
		model::AnimationCurve<math::float1> rx_;
		model::AnimationCurve<math::float1> ry_;
		model::AnimationCurve<math::float1> rz_;
	};
}

#endif