#ifndef OCTOON_TRANSFORM_ANIM_COMPONENT_H_
#define OCTOON_TRANSFORM_ANIM_COMPONENT_H_

#include <octoon/model/animation_curve.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT TransformAnimComponent final : public GameComponent
	{
		OctoonDeclareSubClass(TransformAnimComponent, GameComponent)
	public:
		TransformAnimComponent() noexcept;
		virtual ~TransformAnimComponent() noexcept;

		void setScale(model::Keyframes<math::float3>&& frames) noexcept;
		void setRotation(model::Keyframes<math::float3>&& frames) noexcept;
		void setTranslate(model::Keyframes<math::float3>&& frames) noexcept;

		void setScale(const model::Keyframes<math::float3>& frames) noexcept;
		void setRotation(const model::Keyframes<math::float3>& frames) noexcept;
		void setTranslate(const model::Keyframes<math::float3>& frames) noexcept;

		void setScale(model::AnimationCurve<math::float3>&& frames) noexcept;
		void setRotation(model::AnimationCurve<math::float3>&& frames) noexcept;
		void setTranslate(model::AnimationCurve<math::float3>&& frames) noexcept;

		void setScale(const model::AnimationCurve<math::float3>& frames) noexcept;
		void setRotation(const model::AnimationCurve<math::float3>& frames) noexcept;
		void setTranslate(const model::AnimationCurve<math::float3>& frames) noexcept;

		void setLocalScale(model::Keyframes<math::float3>&& frames) noexcept;
		void setLocalRotation(model::Keyframes<math::float3>&& frames) noexcept;
		void setLocalTranslate(model::Keyframes<math::float3>&& frames) noexcept;

		void setLocalScale(const model::Keyframes<math::float3>& frames) noexcept;
		void setLocalRotation(const model::Keyframes<math::float3>& frames) noexcept;
		void setLocalTranslate(const model::Keyframes<math::float3>& frames) noexcept;

		void setLocalScale(model::AnimationCurve<math::float3>&& frames) noexcept;
		void setLocalRotation(model::AnimationCurve<math::float3>&& frames) noexcept;
		void setLocalTranslate(model::AnimationCurve<math::float3>&& frames) noexcept;

		void setLocalScale(const model::AnimationCurve<math::float3>& frames) noexcept;
		void setLocalRotation(const model::AnimationCurve<math::float3>& frames) noexcept;
		void setLocalTranslate(const model::AnimationCurve<math::float3>& frames) noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void onActivate() override;
		virtual void onDeactivate() noexcept override;
		
		virtual void onFrame() except;

	private:
		TransformAnimComponent(const TransformAnimComponent&) = delete;
		TransformAnimComponent& operator=(const TransformAnimComponent&) = delete;

	private:
		model::AnimationCurve<math::float3> pos_;
		model::AnimationCurve<math::float3> scale_;
		model::AnimationCurve<math::float3> rotation_;
		model::AnimationCurve<math::float3> localPos_;
		model::AnimationCurve<math::float3> localScale_;
		model::AnimationCurve<math::float3> localRotation_;
	};
}

#endif